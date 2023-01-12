#include <chrono>
#include <iostream>
#include <future>
#include <thread>
int some_task()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(60));//40 get the opposite result
    return 1;
}
void do_some_thing_with(int i)
{
    std::cout << i << std::endl;
}

int main()
{
    std::chrono::milliseconds ms(54802);
    std::chrono::seconds s = std::chrono::duration_cast<std::chrono::seconds>(ms);
    std::cout << s.count() << std::endl; // 54,truncated

    std::future<int> f = std::async(some_task);
    if (f.wait_for(std::chrono::milliseconds(50)) == std::future_status::ready)
    {
        do_some_thing_with(f.get());
    }

    auto start=std::chrono::high_resolution_clock::now();
    some_task();
    auto stop=std::chrono::high_resolution_clock::now();
     std::chrono::duration<double, std::ratio<1, 30>> hz30(3.5);
    std::cout<<"took "<<std::chrono::duration<double,std::milli>(stop-start).count()<<" milliseconds"<<std::endl;
}