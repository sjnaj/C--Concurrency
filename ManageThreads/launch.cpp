#include <iostream>
#include <thread>
void do_some_work()
{
    std::cout << "Hello world\n";
}
void do_something() {}
void do_something_else() {}
class background_task
{
public:
    void operator()() const
    {
        do_something();
        do_something_else();
    }
};


int main()
{
    std::thread my_thread(do_some_work);

    background_task f;
    std::thread my_thread2(f);

    // std::thread my_thread3(background_task());// parentheses were disambiguated as a function declaration
    std::thread my_thread4{background_task()}; // recommanded
    std::thread my_thread5((background_task()));

    std::thread my_thread6([]{do_something();do_something_else();});//c++11



}