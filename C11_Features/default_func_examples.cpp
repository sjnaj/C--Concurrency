#include <iostream>
// An aggregate is an array or a class ([class]) with
// no user-declared or inherited constructors ([class.ctor]),
// no private or protected direct non-static data members ([class.access]),
// no virtual functions ([class.virtual]), and
// no virtual, private, or protected base classes ([class.mi]).

struct aggregate
{
    aggregate() = default;
    aggregate(const aggregate &) = default;
    int a;
    double b;
};
// Plain Old Data
// int, char, wchar_t, bool, float, double are PODs, as are long/short and signed/unsigned versions of them.
// pointers (including pointer-to-function and pointer-to-member) are PODs,
// enums are PODs
// a const or volatile POD is a POD.
// a class, struct or union of PODs is a POD provided that all non-static data members are public,
// and it has no base class and no constructors, destructors, or virtual methods.
// Static members don't stop something being a POD under this rule.
// This rule has changed in C++11 and certain private members are allowed
struct X
{
    int a;
};

struct Y
{
    Y() : b() {}//always 0:For pod-types(int), the default initialization is zero-initialization.
    int b;
};
// Y::Y():b(42){};always 42
// Y::Y(){};indetermined unless at static duration

X x;//global variable at static area
int main()
{
    aggregate x = {42, 3.14};

    X x1;
    std::cout << x1.a << std::endl;//undetermined unless at static duration
    X x2 = X();//explicitly invoking the defaule constructor
    std::cout << x2.a << std::endl;//0

    Y y1;
    std::cout << y1.b << std::endl;//0
    Y y2;
    std::cout<<y2.b<<std::endl;//0


//c++17
    static_assert(std::is_trivial_v<X>, "X should be trivial");
    static_assert(std::is_standard_layout_v<X>, "X should be standard layout");
    static_assert(std::is_pod<X>::value, "X should be POD");

    static_assert(!std::is_trivial_v<Y>, "Y should not be trivial");
    static_assert(!std::is_pod<Y>::value, "Y should not be POD");
    static_assert(std::is_standard_layout_v<Y>, "Y should be standard layout");
}
// standard_layout_class
// 当类或结构不包含某些 C++ 语言功能（例如无法在 C 语言中找到的虚拟函数），并且所有成员都具有相同的访问控制时，该类或结构为标准布局类型。
//可以在内存中对其进行复制，并且布局已经过充分定义，可以由 C 程序使用。 标准布局类型可以具有用户定义的特殊成员函数。 此外，标准布局类型还具有以下特征：

// 没有虚拟函数或虚拟基类

// 所有非静态数据成员都具有相同的访问控制

// 类类型的所有非静态成员均为标准布局

// 所有基类都为标准布局

// 没有与第一个非静态数据成员类型相同的基类。

// 满足以下条件之一：

// 最底层派生类中没有非静态数据成员，并且具有非静态数据成员的基类不超过一个，或者

// 没有含非静态数据成员的基类

// A trivial class is a class that

// is trivially copyable, and
// has one or more eligible default constructors such that each is trivial.


// 在各种情况下，“普通”意味着构造函数/运算符/析构函数并非用户提供，并且属于存在以下情况的类

// 没有虚拟函数或虚拟基类，

// 没有具有相应非普通构造函数/运算符/析构函数的基类

// 没有具有相应非普通构造函数/运算符/析构函数的类类型的数据成员