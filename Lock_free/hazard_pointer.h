#include <atomic>
#include <thread>
#include <stdexcept>
#include <functional>
const unsigned max_hazard_pointers = 100;
struct hazard_pointer
{
    std::atomic<std::thread::id> id;
    std::atomic<void *> pointer;
};
hazard_pointer hazard_pointers[max_hazard_pointers];

class hp_owner
{
    hazard_pointer *hp;

public:
    hp_owner(const hp_owner &) = delete;
    hp_owner &operator=(const hp_owner &) = delete;
    hp_owner() : hp(nullptr)
    {
        for (unsigned i = 0; i < max_hazard_pointers; ++i)
        {
            std::thread::id old_id;
            // get a free slot
            if (hazard_pointers[i].id.compare_exchange_strong(old_id, std::this_thread::get_id()))
            {
                hp = &hazard_pointers[i];
                break;
            }
        }
        if (!hp)
        {
            throw std::runtime_error("No hazard pointers available");
        }
    }
    std::atomic<void *> &get_pointer()
    {
        return hp->pointer;
    }
    ~hp_owner()
    {
        hp->pointer.store(nullptr);
        hp->id.store(std::thread::id());
    }
};
std::atomic<void *> &get_hazard_pointer_for_current_thread()
{
    thread_local static hp_owner hazard;
    return hazard.get_pointer();
}
bool outstanding_hazard_pointer_for(void *p)
{
    for (unsigned i = 0; i < max_hazard_pointers; ++i)
    {
        if (hazard_pointers[i].pointer.load() == p)
        {
            return true;
        }
    }
    return false;
}
template <typename T>
void do_delete(void *p)
{
    delete static_cast<T *>(p);
}
struct data_to_claim
{
    void *data;
    std::function<void(void *)> deleter;
    data_to_claim *next;
    template <typename T>
    data_to_claim(T *p) : data(p), deleter(&do_delete), next(nullptr) {}
    ~data_to_claim()
    {
        deleter(data);
    }
};

std::atomic<data_to_claim *> nodes_to_reclaim;
void add_to_reclaim_list(data_to_claim *node)
{
    node->next = nodes_to_reclaim;
    while (!nodes_to_reclaim.compare_exchange_weak(node->next, node))
        ;
}
template <typename T>
void reclaim_later(T *data)
{
    add_to_reclaim_list(new data_to_claim(data));
}
void delete_nodes_with_no_hazards()
{
    data_to_claim *current = nodes_to_reclaim.exchange(nullptr);
    while (current)
    {
        data_to_claim *const next = current->next;
        if (!outstanding_hazard_pointer_for(current->data))
        {
            delete current;
        }
        else
        {
            add_to_reclaim_list(current);
        }
        current = next;
    }
}
void delete_nodes_with_no_hazards_with_limit(unsigned limits)
{
    data_to_claim *current = nodes_to_reclaim.exchange(nullptr);
    while (current)
    {
        data_to_claim *const next = current->next;
        if (!outstanding_hazard_pointer_for(current->data) && limits--)
        {

            delete current;
        }
        else
        {
            add_to_reclaim_list(current);
        }
        current = next;
    }
}