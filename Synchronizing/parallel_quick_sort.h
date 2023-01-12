#include <bits/stdc++.h>
template <typename T>
std::list<T> parallel_quick_sort(std::list<T> input)
{
    if (input.size() < 2)
    {
        return input;
    }
    std::list<T> result;
    result.splice(result.begin(), input, input.begin());
    const T &pivot = *result.begin();

    auto divide_point = std::partition(input.begin(), input.end(), [&pivot](const T &t)
                                       { return t < pivot; });
    std::list<T> high_part;
    high_part.splice(high_part.begin(), input, divide_point, input.end());
    auto new_higher = std::async(&parallel_quick_sort<T>, std::move(high_part));
    auto new_lower = parallel_quick_sort(std::move(input));
    result.splice(result.begin(), new_lower);
    result.splice(result.end(), new_higher.get());
    return result;
}