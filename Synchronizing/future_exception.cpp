#include <future>
#include <cmath>
#include <iostream>
#include <thread>
#include <typeinfo>
std::promise<double> some_promise;

double squart_root(double x)
{
    if (x < 0)
    {
        throw std::out_of_range("x<0");
    }
    else
    {
        return sqrt(x);
    }
}
void foo(double x)
{
    try
    {
        some_promise.set_value(squart_root(x));
    }
    catch (...)
    {
        some_promise.set_exception(std::current_exception()); // para type: std::__exception_ptr
    }
}
void foo1(double x)
{
    try
    {
        some_promise.set_value(squart_root(x));
    }
    catch (const std::out_of_range &e)//either std::logic_error,if use std::exception execption string member would be lost while copy at make ptr
    {
        some_promise.set_exception(std::make_exception_ptr(e)); // Obtain an exception_ptr pointing to a copy of the supplied object.
    }
}
int main()
{
    // std::thread t{foo, -1}; // set and get can't in  the same thread
    std::thread t1{foo1, -1};
    auto f = some_promise.get_future();
    try
    {
        f.get();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    // t.join();
    t1.join();
}