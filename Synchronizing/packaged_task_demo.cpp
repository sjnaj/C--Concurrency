#include <deque>
#include <mutex>
#include <future>
#include <thread>
#include <iostream>
#include <chrono>
std::mutex m;
std::deque<std::packaged_task<void()>> tasks;
bool gui_shutdown_message_receive() { return false; }
bool get_and_process_gui_message() { return true; }
void gui_thread()
{
    while (!gui_shutdown_message_receive())
    {
        get_and_process_gui_message();
        std::packaged_task<void()> task;
        {
            std::lock_guard<std::mutex> lk(m);
            if (tasks.empty())
                continue;
            task = std::move(tasks.front());
            tasks.pop_front();
        }

        task(); // the future associated with the task will then be made ready when the task completes
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));//delay
    }
}

std::thread gui_bg_thread(gui_thread);

template <typename Func>
std::future<void> post_task_for_gui_thread(Func f)
{
    std::packaged_task<void()> task(f);
    std::future<void> res = task.get_future();
    std::lock_guard<std::mutex> lk(m);
    tasks.push_back(std::move(task));
    return res;
}
int main()
{
    gui_bg_thread.detach();
    auto update1 = post_task_for_gui_thread([]
                                            { std::cout << "update1" << std::endl; });
    auto update2 = post_task_for_gui_thread([]
                                            { std::cout << "update2" << std::endl; });
    auto update3 = post_task_for_gui_thread([]
                                            { std::cout << "update3" << std::endl; });
    auto update4 = post_task_for_gui_thread([]
                                            { std::cout << "update4" << std::endl; });
    update1.wait();
    update1.wait();
    update3.wait();
    update4.wait();
}