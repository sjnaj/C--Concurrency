#include <mutex>
#include <memory>
class some_resource
{
public:
    void do_something();
};
std::shared_ptr<some_resource> resource_ptr;
std::once_flag resource_flag;
void init_resource()
{
    resource_ptr.reset(new some_resource);
}
void foo()
{
    std::call_once(resource_flag, init_resource);
    resource_ptr->do_something();
}
class my_class
{
};
// Static initialization is defined to happen on exactly one thread
//  can be used as an alternative to std::call_once for those cases where a single global instance is required;
my_class &get_my_class_instance()
{
    static my_class instance;
    return instance;
}