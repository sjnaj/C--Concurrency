#include <list>
#include <mutex>
#include <algorithm>
#include <thread>
#include <iostream>
#include <iterator>
#include <type_traits>
#include <memory>
#include <stdlib.h>

template <typename T>
class threadsafe_list
{

private:
    std::list<std::pair<T, std::shared_ptr<std::mutex>>> data;
    /// mutex is neither copyable nor moveable ,thus this pair is same,so should use pointer of mutex

    std::mutex lock_head; // use for lock head_node that can't be assigned a mutex

public:
    auto insert(typename std::list<std::pair<T, std::shared_ptr<std::mutex>>>::iterator pos, const T &value) // The compiler needs to be told that this expression is a type, not a static variable or a function or something else inside the class.
    {
        auto prev = --pos;
        auto curr = ++pos;
        if (data.empty())
        {
            std::lock_guard<std::mutex> lock(lock_head);
        }
        else
        {
            if (curr == data.end())                        // tail insert would prodece deadlock such as list only has one elem but insert get first elem and inverse get head elem , because it break the lock order
                throw std::runtime_error("unsafty usage"); // may use a  function with mutex to realise tail insert
            if (prev == data.end())                        // head insert
            {
                std::lock_guard<std::mutex> lock(lock_head);
            }
            else
            {
                std::lock_guard<std::mutex> lock_a(*(prev->second.get()));
            }

            std::lock_guard<std::mutex> lock_a(*(curr->second.get()));
        }
        return data.insert(curr, std::pair<T, std::shared_ptr<std::mutex>>(value, std::make_shared<std::mutex>()));
    }

    auto begin()
    {
        return data.begin();
    }
    auto end()
    {
        return data.end();
    }
    auto empty()
    {
        return data.empty();
    }

    auto remove(typename std::list<std::pair<T, std::shared_ptr<std::mutex>>>::iterator pos)
    {
        if (!(data.empty() || pos == data.end()))
        {
            auto prev = --pos;
            auto curr = ++pos;
            auto next = ++pos;
            if (prev == data.end())
            {
                std::lock_guard<std::mutex> lock(lock_head);
            }
            else
            {
                std::lock_guard<std::mutex> lock_c(*(prev->second.get()));
            }
            std::lock_guard<std::mutex> lock_c(*(curr->second.get()));
            curr->second.get();
            if (prev != next)
            {
                if (next == data.end())
                {
                    // std::lock_guard<std::mutex> lock(lock_head);
                    throw std::runtime_error("unsafty usage");
                }
                else
                {
                    std::lock_guard<std::mutex> lock_c(*(next->second.get()));
                }
            }
        }
        return data.erase(pos);
    }

    // T next(typename std::list<std::pair<T, std::shared_ptr<std::mutex>>>::iterator pos, typename std::iterator_traits<typename std::list<std::pair<T, std::mutex>>::const_iterator>::difference_type __n = 1)
    // auto next(typename std::list<std::pair<T, std::shared_ptr<std::mutex>>>::iterator pos)
    // {
    //     auto curr = pos;
    //     auto next = ++pos;
    //     if (data.empty())
    //     {
    //         std::lock_guard<std::mutex> lock(lock_head);
    //     }
    //     else
    //     {
    //         if (curr == data.end())
    //         {
    //             std::lock_guard<std::mutex> lock(lock_head);
    //         }
    //         else
    //         {
    //             std::lock_guard<std::mutex> lock_a(*(curr->second.get()));
    //         }
    //         if (next == data.end()) // tail insert
    //         {
    //             std::lock_guard<std::mutex> lock(lock_head);
    //         }
    //         else
    //         {
    //             std::lock_guard<std::mutex> lock_a(*(next->second.get()));
    //         }
    //     }
    //     return std::next(curr, 1);
    // }
    void traverse()
    {
        if (data.empty())
            return;
        auto curr = data.begin();
        (curr->second.get())->lock();
        while (curr != data.end())
        {
            auto curr_tmp = curr++;
            if (curr != data.end()) // curr is next
                (curr->second.get())->lock();
            std::cout << curr_tmp->first << " ";
            (curr_tmp->second.get())->unlock();
        }
    }
};
threadsafe_list<int> li;
void traverse()
{
    while (true)
    {
        li.traverse();
    }
}
void insert()
{
    while (true)
    {
        li.insert(li.begin(), rand() % 100);
    }
}
void remove_() // Inexplicably overload with remove in stdio.h
{
    while (true)
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(10ms);
        // Reduce delete speed
        if (!li.empty())
            li.remove(li.begin());
    }
}
int main()
{
    // std::list<std::pair<int, std::mutex>> data(1);
    // std::pair<int,std::mutex> a;
    // a.first=2;
    // data.insert(data.begin(), std::move(a));//can not copy or move
    std::thread t1{insert};
    std::thread t2{traverse};
    std::thread t3{remove_};
    t1.join();
    t2.join();
    t3.join();
    // input "-exec set scheduler-locking on" at debug console can pause other thread while running a thread
    //// deadlock still exists 😅
}