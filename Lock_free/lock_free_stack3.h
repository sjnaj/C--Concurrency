#include <iostream>
#include <memory>
// since atomic operation on shared_ptr at this mechine is lock free,so this is the most concise implementation
template <typename T>
class lock_free_stack
{
private:
    struct node
    {
        std::shared_ptr<T> data;
        std::shared_ptr<node>next;
        node(const T &data_) : data(std::make_shared<T>(data_))
        {
        }
    };
    std::shared_ptr<node> head;

public:
    ~lock_free_stack()
    {
        while (pop())
            ;
    }
    void push(const T &data)
    {
        const std::shared_ptr<node> new_node = std::make_shared<node>(data);
        new_node->next = std::atomic_load(&head);
        // Note the parameter type
        while (!std::atomic_compare_exchange_weak(&head, &new_node->next, new_node))
            ;
    }
    std::shared_ptr<T> pop()
    {
        std::shared_ptr<node> old_head = std::atomic_load(&head);
        while (old_head && !std::atomic_compare_exchange_weak(&head, &old_head, old_head->next))
            ;
        return old_head ? old_head->data : std::make_shared<T>();
    }
};
