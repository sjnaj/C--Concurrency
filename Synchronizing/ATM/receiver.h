#pragma once
#include "sender.h"
#include "dispatcher.h"

namespace messaging
{
    class receiver
    {
    private:
        queue q;

    public:
        operator sender() // allow implicit convertion to a sender
        {
            return sender(&q);
        }

        dispatcher wait(){
            return dispatcher(&q);
        }
    };

} // namespace messaging
