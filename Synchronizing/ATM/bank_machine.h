#pragma once
#include "sender.h"
#include "receiver.h"
#include "messages.h"
#include "dispatcher.h"
#include "template_dispatcher.h"
#include <iostream>
class bank_machine
{
private:
    messaging::receiver incoming;
    std::string pin;
    unsigned balance;

public:
    bank_machine() = default;
    bank_machine(unsigned balance_, std::string pin_) : balance(balance_), pin(pin_) {}
    void done()
    {
        get_sender().send(messaging::close_queue());
    }
    void run()
    {
        try
        {
            while (true)
            {
                incoming.wait()
                    .handle<verify_pin>(
                        [&](const verify_pin &msg)
                        {
                            if (msg.pin == pin)
                            {
                                msg.atm_queue.send(pin_verified());
                            }
                            else
                            {
                                msg.atm_queue.send(pin_incorrect());
                            }
                        })
                    .handle<withdraw>(
                        [&](const withdraw &msg)
                        {
                            if (balance >= msg.amount)
                            {
                                msg.atm_queue.send(withdraw_ok());
                                balance -= msg.amount;
                            }
                            else
                            {
                                msg.atm_queue.send(withdraw_denied());
                            }
                        })
                    .handle<get_balance>(
                        [&](const get_balance &msg)
                        {
                            msg.atm_queue.send(::balance(balance)); //*tell the compiler to use the global identifier rather than the local identifier
                        })
                    .handle<withdrawal_processed>(
                        [&](const withdrawal_processed &msg) {
                        })
                    .handle<cancel_withdrawal>(
                        [&](const cancel_withdrawal &msg) {

                        });
            }
        }
        catch (const messaging::close_queue &)
        {
            std::mutex m;
            {
                std::lock_guard<std::mutex> lk(m);
                std::cout << "bank machine done!" << std::endl;
            }
        }
    }
    messaging::sender get_sender()
    {
        return incoming;
    }
};
