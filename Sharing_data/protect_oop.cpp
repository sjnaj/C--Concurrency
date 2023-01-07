#include <list>
#include <mutex>
#include <algorithm>
#include <thread>
#include <iostream>
#include <vector>
#include <functional>
class some_data
{
private:
    int a;
    std::string b;

public:
    void do_something() { a ++; }
    int get_a() const
    {
        return a;
    }
};
template <typename T>
class data_wapper
{
private:
    T data;
    mutable std::mutex m;//*

public:
    template <typename Function>
    void process_data(Function func)
    {
        std::lock_guard<std::mutex> guard(m);
        func(data);
    }
    T get_data() const
    {
        std::lock_guard<std::mutex> guard(m);
        return data;
    }
};
some_data *unprotected;
void malicious_function(some_data &protected_data)
{
    unprotected = &protected_data;
}
data_wapper x = data_wapper<some_data>();
void foo()
{
    x.process_data(malicious_function);
    for (int i = 0; i < 10000; i++)
    {
        unprotected->do_something();
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
    std::cout << x.get_data().get_a() << std::endl;//maybe not 10000*10
    //tips:
    //!don't pass pointer or reference to protected data outside the scope of the lock
}