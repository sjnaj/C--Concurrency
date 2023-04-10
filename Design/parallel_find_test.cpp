#include "parallel_find.h"
#include "../Timer.h"
int main()
{
    std::vector<int> vec(100000000, 0);
    vec.emplace_back(1);
    {
        Timer t1("t1");
        std::cout << *parallel_find(vec.begin(), vec.end(), 1, vec.size() / std::thread::hardware_concurrency());
    }
    {
        Timer t2("t2");
        std::cout<<*std::find(vec.begin(),vec.end(),1);
    }
}