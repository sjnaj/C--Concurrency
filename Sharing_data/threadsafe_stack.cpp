#include <iostream>
#include <memory>
#include <thread>
#include <stack>
#include <deque>
#include <vector>
#include <exception>
#include <mutex>
#include <type_traits>
#include <algorithm>
#include <functional>
#include <stdexcept>

struct empty_stack : std::exception
{
    const char *what() const noexcept
    {
        return "stack is empty!\n";
    }
};

template <typename T>
class threadsafe_stack
{
private:
    std::stack<T> data;
    mutable std::mutex mutex;
public:
    threadsafe_stack() {}

    threadsafe_stack(const threadsafe_stack &other)
    {
        std::lock_guard<std::mutex> lock(other.mutex);
        data = other.data;
    }
    threadsafe_stack &operator=(const threadsafe_stack &) = delete;
    void push(T new_value)
    {
        std::lock_guard<std::mutex> lock(mutex);
        data.push(new_value);
    }
    T top()
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (data.empty())
            throw empty_stack();
        return data.top();
    }
    // combine top and pop to avoid race condition

    // use pointer to avoid return failure(alloc failure while copying) because it can be freely returned
    std::shared_ptr<T> pop() // throw(empty_stack) c++17 does not allow dynamic exception specification
    {
        std::lock_guard<std::mutex>
            lock(mutex);
        if (data.empty())
            throw empty_stack();
        std::shared_ptr<T> const res{std::make_shared<T>(data.top())};
        data.pop();
        return res;
    }
    // get the result before pop to prevents elements from being poped when return copy fails
    void pop(T &value) // similar to NRVO: named return value optimization
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (data.empty())
            throw empty_stack();
        value = data.top();
        data.pop();
    }
    // use no throw copy function to avoid element losting(terminate or processed by programmer) or no throw move function to avoid allocating  extra memory
    T safe_pop()
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (!(std::is_nothrow_copy_constructible<T>::value || std::is_nothrow_move_constructible<T>::value))
            throw std::runtime_error("unsafty usage");
        if (data.empty())
            throw empty_stack();
        T res = data.top();
        data.pop();
        return res; // RVO will optimize it to receiver=std::move(res),if move constructor exists
        // The compiler synthesizes a move constructor for a class only if it does not define any copy control functions of its own and all its data members can move constructs or move assignments
    }
    bool empty() const // superfluous
    {
        std::lock_guard<std::mutex> lock(mutex);
        return data.empty();
    }
};
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
