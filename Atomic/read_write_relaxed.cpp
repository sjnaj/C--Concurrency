#include <atomic>
#include <thread>
#include <cassert>
#include <iostream>
#include <chrono>
#include <iostream>
#include <vector>

std::atomic<bool> x, y;
std::atomic<bool> flag, go;
void write_x_then_y()
{
    while (!go)
        std::this_thread::yield();
    x.store(true, std::memory_order_relaxed);
    y.store(true, std::memory_order_relaxed);
}

void read_y_then_x()
{
    while (!go)
        std::this_thread::yield();

    // while (!y.load(std::memory_order_relaxed))
    //     ;
    // assert(x.load(std::memory_order_relaxed));

    // theoretically would break,but hard to get in this example
    // Probably because adjacent variables in a thread are synchronized together
}
int main()
{

    for (int i = 0; i < 10; i++)
    {
        x = y = false;
        go = false;
        std::thread t1(write_x_then_y);
        std::thread t2(read_y_then_x);
        std::thread t3(read_y_then_x);
        std::thread t4(read_y_then_x);
        go = true;
        t1.join();
        t2.join();
        t3.join();
        t4.join();
    }
}