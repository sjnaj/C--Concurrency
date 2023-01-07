#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
class box
{
private:
    bool drum=true;
    bool drumstick=true;
    mutable std::mutex mutex;

public:
    bool get_drum()
    {
        std::lock_guard<std::mutex> lock(mutex);
        return drum ? (drum = false, true) : false;
    }
    bool get_drumstick() 
    {
        std::lock_guard<std::mutex> lock(mutex);
        return drumstick? (drumstick = false, true) : false;
    }
    void put()
    {
        std::lock_guard<std::mutex> lock(mutex);
        drum = drumstick = true;
    }
};
box b;
void player()
{
    while (true)
    {
        // if (b.get_drum() & b.get_drumstick())//未定义顺序时会发生死锁
        if(b.get_drum()&&b.get_drumstick())//定义了获取顺序，不会死锁
        {
            std::cout << "player " << std::this_thread::get_id() << "is playing" << std::endl;
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(100ms);
            b.put();
        }
    }
}
int main()
{
    std::vector<std::thread> threads(2);
    std::thread t1(player);
    std::thread t2(player);
    t1.join();
    t2.join();
}
