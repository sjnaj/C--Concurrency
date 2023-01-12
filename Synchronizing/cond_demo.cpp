#include <mutex>
#include <queue>
#include <condition_variable>


// std::mutex mutex;
// std::queue<data_chunk> data_queue;
// std::condition_variable data_cond;

// void data_preparation_thread()
// {
//     while (more_data_to_prepare())
//     {
//         const data_chunk data = prepare_data();
//         std::lock_guard<std::mutex> lk(mutex);
//         data_queue.push(data);
//         data_cond.notify_one();
//     }
// }
// void data_processing_thread()
// {
//     while (true)
//     {
//         std::unique_lock<std::mutex> lk(mutex);
//         data_cond.wait(lk, []
//                        { return !data_queue.empty(); });
//         data_chunk data = data_queue.front();
//         data_queue.pop();
//         lk.unlock();
//         process(data);
//         if (is_last_chunk(data))
//             break;
//     }
// }