#include <iostream>
#include <thread>
#include <mutex>
static std::mutex mtx_cout;

// Asynchronous output
struct acout
{
        std::unique_lock<std::mutex> lk;
        acout()
            :
              lk(std::unique_lock<std::mutex>(mtx_cout))
        {

        }

        template<typename T>
        acout& operator<<(const T& _t)
        {
            std::cout << _t;
            return *this;
        }

        acout& operator<<(std::ostream& (*fp)(std::ostream&))
        {
            std::cout << fp;
            return *this;
        }
};
std::thread::id master_thread;
void do_master_thread_work(){
    acout()<<"I'm master"<<std::endl;// 
}
void do_common_work(){
    acout()<<"Common work"<<std::endl<<"Thread id:"<<std::this_thread::get_id()<<std::endl;
}
void some_core_part_of_algorithm(){
    if(std::this_thread::get_id()==master_thread){
        do_master_thread_work();
    }
    do_common_work();
}
int main(){
    master_thread=std::this_thread::get_id();
    std::thread t1(some_core_part_of_algorithm);
    std::thread t2(some_core_part_of_algorithm);
    some_core_part_of_algorithm();
    acout()<<master_thread<<std::endl;
    t1.join();
    t2.join();
    acout()<<t1.get_id()<<std::endl;
}