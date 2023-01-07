#include <iostream>
template <unsigned size>
struct test
{
    /* data */
};
constexpr int square(int x)
{
    return x * x;
}

// literal type class,could be treated as constant expression

class CX
{
private:
    int a;
    int b;

public:
    CX() = default;
    CX(int a_, int b_) : a(a_), b(b_) {}
    int get_a() const { return a; }
    int get_b() const { return b; }
    int foo() const { return a + b; }
};

constexpr CX create_cx()
{
    return CX();
}
constexpr CX clone(CX val)
{
    return val;
}

class CY
{
private:
    int a;
    int b;

public:
    CY() = default;
    constexpr CY(int a_, int b_) : a(a_), b(b_) {}
    constexpr int get_a() // const is omitted,because object is a constexpr
    {
        return a;
    }
    constexpr int get_b() { return b; };
    constexpr int foo() { return a + b; }
};
constexpr CY make_cy(int a)
{
    return CY(a, 1);
}
constexpr CY half_double(CY old)
{
    return CY(old.get_a() / 2, old.get_b() * 2);
}
constexpr int foo_squared(CY val)
{
    return square(val.foo());
}

template <typename T>
constexpr T sum(T a, T b)
{
    return a + b;
}
int main()
{
    int bounds = 99;
    int array[bounds]; // error except g++,VAL is a standard in c99, not in c++
    const int bounds2 = 99;
    int array2[bounds2];

    // test<bounds>ia;//the value of 'bounds' is not usable in a constant expression
    test<bounds2> ia2;

    test<square(5)> a;
    int dummy = 10;
    // test<square(dummy)>b;dummy is not constant expression,so square isn't either,just a normal function call

    int array3[foo_squared(half_double(make_cy(10)))];

    std::cout << sum(3, 4) << std::endl;                                        // is constexpr
    std::cout << sum(std::string("hello"), std::string(" world")) << std::endl; // isn't constexpr,is treated as normal funtion
}