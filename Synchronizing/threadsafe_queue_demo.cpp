#include "threadsafe_queue.h"
#include <thread>
#include "../acout.h"

threadsafe_queue<int> queue;
void push_thread()
{
    while (true)
    {
        queue.push(1);
    }
}
void try_pop_thread()
{
    while (true)
    {
        int value;
        if (queue.try_pop(value))
            acout() << value << " ";
    }
}
void wait_pop_thread()
{
    while (true)
    {
        int value;
        queue.wait_and_pop(value);
        acout() << value << " ";
    }   
}
int main()
{
    std::thread t1{push_thread};
    std::thread t2{try_pop_thread};
    std::thread t3{wait_pop_thread};
    t1.join();
    t2.join();
    t3.join();
}