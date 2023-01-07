#include <iostream>
#include <thread>
#include <assert.h>
void do_something(int i)
{
}
struct func
{
    int &i;
    func(int &i_) : i(i_) {}
    void operator()()
    {
        for (unsigned j = 0; j < 1000000; ++j)
        {
            i += 1;
            do_something(i);
        }
    }
};
void oops()
{
    int some_local_state = 0;
    func my_func(some_local_state); // my_func will be moved to my_thread
    std::thread my_thread(my_func);
    my_thread.detach(); // don't wait for thread to finish,terminate won't be called when the thread object is destroyed;
    
// 程序退出后线程的命运是不确定的行为。但是现代操作系统将在关闭进程时清理该进程创建的所有线程。
    assert(!my_thread.joinable());//can't be joined after detach
    //如果调用时主进程已经结束，则会报错,因为线程已经无效
    //terminate called after throwing an instance of 'std::system_error'
    //   what():  No such process
}

void do_something_in_current_thread() {}
void f()
{
    int some_local_state = 0;
    func my_func(some_local_state);
    std::thread t(my_func);
    try
    {
        do_something_in_current_thread();
    }
    catch (...)
    {
        t.join(); // 保证线程终止
        throw;    // 抛出
    }
    t.join();
}

class thread_guard
{
    std::thread t;

public:
    explicit thread_guard(std::thread &t_) : t(std::move(t_))
    {
    }
    ~thread_guard()
    {
        if (t.joinable())
        {
            t.join();
        }
    }
    thread_guard(const thread_guard &) = delete;
    thread_guard &operator=(const thread_guard &) = delete;
};

void g()
{
    int some_local_state = 0;
    func my_func(some_local_state);
    std::thread t(my_func);
    thread_guard g(t);
    do_something_in_current_thread();
}
int main()
{
    oops(); // It is possible to refer to some_local_state even after the function exits
    f();
    g();
    getchar();
}