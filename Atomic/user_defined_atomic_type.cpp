#include <atomic>
#include <iostream>
#include <type_traits>
#include <cassert>
struct foo
{
    int i;
};

// nontrival copyable
struct bar
{
    int a;

public:
    bar(const bar &) {}
};

class trivial_class // TriviallyCopyable type T satisfying both CopyConstructible and CopyAssignable.
{
public:
    int j;
    foo f;
    static bar b; // nontrival member should be static
};
int main()
{
    assert(std::is_trivially_copyable<trivial_class>::value);
    assert(std::is_copy_constructible<trivial_class>::value);
    assert(std::is_move_constructible<trivial_class>::value);
    assert(std::is_copy_assignable<trivial_class>::value);
    assert(std::is_move_assignable<trivial_class>::value);
    std::atomic<trivial_class> t{trivial_class()};
    std::cout << t.load().f.i << std::endl;
    // std::cout<<t.load().b.a<<std::endl;//cannot access static member by instantiation
}