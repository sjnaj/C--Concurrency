#pragma once
#include "message_queue.h"
namespace messaging
{
    template <typename PreviousDispatcher, typename Msg, typename Func>
    class template_dispatcher
    {
    private:
        queue *q;
        PreviousDispatcher *prev;
        Func f;
        bool chained;

        // no copying
        template_dispatcher(const template_dispatcher &) = delete;
        template_dispatcher &operator=(const template_dispatcher &) = delete;
        template <typename Dispatcher, typename OtherMsg, typename OtherFunc>
        friend class template_dispatcher; // template_dispatcher instantiations are friends of each other
    public:
        void wait_and_diapatch()
        {
            while (true)
            {
                auto msg = q->wait_and_pop();
                if (dispatch(msg))
                    break;
            }
        }
        bool dispatch(const std::shared_ptr<message_base> &msg)
        {
            if (warpped_message<Msg> *warpper = dynamic_cast<warpped_message<Msg> *>(msg.get()))
            {
                f(warpper->contents);
                return true;
            }
            else
            {
                return prev->dispatch(msg); // Chain of Responsibility Pattern
                //                check the msg type down to up
            }
        }

    public:
        // moveable
        template_dispatcher(template_dispatcher &&other)
            : q(other.q), prev(other.prev), f(std::move(other.f)), chained(other.chained)
        {
            other.chained = true;
        }
        template_dispatcher(queue *q_, PreviousDispatcher *prev_, Func &&f_)
            : q(q_), prev(prev_), f(std::forward<Func>(f_)), chained(false)
        {
            prev_->chained = true;
        }
        // additional handles can be chained
        template <typename OtherMsg, typename OtherFunc>
        template_dispatcher<template_dispatcher, OtherMsg, OtherFunc> handle(OtherFunc &&of)
        {
            return template_dispatcher<template_dispatcher, OtherMsg, OtherFunc>(q, this, std::forward<OtherFunc>(of));
        }

        ~template_dispatcher() noexcept(false)
        {
            if (!chained)
            {
                wait_and_diapatch();
            }
        }
    };

} // namespace messaging
