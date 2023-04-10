#include <bits/stdc++.h>
template <typename Iterator, typename MatchType>
Iterator parallel_find_impl(Iterator begin, Iterator end, MatchType match, std::atomic<bool> &done_flag, unsigned const long max_batch_size = 1000000)
{
    try
    {
        unsigned long const length = std::distance(begin, end);
        if (length < max_batch_size)
        {
            for (; (begin != end) && !(done_flag.load(std::memory_order_acquire)); ++begin)
            {
                if (*begin == match)
                {
                    done_flag.store(true, std::memory_order_release);
                    return begin;
                }
            }
            return end;
        }
        else
        {
            Iterator const mid_point = begin + (length / 2);
            std::future<Iterator> async_result = std::async(parallel_find_impl<Iterator, MatchType>, mid_point, end, match, std::ref(done_flag), max_batch_size);
            Iterator const direct_result = parallel_find_impl<Iterator, MatchType>(begin, mid_point, match, done_flag, max_batch_size);
            return (direct_result == mid_point) ? async_result.get() : direct_result;
        }
    }

    catch (...)
    {
        done_flag = true;
        throw;
    }
};
template <typename Iterator, typename MatchType>
Iterator parallel_find(Iterator first, Iterator last, MatchType match, unsigned long const max_batch_size = 1000000)
{

    std::atomic<bool> done_flag(false);
    return parallel_find_impl(first, last, match, done_flag, max_batch_size);
}
