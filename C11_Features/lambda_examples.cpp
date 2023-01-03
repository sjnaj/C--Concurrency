#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

std::function<int(int)> make_offseter(int offset)
{
    return [=](int j)
    { return offset + j; };
}
struct X
{
    int some_data;
    void foo(std::vector<int> &vec)
    {
        std::for_each(vec.begin(), vec.end(), [this](int &i) // need this
                      { i += some_data; });
    }
};
int main()
{
    std::vector<int> data = {1, 2, 3, 4, 5};
    std::for_each(data.begin(), data.end(), [](int i)
                  { std::cout << i << std::endl; });
    std::function<int(int)> offset_42 = make_offseter(42);
    std::function<int(int)> offset_123 = make_offseter(123);
    std::cout << offset_42(12) << "," << offset_42(12) << std::endl;
    std::cout << offset_123(12) << "," << offset_123(12) << std::endl;

    int offset = 42;
    std::function<int(int)> offset_1 = [=](int j)
    { return offset + j; };
    std::function<int(int)> offset_a = [&](int j)
    { return offset + j; };
    offset = 123;
    std::function<int(int)> offset_2 = [=](int j)
    { return offset + j; };
    std::function<int(int)> offset_b = [&](int j)
    { return offset + j; };
    std::cout << offset_1(12) << "," << offset_2(12) << std::endl;

    std::cout << offset_a(12) << "," << offset_b(12) << std::endl;
    offset = 99;
    std::cout << offset_1(12) << "," << offset_2(12) << std::endl;

    std::cout << offset_a(12) << "," << offset_b(12) << std::endl;
    //=:Depends on the values of external variables at the time of definition
    //&:would change dynamically会动态改变

    // other combinations of variable list
    //[=,&j,&k]
    //[&,j,k]
    //[&i,j,&k]
}