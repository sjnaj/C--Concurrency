#include <atomic>
#include <thread>
std::atomic<int> sync(0);
void thread_1()
{
    //...
    sync.store(1, std::memory_order_relaxed);
}
void thread_2()
{
    int expected = 1;
    while (!sync.compare_exchange_strong(expected, 2, std::memory_order_acq_rel))//acquired read and release write
        ;
}
void thread_3()
{
    while (sync.load(std::memory_order_acquire) < 2)
        ;
        //...
}
int main()
{
    std::thread t1(thread_1);
    std::thread t2(thread_2);
    std::thread t3(thread_3);
    t1.join();
    t2.join();
    t3.join();
}