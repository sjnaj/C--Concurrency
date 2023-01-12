#include <mutex>
#include <iostream>
#include <thread>
class some_big_object
{
};
class X
{
private:
    some_big_object some_detail;
    std::mutex m;

public:
    X(const some_big_object &sd) : some_detail(sd) {}
    friend void swap(X &lhs, X &rhs)
    {
        if (&lhs == &rhs)
            return;
        std::unique_lock<std::mutex> lock_a(lhs.m, std::defer_lock);
        std::unique_lock<std::mutex> lock_b(rhs.m, std::defer_lock);
        std::lock(lock_a, lock_b);
        std::swap(lhs.some_detail, rhs.some_detail);
    };
};
std::mutex some_mutex;
void prepare_data() {}
std::unique_lock<std::mutex> get_lock()
{
    extern std::mutex some_mutex;
    std::unique_lock<std::mutex> lk(some_mutex);
    // std::lock_guard<std::mutex>lk1(some_mutex);//isn't moveable
    prepare_data();
    return lk; // compiler takes care of calling the move constructor
}
void do_something(){}
void process_data()
{
    std::unique_lock<std::mutex>lk(get_lock());
    do_something();
}
/*std::mutex the_mutex;
void get_and_process_data()
{
    std::unique_lock<std::mutex> my_lock(the_mutex);
    some_class data_to_process = get_next_data_chunk();
    my_lock.unlock();
    result_type result = process(data_to_process);
    my_lock.lock();
    write_result(data_to_process, result);
    //however ,not only is there likely to be more contention for the lock,but also the protential 
    //for reducing the time that the lock is held is less.More of the operation steps will require a lock on the same mutex.
}*/
int main()
{
}