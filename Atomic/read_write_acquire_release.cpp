#include <atomic>
#include <thread>
#include <cassert>
std::atomic<bool> x, y;
std::atomic<int> z;
void write_x()
{
    x.store(true, std::memory_order_release);//1
}
void write_y()
{
    y.store(true, std::memory_order_release);//2
}
void read_x_then_y()
{
    while (!x.load(std::memory_order_acquire))//3
        ;
    if (y.load(std::memory_order_acquire))//4
        z++;
}
void read_y_then_x()
{
    while (!y.load(std::memory_order_acquire))//5
        ;
    if (x.load(std::memory_order_acquire))//6
        z++;
}

//(1,3),(2,5) are pairs ,are synchrony
//however,(1,6),(2,4) are not forced synchronize
int main()
{
   
        x = false;
        y = false;
        z = 0;
        std::thread a(write_x);
        std::thread b(write_y);
        std::thread c(read_x_then_y);
        std::thread d(read_y_then_x);
        a.join();
        b.join();
        c.join();
        d.join();
        assert(z != 0);//can fire(激发)
    
}