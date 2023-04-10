
#include <bits/stdc++.h>
// concise but slow if chunk_size not appropriate
//could call like this :std::cout << parallel_accumulate_using_async(vec.begin(), vec.end(), 0,vec.size()/std::thread::hardware_concurrency()) << std::endl;
template <typename Iterator, typename T>
T parallel_accumulate_using_async(Iterator first, Iterator last, T init,unsigned long const max_chunk_size=1000000)
{
    unsigned long const length = std::distance(first, last);
    if (length <= max_chunk_size)
    {
        return std::accumulate(first, last, init);
    }
    else
    {
        Iterator mid_point = first;
        std::advance(mid_point, length / 2);
        std::future<T> first_half_result = std::async(parallel_accumulate_using_async<Iterator, T>, first, mid_point, init,max_chunk_size);
        T second_half_result = parallel_accumulate_using_async(mid_point, last, T());
        return first_half_result.get() + second_half_result;
    }
}