#include "parallel_accumulate_using_async.h"
#include "parallel_accumulate_using_task.h"
#include "../Timer.h"
int main()
{
    std::vector<int> vec(100000000, 1);
    {
        // 287ms
        Timer t1("async");
        std::cout << parallel_accumulate_using_async(vec.begin(), vec.end(), 0, vec.size() / std::thread::hardware_concurrency()) << std::endl;
    }
    {
        // 538ms
        Timer t2("task");
        std::cout << parallel_accumulate_using_task(vec.begin(), vec.end(), 0) << std::endl;
    }
    {
        // 1237ms
        Timer t3("sequence");
        std::cout << std::accumulate(vec.begin(), vec.end(), 0) << std::endl;
    }
}
