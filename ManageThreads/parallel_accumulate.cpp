#include <iostream>
#include <numeric>
#include <thread>
#include <vector>
#include <functional>
#include <algorithm>
#include <chrono>
#include <iomanip>

struct Timer
{
    std::chrono::time_point<std::chrono::steady_clock> start, end;

// the high_resolution_clock is not implemented consistently across different standard library implementations, and its use should be avoided. 
// It is often just an alias for std::chrono::steady_clock or std::chrono::system_clock, but which one it is depends on the library or configuration. 
// When it is a system_clock, it is not monotonic (e.g., the time can go backwards). For example, for gcc's libstdc++ it is system_clock, 
// for MSVC it is steady_clock, and for clang's libc++ it depends on configuration.

// Generally one should just use std::chrono::steady_clock or std::chrono::system_clock directly instead of std::chrono::high_resolution_clock: use steady_clock for duration measurements, and system_clock for wall-clock time.

    Timer()
    {
        start = std::chrono::steady_clock::now();
    }
    ~Timer()
    {
        // using namespace  std::chrono_literals;
        // std::this_thread::sleep_for(1000ms);
        end = std::chrono::steady_clock::now();
        auto duration=std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
        std::cout << "Timer took" <<duration.count()<< "ms" << std::endl;
    }
};


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
    int result = parallel_accumulate(nums.begin(), nums.end(), 0);//1350ms
    // int result=std::accumulate(nums.begin(),nums.end(),0);//7370ms
    std::cout << result << std::endl;
}