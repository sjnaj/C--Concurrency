#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <string>

struct some_big_obj
{
    int value;
    some_big_obj(int val) : value(val) {}
};
class X
{
private:
    some_big_obj some_detail;
    std::mutex mutex;

public:
    X(const some_big_obj &sd) : some_detail(sd) {}
    friend void swap(X &lhs, X &rhs)
    {
        if (&lhs == &rhs)
            return;

        //A deadlock will occur if two threads swap(A,B) and swap(B,A) seperately;
        // std::lock_guard<std::mutex> lock_a(lhs.mutex);
        // std::lock_guard<std::mutex> lock_b(rhs.mutex);
        std::lock(lhs.mutex, rhs.mutex);
        std::lock_guard<std::mutex> lock_a(lhs.mutex,std::adopt_lock);//std::adopt_lock:Tag used to make a scoped lock take ownership of a locked mutex.
        std::lock_guard<std::mutex> lock_b(rhs.mutex,std::adopt_lock);
        std::swap(lhs.some_detail, rhs.some_detail);
        std::cout << lhs.get_data().value << rhs.get_data().value << std::endl;
    }
    some_big_obj get_data() const
    {
        return some_detail;
    }
};
X x1{some_big_obj(1)};
X x2{some_big_obj(2)};
void foo()
{

    while (true)
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(100ms);
        swap(x1, x2);
    }
}
void foo2()
{
    while (true)
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(100ms);
        swap(x2, x1);
    }
}
int main()
{
    std::thread t1{foo};
    std::thread t2{foo};
    t1.join();
    t2.join();
}