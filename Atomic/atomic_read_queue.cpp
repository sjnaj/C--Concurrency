#include <atomic>
#include <thread>
#include <vector>
#include <iostream>
std::vector<int> queue_data;
std::atomic<int> count;

void populate_queue()
{
    const unsigned number_of_items = 20;
    queue_data.clear();
    for (unsigned i = 0; i < number_of_items; i++)
    {
        queue_data.push_back(i);
    }
    count.store(number_of_items, std::memory_order_release);
}
void consume_queue_items()
{
    while (true)
    {
        int item_index;
        if ((item_index = count.fetch_sub(1, std::memory_order_acquire)) <= 0)
        {
            break;
        }
        std::cout << queue_data[item_index - 1];
    }
}

int main()
{
    std::thread a(populate_queue);
    std::thread b(consume_queue_items);
    std::thread c(consume_queue_items);
    a.join();
    b.join();
    c.join();
    // release chain store->fetch_sub1->fetch_sub2
}