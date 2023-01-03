#include <iostream>
#include  <numeric>
template<typename Iterator,typename T>
struct accumulata_block
{
    void operator()(Iterator frist,Iterator last,T& result){
        result=std::accumulate(frist,last,result);
    }
};
template<typename Iterator,typename T>
T parallel_accumulate(Iterator first,Iterator last,T init){
   const unsigned long  length=std::distance(first,lase);
    if(!length)return init;
   const unsigned long min_per_thread=25;
   const unsigned long max_threads=(length+min_per_thread-1)/min_per_thread;//

}