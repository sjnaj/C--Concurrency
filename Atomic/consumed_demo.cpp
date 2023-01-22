#include <atomic>
#include <thread>
#include <cassert>
#include <chrono>
struct X
{
    int i;
    std::string s;
};

std::atomic<X *> p;
std::atomic<int> a;

void create_X()
{
    X *x = new X;
    x->i = 42;
    x->s = "hello";
    a.store(99, std::memory_order_relaxed);
    p.store(x, std::memory_order_release);
}
void use_x()
{
    X *x;
    while (!(x == p.load(std::memory_order_consume)))//synchronize occur,but don't synchronize the variable happen-before p
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    //guarantee
    assert(x->i == 42);
    assert(x->s == "hello");
    //no guarantee,because a isn't dependent on the p
    assert(a.load(std::memory_order_relaxed) == 99);
}
int global_data[] = {1, 1, 1};
void do_some_thing(int) {}
std::atomic<int> index;
void f()
{
    int i = index.load(std::memory_order_consume);
    do_some_thing(global_data[std::kill_dependency(i)]);//i no need to wait load
}
int main()
{
    std::thread t1(create_X);
    std::thread t2(use_x);
    t1.join();
    t2.join();
}