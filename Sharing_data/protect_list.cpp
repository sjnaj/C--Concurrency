#include <list>
#include <mutex>
#include <algorithm>
#include <thread>
#include <iostream>
std::list<int> some_list;
std::mutex some_mutex;

void add_to_list(int new_value)
{
    std::lock_guard<std::mutex> guard(some_mutex); // RAII idiom for a mutex
    some_list.push_back(new_value);
}
bool list_contains(int value_to_find)
{
    std::lock_guard<std::mutex> guard(some_mutex);
    return std::find(some_list.begin(), some_list.end(), value_to_find) != some_list.end();
}
int main()
{
    std::thread t1 = std::thread(add_to_list, 1);
    std::thread t2 = std::thread(add_to_list, 2);
    std::thread t3 = std::thread(add_to_list, 3);
    std::thread t4 = std::thread(list_contains, 1);
    std::thread t5 = std::thread(add_to_list, 4);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    std::for_each(some_list.begin(),some_list.end(),[](int i){std::cout<<i;});
    std::cout << std::endl;
}
