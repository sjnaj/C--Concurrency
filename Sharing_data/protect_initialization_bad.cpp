#include <mutex>
#include <memory>
class some_resource
{
public:
    void do_something();
};

std::shared_ptr<some_resource> resource_ptr;
std::mutex resource_mutex;
void foo()
{
    std::unique_lock<std::mutex> lk(resource_mutex);// all threads are serialized here
    if (!resource_ptr)
    {
        resource_ptr.reset(new some_resource);
    }
    lk.unlock();
    resource_ptr->do_something();
}

//read isn't synchronized with the write,data race will occur
void undefined_behaviour_with_double_checked_locking()
{
    if (!resource_ptr)
    {
        std::lock_guard<std::mutex> lk(resource_mutex);//will destory at end of action scope ,no need to use unique_lock
        if (!resource_ptr)
        {
            resource_ptr.reset(new some_resource);
        }
    }
    resource_ptr->do_something();
}
