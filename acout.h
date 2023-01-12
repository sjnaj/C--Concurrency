#include <mutex>
#include <iostream>

// Asynchronous output
struct acout
{
    static std::mutex mtx_cout;

    std::unique_lock<std::mutex> lk;
    acout()
        : lk(std::unique_lock<std::mutex>(mtx_cout))
    {
    }

    template <typename T>
    acout &operator<<(const T &_t)
    {
        std::cout << _t;
        return *this;
    }

    acout &operator<<(std::ostream &(*fp)(std::ostream &))
    {
        std::cout << fp;
        return *this;
    }
};
std::mutex acout::mtx_cout;//静态变量需要在类外初始化
