#include <iostream>
#include <numeric>
#include <thread>
#include <vector>
#include <functional>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include "../Timer.h"



template <typename Iterator, typename T>
struct accumulata_block
{
    void operator()(Iterator frist, Iterator last, T &result)
    {
        result = std::accumulate(frist, last, result);
    }
};
template <typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
    const unsigned long length = std::distance(first, last);
    if (!length)
        return init;
    const unsigned long min_per_thread = 25;
    const unsigned long max_threads = (length + min_per_thread - 1) / min_per_thread; //*equals to length/min_per_thread+length%min_per_length?1:0;
    const unsigned long hardware_threads = std::thread::hardware_concurrency();
    // std::cout<<hardware_threads<<std::endl;//8 in this machine
    const unsigned long num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
    const unsigned long block_size = length / num_threads;
    std::vector<T> results(num_threads);
    std::vector<std::thread> threads(num_threads - 1); // a thread is main thread

    Iterator block_start = first;
    for (unsigned long i = 0; i < num_threads - 1; i++)
    {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        threads[i]=std::thread(accumulata_block<Iterator, T>(), block_start, block_end,std::ref(results[i]));
        block_start = block_end;
    }

    accumulata_block<Iterator, T>()(block_start, last, results[num_threads - 1]);
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
    return std::accumulate(results.begin(), results.end(), init);
}
int main()
{
    std::vector<int> nums(1000000000, 1);
    Timer t;
    int result = parallel_accumulate(nums.begin(), nums.end(), 0);
    // int result=std::accumulate(nums.begin(),nums.end(),0);
    std::cout << result << std::endl;
}