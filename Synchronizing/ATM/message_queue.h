#pragma once
#include <mutex>
#include <condition_variable>
#include <queue>
#include <queue>
#include <memory>
namespace messaging
{
    struct message_base
    {
        virtual ~message_base() {}
    };
    template <typename Msg>
    struct warpped_message : message_base
    {
        Msg contents;
        explicit warpped_message(const Msg &contents_) : contents(contents_)
        {
        }
    };
    class queue
    {
        std::mutex m;
        std::condition_variable cv;
        std::queue<std::shared_ptr<message_base>> q;

    public:
        template <typename T>
        void push(const T &msg)
        {
            std::lock_guard<std::mutex> lk(m);
            q.push(std::make_shared<warpped_message<T>>(msg));
            cv.notify_all();
        }
        std::shared_ptr<message_base> wait_and_pop()
        {
            std::unique_lock<std::mutex> lk(m);
            cv.wait(lk, [&]
                    { return !q.empty(); });
            auto res = q.front();
            q.pop();
            return res;
        }
    };

} // namespace messaging
