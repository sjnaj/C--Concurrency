#include <iostream>
#include <memory>
class no_copies
{
public:
    no_copies() {}
    no_copies(const no_copies &);
    no_copies &operator=(const no_copies &);
};
class no_copies1
{

public:
    no_copies1() {}
    no_copies1(const no_copies1 &) = delete; // 形参名省略
    no_copies1 &operator=(const no_copies1 &) = delete;
};

class my_class
{
};
class move_only
{
private:
    std::unique_ptr<my_class> data;

public:
    move_only()=default;
    move_only(const move_only &) = delete;
    move_only(move_only &&other) : data(std::move(other.data)){};
    move_only &operator=(const move_only &) = delete;
    move_only &operator=(move_only &&other)
    {
        data = std::move(other.data);
        return *this;
    }
};

int main()
{
    no_copies a;
    // no_copies b(a);no implementation,link error
    no_copies1 c;
    // no_copies1 d(c);//compiler error;easy to get;

    move_only m1;
   // move_only m2(m1);copy constructor is declared deleted
    move_only m3(std::move(m1));
}