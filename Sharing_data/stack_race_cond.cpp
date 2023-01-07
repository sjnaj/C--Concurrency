#include <stack>
#include <deque>
#include <memory>
#include <vector>
#include <thread>
#include <iostream>
#include <algorithm>
#include <functional>
#include <assert.h>
#include <chrono>
std::stack<int> s(std::deque<int>{1, 2, 3, 4, 5}); // the underlying inplementation is deque
void do_something(int value) { std::cout << value; }
void foo()
{
    if (!s.empty())
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(100ms); // make error situation easy to arise
        assert(("isn't consistent",!s.empty()));
        const int value = s.top();
        s.pop();
        do_something(value);
    }
}
int main()
{
    std::vector<std::thread> threads(10);
    for (int i = 0; i < 10; i++)
    {
        threads[i] = std::thread(foo);
    }
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
}