#include <future>
#include <string>
#include <iostream>

struct X
{
    void foo(int, const std::string &);
    std::string bar(const std::string &);
};
void X::foo(int, const std::string &) {}
std::string X::bar(const std::string &) {}
X x;
auto f1 = std::async(&X::foo, &x, 42, "hello");
auto f2 = std::async(&X::bar, x, "goodbye");
struct Y
{
    double operator()(double arg)
    {
        std::cout << "start operate:" << arg << std::endl;
        return arg;
    };
};
Y y;
auto f3 = std::async(Y(), 3.14);
auto f4 = std::async(std::ref(y), 2.718);
X baz(X &) {}

auto f5 = std::async(baz, std::ref(x));

auto f8 = std::async(std::launch::async | std::launch::deferred, Y(), 1.25); // default

auto f6 = std::async(std::launch::async, Y(), 1.2); // run in new thread

auto f7 = std::async(std::launch::deferred, Y(), 1.3); // run in wait() or get()
int main()
{
    f7.wait();
    f8.wait();//in this run ,policy is std::launch::async
    f6.wait();
}