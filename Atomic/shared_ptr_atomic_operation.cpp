#include <memory>
class my_data
{
};
std::shared_ptr<my_data> p;
void process_global_data()
{
    std::shared_ptr<my_data> local = std::atomic_load(&p);
}
void update_global_data()
{
    std::shared_ptr<my_data> local(new my_data);
    std::atomic_store(&p, local);
}
//weak and shared ptr are nonatomic types that be support by atomic operations