#include <thread>
#include <atomic>
#include <iostream>
std::atomic<int> x(0), y(0), z(0);
std::atomic<bool> go(false);
const unsigned loop_count = 10;
struct read_values
{
    int x, y, z;
};

read_values value1[loop_count];
read_values value2[loop_count];
read_values value3[loop_count];
read_values value4[loop_count];
read_values value5[loop_count];

void increment(std::atomic<int> *var_to_inc, read_values *values)
{
    while (!go)
        std::this_thread::yield();
    // The exact behavior of this function depends on the implementation, in particular on the mechanics of the OS scheduler in use and the state of the system. For example,
    //  a first-in-first-out realtime scheduler (SCHED_FIFO in Linux) would suspend the current thread and put it on the back of the queue of the same-priority threads that are ready to run (and if there are no other threads at the same priority, yield has no effect).

    for (unsigned i = 0; i < loop_count; ++i)
    {
        values[i].x = x.load(std::memory_order_relaxed);
        values[i].y = y.load(std::memory_order_relaxed);
        values[i].z = z.load(std::memory_order_relaxed);
        var_to_inc->store(i + 1, std::memory_order_relaxed);
        std::this_thread::yield();
    }
}
void read_vals(read_values *values)
{
    while (!go)
        std::this_thread::yield();
    for (unsigned i = 0; i < loop_count; ++i)
    {
        values[i].x = x.load(std::memory_order_relaxed);
        values[i].y = y.load(std::memory_order_relaxed);
        values[i].z = z.load(std::memory_order_relaxed);
        std::this_thread::yield();
    }
}
void print(read_values *v)
{
    for (unsigned i = 0; i < loop_count; ++i)
    {
        if (i)
            std::cout << ",";
        std::cout << "(" << v[i].x << "," << v[i].y << "," << v[i].z << ")";
    }
    std::cout << std::endl;
}

int main()
{
    std::thread t1(increment, &x, value1);
    std::thread t2(increment, &y, value2);
    std::thread t3(increment, &z, value3);
    std::thread t4(read_vals, value4);
    std::thread t5(read_vals, value5);

    go = true;
    t5.join();
    t4.join();
    t3.join();
    t2.join();
    t1.join();
    print(value1);
    print(value2);
    print(value3);
    print(value4);
    print(value5);
    // a more extreme result increment of a never be synchornize to other thread
    // (0,10,10),(1,10,10),(2,10,10),(3,10,10),(4,10,10),(5,10,10),(6,10,10),(7,10,10),(8,10,10),(9,10,10)
    // (0,0,0),(0,1,1),(0,2,2),(0,3,3),(0,4,3),(0,5,4),(0,6,5),(0,7,6),(0,8,7),(0,9,8)
    // (0,1,0),(0,2,1),(0,3,2),(0,4,3),(0,6,4),(0,7,5),(0,8,6),(0,9,7),(0,10,8),(0,10,9)
    // (0,0,0),(0,1,0),(0,2,1),(0,3,2),(0,3,3),(0,5,4),(0,6,4),(0,7,5),(0,7,6),(0,9,7)
    // (0,0,0),(0,1,0),(0,2,1),(0,3,2),(0,3,3),(0,5,4),(0,6,4),(0,7,5),(0,7,6),(0,8,6)
}
