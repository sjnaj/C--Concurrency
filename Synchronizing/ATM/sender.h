#pragma once
#include "message_queue.h"
namespace messaging
{
    class sender
    {
    private:
        queue *q;

    public:
        sender() : q(nullptr){}
        explicit sender(queue *q_) : q(q_) {}
        template <typename Message>
        void send(const Message &msg)
        {
            if (q)
            {
                q->push(msg);
            }
        }
    };
}
