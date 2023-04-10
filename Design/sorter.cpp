#include <bits/stdc++.h>
#include "../Sharing_data/threadsafe_stack.h"
#include "../Synchronizing/sequential_quick_sort.h"
#include <boost/lockfree/stack.hpp>
#include <cmath>
#include "../Timer.h"
template <typename T>
struct sorter
{
    struct chunk_to_sort
    {
        std::list<T> data;
        std::promise<std::list<T>> promise;
        chunk_to_sort() = default;
        // implicit deleted,thus default =delete, since member promise is not copyable
        //  chunk_to_sort(const chunk_to_sort &)=default;

        // Call this copy function first,if is not defined,shared_ptr constructor will call default copy constructor,which is not exist
        chunk_to_sort(chunk_to_sort &chunk)
        {
            data = std::move(chunk.data);
            promise = std::move(chunk.promise);
        }
    };


    // threadsafe_stack<chunk_to_sort> chunks;//chunk_to_sort is not copy_constructible
    //wrap/raep/ by shared_ptr to use
    //     语义要求
    // 若 T 为对象类型，则 copy_constructible<T> 仅若符合下列条件才得到实现。给定

    // v ， T 类型左值（可为 const ）或为 const T 类型右值，
    // 下列为真：

    // 定义 T u = v; 后， u 等于 v 且 v 不被修改；
    // T(v) 等于 v 且不修改 v 。

    /*The stack class provides a multi-writer/multi-reader stack, pushing and popping is lock-free,
     *  construction/destruction has to be synchronized. It uses a freelist for memory management,
     *  freed nodes are pushed to the freelist and not returned to the OS before the stack is destroyed.
     */
    boost::lockfree::stack<std::shared_ptr<chunk_to_sort>> chunks;

    std::vector<std::thread> threads;
    const unsigned max_thread_count;
    std::atomic<bool> end_of_data;

    // must init size to lockfree stack
    sorter() : chunks(8), max_thread_count(std::thread::hardware_concurrency() - 1), end_of_data(false)
    {
    }
    //stop and reclaim threads
    ~sorter()
    {
        end_of_data = true;
        for (unsigned i = 0; i < threads.size(); i++)
        {
            threads[i].join();
        }
    }
    void try_sort_chunk()
    {
        std::shared_ptr<chunk_to_sort> chunk_ptr;
        if (chunks.pop(chunk_ptr))
        {
            sort_chunk(chunk_ptr);
        }
    }
    std::list<T> do_sort(std::list<T> &chunk_data)
    {
        if (chunk_data.size() <= 1)
        {
            return chunk_data;
        }
        std::list<T> result;
        result.splice(result.begin(), chunk_data, chunk_data.begin());
        const T &partition_val = *result.begin();
        typename std::list<T>::iterator divide_point = std::partition(chunk_data.begin(), chunk_data.end(), [&](const T &val)
                                                                      { return val < partition_val; });
        chunk_to_sort new_lower_chunk;
        new_lower_chunk.data.splice(new_lower_chunk.data.end(), chunk_data, chunk_data.begin(), divide_point);
        std::future<std::list<T>> new_lower = new_lower_chunk.promise.get_future();

        chunks.push(std::make_shared<chunk_to_sort>(new_lower_chunk));
        // add new worker thread
        if (threads.size() < max_thread_count)
        {
            threads.push_back(std::thread(&sorter<T>::sort_thread, this));
        }
        std::list<T> new_higher = do_sort(chunk_data); // do some split operation
        result.splice(result.end(), new_higher);
        // if other thread is busy then try process by it self
        while (new_lower.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
        {
            try_sort_chunk();
        }
        result.splice(result.begin(), new_lower.get());
        return result;
    }
    void sort_chunk(const std::shared_ptr<chunk_to_sort> &chunk)
    {
        chunk->promise.set_value(do_sort(chunk->data));
    }
    void sort_thread()
    {
        while (!end_of_data)
        {
            try_sort_chunk();
            std::this_thread::yield();
        }
    }
};
template <typename T>
std::list<T> parallel_quick_sort(std::list<T> input)
{
    if (input.size() <= 1)
    {
        return input;
    }
    sorter<T> s;
    return s.do_sort(input);
}
std::list<int> li(2000000);

int main()
{

    srand(time(0));
    std::generate(li.begin(), li.end(), []
                  { return rand() % 1000; });
    {
        Timer t1("sequential");
        auto sequential_sorted_list = sequential_quick_sort(li);
    }
    {
        Timer t2("parallel");
        // parallel faster than sequential while there has huge amount of data
        auto parallel_sorted_list = parallel_quick_sort(li);
    }
}