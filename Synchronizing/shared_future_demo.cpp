#include <future>
#include <cassert>
#include <iostream>
#include <thread>
std::promise<int> p_global;
auto sf = p_global.get_future().share();
void foo()
{
    auto sf_local = sf;
    std::cout << sf.get() << std::endl;
}

int main()
{
    std::promise<int> p;
    std::future<int> f = p.get_future();
    assert(f.valid());
    std::shared_future<int> sf(std::move(f)); // only moveable
    assert(!f.valid());
    assert(sf.valid());
    auto sf2 = sf; // either movable or copyable
    assert(sf2.valid());

    p_global.set_value(1);
    std::thread t1{foo};
    std::thread t2{foo};
    t1.join();
    t2.join();
}