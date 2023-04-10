#include "parallel_for_each.h"
#include "../Timer.h"
int main()
{
    std::vector<int> vec(100000000);
    {
        Timer t("t1");
        parallel_for_each(
            vec.begin(), vec.end(), [](int i) {}, vec.size() / std::thread::hardware_concurrency());
    }
    {
        Timer t("t2");
        std::for_each(vec.begin(), vec.end(), [](int i) {});
    }
}