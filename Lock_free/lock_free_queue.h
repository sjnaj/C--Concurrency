// #include <memory>
// #include <atomic>
// //Not implement
// template <typename T>
// class lock_free_queue
// {
// private:
//     struct node
//     {
//         std::atomic<T*> data;
//         std::shared_ptr<node> next;
//         node(const T &data_) : data(new T(data_)) {}
//     };

//     std::shared_ptr<node> head;
//     std::shared_ptr<node> tail;
//     std::shared_ptr<T> pop_head()
//     {
//         const std::shared_ptr<T> old_head = std::atomic_load(&head);
//         if (old_head == std::atomic_load(&tail))
//         {
//             return nullptr;
//         }
//         std::atomic_store(&head, head->next);
//     }

// public:

//     lock_free_queue(const lock_free_queue &) = delete;
//     lock_free_queue &operator=(const lock_free_queue &) = delete;

//     std::shared_ptr<T> pop()
//     {
//         std::shared_ptr<T> old_head =std::atomic_load(&head);
        
//         // std::shared_ptr<T> old_head = pop_head();
//         return old_head ? old_head->data : std::make_shared<T>();
//     }
//     void push(T new_value)
//     {
//         std::unique_ptr<T>new_data(new T(new_value));
//         T* old_data=nullptr;
//         while(!std::atomic_compare_exchange_weak(tail->data,old_data,new_data.get()));
//         // std::shared_ptr<node>old_tail=
//         old_tail->data.swap(new_data);
//         old_tail->next = std::move(p);
//         tail.store(p);
//     }
// };
