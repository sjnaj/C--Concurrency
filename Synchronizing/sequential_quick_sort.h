#include <bits/stdc++.h>
template <typename T>
std::list<T> sequential_quick_sort(std::list<T> input)
{
    if (input.size() < 2)
    {
        return input;
    }
    std::list<T> result;
    // Transfers the element pointed to by it from other into *this. The element is inserted before the element pointed to by pos.
    result.splice(result.begin(), input, input.begin()); // move input.begin() to result
    const T &pivot = *result.begin();

    // Move elements for which a predicate is true to the beginning of a sequence.
    // partition() does not preserve the relative ordering of elements in each group, use stable_partition() if this is needed.
    auto divide_point = std::partition(input.begin(), input.end(), [&pivot](const T &t)
                                       { return t < pivot; });
    std::list<T> high_part;
    high_part.splice(high_part.begin(), input, divide_point, input.end());

    // implicit constructed by moving
    auto new_lower = sequential_quick_sort(std::move(input));
    auto new_higher = sequential_quick_sort(std::move(high_part));
    result.splice(result.end(), new_higher);
    result.splice(result.begin(), new_lower);
    return result;
}