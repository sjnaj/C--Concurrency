#include <atomic>
#include <thread>
#include <cassert>

std::atomic<bool> x, y;
std::atomic<int> z;
void write_x_then_y()
{
    x.store(true, std::memory_order_relaxed);
    y.store(true, std::memory_order_release);
}
void read_y_then_x()
{
    while (!y.load(std::memory_order_acquire))
        ;
    if (x.load(std::memory_order_relaxed))
        z++;
}
void read_x_then_y()
{
    while (!x.load(std::memory_order_acquire))
        ;
    if (y.load(std::memory_order_relaxed))
        z++;
}
int main()
{
    x = false;
    y = false;
    z = 0;
    std::thread t(write_x_then_y);
    std::thread t1(read_x_then_y);
    std::thread t2(read_y_then_x);
    t.join();
    t1.join();
    t2.join();
    assert(z != 0);
}