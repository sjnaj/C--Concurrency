#pragma once
#include "template_dispatcher.h"
namespace messaging
{
    class close_queue
    {
    };
    class dispatcher
    {
        queue *q;
        bool chained;
        dispatcher(const dispatcher &) = delete;
        dispatcher &operator=(const dispatcher &) = delete;
        template <typename Dispatcher, typename Msg, typename Func>
        friend class template_dispatcher;
        void wait_and_dispatch()
        {
            while (true)
            {
                auto msg = q->wait_and_pop();
                dispatch(msg);
            }
        }

        //chain check will end up there
        bool dispatch(const std::shared_ptr<message_base> &msg)
        {
            if (dynamic_cast<warpped_message<close_queue> *>(msg.get()))
            {
                throw close_queue();
            }
            return false; // unhandled
        }

    public:
        dispatcher(dispatcher &&other) : q(other.q), chained(other.chained) {}
        explicit dispatcher(queue *q_) : q(q_), chained(false)
        {
        }
        template <typename Msg, typename Func>
        template_dispatcher<dispatcher, Msg, Func> handle(Func &&f)
        {
            return template_dispatcher<dispatcher, Msg, Func>(q, this, std::forward<Func> (f));
        }
        //allow except:close queue
        ~dispatcher() noexcept(false)
        {
            if (!chained)
            {
                wait_and_dispatch();
            }
        }
    };
} // namespace messaging
