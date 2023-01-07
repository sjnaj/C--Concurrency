#include <map>
#include <string>
#include <mutex>
#include <shared_mutex> //since c++17,or can use boost
class dns_entry
{
};
class dns_cache
{
    std::map<std::string, dns_entry> entries;
    mutable std::shared_mutex entry_mutex;

public:
    dns_entry find_entry(const std::string &domain) const//read threads share a shared lock
    {
        std::shared_lock<std::shared_mutex> lk(entry_mutex);
        auto it = entries.find(domain);
        return (it == entries.end()) ? dns_entry() : it->second;
    }
    void update_or_add_entry(const std::string &domain, const dns_entry &dns_details)//every update thread havs an exclusive lock
    {
        std::lock_guard<std::shared_mutex> lk(entry_mutex);
        entries[domain] = dns_details;
    }
    //The program behaves like a lock-free program without an update operation
    //if have update ,other threads will be waiting for it to be relinquished
    //be appropriate for protecting rarely updated data structures
};
