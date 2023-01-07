#include <iostream>
class X
{
private:
    /* data */
    int *data;

public:
    X() : data(new int[1000000]){};
    ~X()
    {
        delete[] data;
    };
    X(const X &other) : data(new int[1000000])
    {
        std::copy(other.data, other.data + 1000000, data);
    }
    X(X &&other) : data(other.data)
    {
        other.data = nullptr;
    }
};
void do_stuff(X &&x_)
{
    X a(x_);            // copy,within the function rvalue is treated as an lvalue;
    X b(std::move(x_)); // move
}

template <typename T>
void foo(T &&t) {}

int main()
{

    X x1;
    X x2 = std::move(x1);
    X x3 = static_cast<X &&>(x2); // equals to previout line

    do_stuff(X());
    X x;
    // do_stuff(x);//error,lvalue can't bind to rvalue reference

    foo(42);
    foo(std::string()); // foo(std::string)(std::string);T:std::string;&& is treated as rvalue reference

    int i = 42;
    foo(i); // foo<int&>(i);T:int&,i:int&; &&is treated as reference to a reference
}
