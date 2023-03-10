#include <mutex>
#include <memory>
template <typename T>
class threadsafe_list
{
private:
    struct node
    {
        std::mutex m;
        std::shared_ptr<T> data;
        std::unique_lock<node> next;
        node() : next() {}
        node(const T &value) : data(value) {}
    };
    node head;

public:
    threadsafe_list(/* args */);
    ~threadsafe_list()
    {
        remove_if([](const node &)
                  { return true; });
    }
    threadsafe_list(const threadsafe_list &) = delete;
    threadsafe_list &operator=(const threadsafe_list &) = delete;
    void push_front(const T &value)
    {
        std::unique_ptr<node> new_node(new node(value));
        std::lock_guard<std::mutex> lk(head.m);
        new_node->next = std::remove(head.next);
        head.next = std::move(new_node);
    }
    template <typename Function>
    void for_each(Function f)
    {
        node *current = &head;
        std::unique_lock<std::mutex> lk(head.m);//lock next node under the protection of  current node
        while (node *const next = current->next.get())
        {
            std::unique_lock<std::mutex> next_lk(next->m);
            lk.unlock();
            f(*next->data);
            current = next;
            lk = std::move(next_lk);
        }
    }
    template <typename Predicate>
    std::shared_ptr<T> find_first_if(Predicate p)
    {
        node *current = &head;
        std::unique_lock<std::mutex> lk(head.m);
        while (node *const next = current->next.get())
        {
            std::unique_lock<std::mutex> next_lk(next->m);
            lk.unlock();
            if (p(*next->data))
            {
                return next->data;
            }
            current = next;
            lk = std::move(next_lk);
        }
        return std::shared_ptr<T>();
    }
    template <typename Predicate>
    void remove_if(Predicate p)
    {
        node *current = &head;
        std::unique_lock<std::mutex> lk(head.m);
        while (node *const next = current->next.get())
        {
            std::unique_lock<std::mutex> next_lk(next->m);
            if (p(*next->data))
            {
                std::unique_ptr<node> old_next = std::move(current->next);
                current->next = std::move(next->next);
                next_lk.unlock();
            }
            else
            {
                lk.unlock();
                current = next;
                lk = std::move(next_lk);
            }
        }
    }
};
