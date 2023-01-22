#include "threadsafe_queue.h"
#include <iostream>
#include <thread>
#include <climits>
threadsafe_queue<int> q;
void push_thread()
{
    for (unsigned long long i = 0; i < ULLONG_MAX; i++)
    {
        q.push(i);
    }
}
void pop_thread()
{
    while (true)
    {
        if (auto ptr = q.try_pop())
        {
            std::cout << *ptr<<' ';
        }
    }
}
void pop_wait_thread()
{
    while (true)
    {
        int value;
        q.wait_and_pop(value);
        std::cout << value<<' ';
    }
}

int main()
{
    std::thread t1(push_thread);
    std::thread t2(push_thread);
    std::thread t3(pop_thread);
    std::thread t4(pop_thread);
    std::thread t5(pop_wait_thread);
    std::thread t6(pop_wait_thread);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
}