#include <bits/stdc++.h>
template <typename Iterator, typename Func>
void parallel_for_each(Iterator first, Iterator last, Func f, unsigned long const max_chunk_size = 1000000)
{
    unsigned long const length = std::distance(first, last);
    if (!length)
        return;
    if (length < max_chunk_size)
    {
        std::for_each(first, last, f);
    }
    else
    {
        Iterator const mid_point = first + length / 2;
        std::future<void> first_half = std::async(&parallel_for_each<Iterator, Func>, first, mid_point, f, max_chunk_size);
        parallel_for_each(mid_point, last,f);
        first_half.get();
    }
}