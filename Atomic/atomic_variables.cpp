#include <atomic>
#include <cassert>
std::atomic<bool> g_b;
int main()
{

    std::atomic<bool> b(true);
    b = false;
    bool x = b.load(std::memory_order_acquire);
    b.store(true);
    x = b.exchange(false, std::memory_order_acq_rel);

    bool expected = false;
    extern std::atomic<bool> g_b;
    while (!g_b.compare_exchange_weak(expected, true) && !expected) // loop can prevent the spurious failure because some machine lack a single compare-and-exchanfe instruction(Most RISC processors do not have a compare-exchange instruction) ,compare_exchange_strong contains a loop to avoid
        // for simple type or it's already  used in a loop ,use weak can avoid a double loop
        ;

    // these two are equal
    // in trival situation,falure orderis same to success
    b.compare_exchange_weak(expected, true, std::memory_order_acq_rel, std::memory_order_acquire);
    b.compare_exchange_weak(expected, true, std::memory_order_acq_rel);

    class Foo
    {
    };
    Foo some_array[5];
    std::atomic<Foo *> p(some_array);
    Foo *t = p.fetch_add(2);
    assert(t == some_array);//return old value
    assert(p.load() == &some_array[2]);
    t = (p -= 1);
    assert(t == &some_array[1]);
    assert(p.load() == &some_array[1]);
    p.fetch_add(3,std::memory_order_release);
}
