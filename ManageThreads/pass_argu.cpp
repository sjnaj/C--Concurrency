#include <iostream>
#include <thread>

void f(int i, const std::string &s)
{
}
void oops(int some_para)
{
    char buffer[1024];
    sprintf(buffer, "%i", some_para);
    //! std::thread t(f,3,buffer);
    // Threads begin execution immediately upon construction of the associated thread object
    // oops will exit before the buffer has been converted to a std::stringe

    std::thread t(f, 3, std::string(buffer)); 
    // copy(move) std::string while construct thread object

    //  Creates new std::thread object and associates it with a thread of execution. The new thread of execution
    //* starts executing /*INVOKE*/(std::move(f_copy), std::move(args_copy)...), where
    // /*INVOKE*/ performs the INVOKE operation specified in Callable, which can be performed by std::invoke (since C++17), and
    // f_copy is an object of type std::decay<Function>::type and constructed from std::forward<Function>(f), and
    // args_copy... are objects of types std::decay<Args>::type... and constructed from std::forward<Args>(args)....
   
    //*copy occers during the conversation to its dacay type
    t.detach();
}

struct widget_data
{
    int value;
};
void update_data_widget(widget_data &data)
{
    data.value += 1;
}
void oops_again()
{
    widget_data data = widget_data();
    // std::thread t(update_data_widget,  data);
    /*static_assert( __is_invocable<typename decay<_Callable>::type,
                  typename decay<_Args>::type...>::value,
      "std::thread arguments must be invocable after conversion to rvalues"
      );*/
    // lvalue will be deduced to lreference by Arg&&,and  ref is removed by decay,causing static_assert error
    // std::ref can warp lreference to pass the assert

    std::thread t(update_data_widget, std::ref(data));
    t.join();
    std::cout << data.value;
}

struct X
{
    void do_lengthy_work(int i) { std::cout << i << std::endl; }
};

int main()
{
    oops(1);
    oops_again();

    X my_x;
    std::thread t(&X::do_lengthy_work, &my_x, 1);
    t.join();

    getchar();
}