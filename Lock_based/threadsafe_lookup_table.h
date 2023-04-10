#include <list>
#include <vector>
#include <memory>
#include <shared_mutex>
#include <map>
#include <algorithm>
template <typename Key, typename Value, typename Hash = std::hash<Key>>
class threadsafe_lookup_table
{
private:
    class bucket_type
    {
        friend std::map<Key, Value> threadsafe_lookup_table<Key, Value, Hash>::get_map() const;

    private:
        typedef std::pair<Key, Value> bucket_value;
        typedef std::list<bucket_value> bucket_data;
        typedef typename bucket_data::const_iterator bucket_iterator;

        bucket_data data;
        mutable std::shared_mutex mutex;
        bucket_iterator find_entry_for(const Key &key) const
        {
            return std::find_if(data.begin(), data.end(), [&](const bucket_value &item)
                                { return item.first == key; }); // return a const_iterator,deduced by data.begin()
        }

    public:
        Value value_for(const Key &key, const Value &default_value) const
        {
            std::shared_lock<std::shared_mutex> lock(mutex);
            bucket_iterator found_entry = find_entry_for(key);
            return (found_entry == data.end()) ? default_value : found_entry->second;
        }
        void add_or_update_mapping(const Key &key, const Value &value)
        {
            std::unique_lock<std::shared_mutex> lock(mutex);
            bucket_iterator found_entry = find_entry_for(key);
            if (found_entry == data.end())
            {
                data.push_back(bucket_value(key, value));
            }
            else
            {
                // convert const_iterator to iterator
                
                auto i = data.begin();
                std::advance(i, std::distance<bucket_iterator>(i, found_entry));

                i->second = value;
            }
        }
        void remove_mapping(const Key &key)
        {
            std::unique_lock<std::shared_mutex> lock(mutex);
            bucket_iterator found_entry = find_entry_for(key);
            if (found_entry != data.end())
            {
                data.erase(found_entry);
            }
        }
    };
    std::vector<std::unique_ptr<bucket_type>> buckets;
    Hash hasher;
    bucket_type &get_bucket(const Key &key) const
    {
        const std::size_t bucket_index = hasher(key) % buckets.size();
        return *buckets[bucket_index];
    }

public:
    threadsafe_lookup_table(unsigned num_buckets = 19, const Hash &hasher_ = Hash()) : buckets(num_buckets), hasher(hasher_)
    {
        for (unsigned i = 0; i < num_buckets; ++i)
        {
            buckets[i].reset(new bucket_type);
        }
    }
    threadsafe_lookup_table(const threadsafe_lookup_table &) = delete;
    threadsafe_lookup_table &operator=(const threadsafe_lookup_table &) = delete;
    Value value_for(const Key &key, const Value &default_value = Value()) const
    {
        return get_bucket(key).value_for(key, default_value);
    }
    void add_or_update_mapping(const Key &key, const Value &value)
    {
        get_bucket(key).add_or_update_mapping(key, value);
    }
    void remove_mapping(const Key &key)
    {
        get_bucket(key).remove_mapping(key);
    }
    std::map<Key, Value> get_map() const;
};
template <typename Key, typename Value, typename Hash>
std::map<Key, Value> threadsafe_lookup_table<Key, Value, Hash>::get_map() const
{
    std::vector<std::unique_lock<std::shared_mutex>> locks;
    for (unsigned i = 0; i < buckets.size(); ++i)
    {
        locks.push_back(std::unique_lock<std::shared_mutex>(buckets[i]->mutex));
    }
    std::map<Key, Value> res;
    for (unsigned i = 0; i < buckets.size(); ++i)
    {
        for (typename bucket_type::bucket_iterator it = buckets[i]->data.begin(); it != buckets[i]->data.end(); ++it)
        {
            res.insert(*it);
        }
    }
    return res;
}