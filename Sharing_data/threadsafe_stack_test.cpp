#include "threadsafe_stack.h"
#include <iostream>
threadsafe_stack<std::vector<int>> s;
template <typename T>
void do_something(const std::vector<T> vec)
{
    std::cout << vec[0];
}
void foo()
{

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(100ms); // make error situation easy to arise
    try
    {

        const std::vector<int> value = s.safe_pop();
        do_something(value);
    }
    catch (const empty_stack &)
    {
    }
}

void foo2()
{

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(100ms); // make error situation easy to arise
    try
    {
        const std::vector<int> value = *(s.pop().get());
        do_something(value);
    }
    catch (const empty_stack &)
    {
    }
}
void foo3()
{

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(100ms); // make error situation easy to arise
    try
    {

        std::vector<int> value;
        s.pop(value);
        do_something(value);
    }
    catch (const empty_stack &)
    {
    }
}
int main()
{
    for (int i = 0; i < 8; i++)
    {
        s.push(std::vector<int>(10, i));
    }
    std::vector<std::thread> threads(10);
    for (int i = 0; i < 3; i++)
    {
        threads[i] = std::thread(foo);
    }
    for (int i = 3; i < 6; i++)
    {
        threads[i] = std::thread(foo2);
    }
    for (int i = 6; i < 10; i++)
    {
        threads[i] = std::thread(foo3);
    }
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
}
