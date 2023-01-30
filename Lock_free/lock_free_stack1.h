#include <atomic>
#include <thread>
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
    std::atomic<unsigned> threads_in_pop;
    std::atomic<node *> to_be_deleted;

    static void delete_nodes(node *nodes)
    {
        while (nodes)
        {
            node *next = nodes->next;
            delete nodes;
            nodes = next;
        }
    }
    //find last
    void chain_pending_nodes(node *nodes)
    {
        node *last = nodes;
        while (node *const next = last->next)
        {
            last = next;
        }
        chain_pending_nodes(nodes, last);
    }
    void chain_pending_nodes(node *first, node *last)
    {
        last->next = to_be_deleted;
        while (!to_be_deleted.compare_exchange_weak(last->next, first))//splice:last->next=another first
            ;
    }
    void chain_pending_node(node *n)
    {
        chain_pending_nodes(n, n);
    }

    // in high-load situation,delete_nodes would be never be triggered,therefore make list unbounded
    void try_reclaim(node *old_head)
    {
        if (threads_in_pop == 1)// no other thread call pop,may can reclaim list
        {
            node *nodes_to_deleted = to_be_deleted.exchange(nullptr);//get pending list to be deleted and clear to_be_deleted
            if (!--threads_in_pop)// no other pop
            {
                delete_nodes(nodes_to_deleted);
            } 
            else if (nodes_to_deleted)//has other pop,add to chain
            {
                chain_pending_nodes(nodes_to_deleted);
            }
            //on other thread reference old_head
            delete old_head;//delete in the end to avoid too many other pops between two checks
        }
        else
        {
            chain_pending_node(old_head);
            threads_in_pop--;
        }
    }


public:
    void push(const T &data)
    {
        node *const new_node = new node(data);
        new_node->next = head.load();
        int i = 0;
        while (!head.compare_exchange_weak(new_node->next, new_node))
            ;
    }
    
    std::shared_ptr<T> pop()
    {
        threads_in_pop++;
        node *old_head = head.load();
        while (old_head && !head.compare_exchange_weak(old_head, old_head->next))
            ;
        std::shared_ptr<T> res;
        if (old_head)
        {
            res.swap(old_head->data);
        }
        try_reclaim(old_head);
        return res; // get value after modify,so should use shared_ptr to avoid execpton unsafey
    }
};

lock_free_stack<int> s;
void push()
{
    while (true)
    {
        s.push(1);
    }
}
int main()
{
    std::thread t1(push);
    std::thread t2(push);
    t1.join();
    t2.join();
}
