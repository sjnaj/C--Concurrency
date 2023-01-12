#include "sequential_quick_sort.h"
#include "parallel_quick_sort.h"
std::list<int> li(100000);
int main()
{
    srand(time(0));
    std::generate(li.begin(), li.end(), []
                  { return rand() % 1000; });

    auto sequential_sorted_list = sequential_quick_sort(li);

    auto parallel_sorted_list = parallel_quick_sort(li);//very slow
    std::cout<<"complete";
    // for (auto &i : sorted_list)
    // {
    //     std::cout << i << "";
    // }
}