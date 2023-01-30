#include "hazard_pointer.h"
#include <memory>

template <typename T>
class lock_free_stack
{
private:
    struct node
    {
        std::shared_ptr<T> data;
        node *next;
        node(const T &data_) : data(std::make_shared<T>(data_)) {}
    };
    std::atomic<node *> head;
    std::atomic<unsigned> nodes_count;

public:
    std::shared_ptr<T> pop()
    {
        std::atomic<void *> &hp = get_hazard_pointer_for_current_thread();
        node *old_head = head.load();
        do
        {
            node *temp;
            do
            {
                temp = old_head;
                hp.store(old_head);
                old_head = head.load();
            } while (old_head != temp);

        } while (old_head && !head.compare_exchange_strong(old_head, old_head->next));
        hp.store(nullptr);
        std::shared_ptr<T> res;
        if (old_head)
        {
            res.swap(old_head->data);
            if (outstanding_hazard_pointer_for(old_head))
            {
                reclaim_later(old_head);
            }
            else
            {
                delete old_head;
            }
            delete_nodes_with_no_hazards();
        }
        return res;
    }
    // do not check every time
    std::shared_ptr<T> pop_effcient()
    {
        std::atomic<void *> &hp = get_hazard_pointer_for_current_thread();
        node *old_head = head.load();
        do
        {
            node *temp;
            do
            {
                temp = old_head;
                hp.store(old_head);
                old_head = head.load();
            } while (old_head != temp);

        } while (old_head && !head.compare_exchange_strong(old_head, old_head->next));
        hp.store(nullptr);
        std::shared_ptr<T> res;
        if (old_head)
        {
            res.swap(old_head->data);
            reclaim_later(old_head);
            nodes_count++;
            if (nodes_count.compare_exchange_strong(max_hazard_pointers * 2, max_hazard_pointers))//Expand the initial capacity expansion threshold，The remainder can be cleared at the end of the program
                ;
            {
                delete_nodes_with_no_hazards_with_limit(max_hazard_pointers);
            }
        }
        return res;
    }
    void push(const T &data)
    {
        node *const new_node = new node(data);
        new_node->next = head.load();
        int i = 0;
        while (!head.compare_exchange_weak(new_node->next, new_node))
            ;
    }
};