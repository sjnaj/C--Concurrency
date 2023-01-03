#include <iostream>
#include <memory>
#include <typeinfo>
#include <cxxabi.h>

template <typename T>
std::string type_name()
{
    // typedef typename std::remove_reference<T>::type TR;
    using TR = typename std::remove_reference<T>::type; // 告诉编译器type是类型而不是变量
    int status;
    std::string tname = typeid(T).name();
    // convert to human friendly
    char *demangled_name = abi::__cxa_demangle(tname.c_str(), NULL, NULL, &status);
    if (status == 0)
    {
        tname = demangled_name;
        std::free(demangled_name);
    }
    if (std::is_const<TR>::value)
        tname += " const";
    if (std::is_volatile<TR>::value)
        tname += " volatile";
    if (std::is_lvalue_reference<T>::value)
        tname += "&";
    else if (std::is_rvalue_reference<T>::value)
        tname += "&&";
    return tname;
}

template <typename... Params>
struct dummy1
{
    std::tuple<Params...> data;
};
template <typename... Params>
struct dummy2
{
    std::tuple<std::string, Params...> data;
};
template <typename... Params>
struct dummy3
{
    std::tuple<Params *...> pointers;
    std::tuple<std::unique_ptr<Params>...> unique_pointers;
};
template <typename... ArgTypes>
void foo(ArgTypes... args)
{
    //* ((std::cout << args << std::endl), ...);//C++17
    using expander = int[];
    (void)expander{0,//c++11
                   ((void)(std::cout << args << " "), 0)...};
    //展开为列表初始化形式,逗号表达式使得括号内值为0,最前面的void防止警告，后面的void强转是忽略的意思,其上括号可以省略
    //Any expression can be explicitly converted to type “void.” The expression value is discarded.
    std::cout << std::endl;
    
}
template <typename... ArgTypes>
void bar(ArgTypes &&...args)
{
    foo(std::forward<ArgTypes>(args)...);
    using expander = int[];
    (void)expander{0,
                   (void(std::cout << type_name<decltype(std::forward<ArgTypes>(args))>() << " "), 0)...};
    std::cout << std::endl;
}

int main()
{
    dummy1<const int, char, double> d1;
    // 悬浮即可显示展开后的类型
    std::cout << type_name<decltype(d1.data)>() << std::endl;
    dummy2<int, char> d2;
    std::cout << type_name<decltype(d2.data)>() << std::endl;
    dummy3<int, int, int> d3;
    std::cout << type_name<decltype(d3.pointers)>() << std::endl;
    void(0);
    // std::cout << type_name(d3.unique_pointers) << std::endl;//看不了
    int i = 1;
    bar(i, 3.141, std::string("hello"));
}
