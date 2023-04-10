#include "threadsafe_lookup_table.h"
#include <iostream>
#include <thread>
#include <random>

threadsafe_lookup_table<unsigned, unsigned> table;
std::default_random_engine e;
std::uniform_int_distribution<unsigned> u;
const unsigned count = 100000;
void thread_add()
{
    for (unsigned i = 0; i < count; i++)
    {
        table.add_or_update_mapping(u(e), u(e));
    }
}
void thread_erase()
{

    for (unsigned i = 0; i < count; i++)
    {
        table.remove_mapping(u(e));
    }
}
void thread_find()
{
    for (unsigned i = 0; i < count; i++)
    {
        table.value_for(u(e));
    }
}
int main()
{
    std::thread t1(thread_add);
    std::thread t2(thread_erase);
    std::thread t3(thread_find);
    std::thread t4(thread_find);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    for (auto i : table.get_map())
    {
        std::cout << i.first << ":" << i.second << " ";
    }
    
}