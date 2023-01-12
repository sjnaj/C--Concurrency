#pragma once
#include "sender.h"
#include "receiver.h"
#include "messages.h"
#include "dispatcher.h"
#include "template_dispatcher.h"
#include <iostream>
class interface_machine
{
private:
    messaging::receiver incoming;
    // protect io
    std::mutex iom;

public:
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
                // The Standard mandates that all temporary objects get destroyed as the last step of evaluating of the full-expression that contains the point where the temporaries were created
                incoming.wait()
                    .handle<issue_money>(
                        [&](const issue_money &msg)
                        {
                            {
                                std::lock_guard<std::mutex> lk(iom); // lock to protect cout
                                std::cout << "Issuing " << msg.amount << std::endl;
                            }
                        })
                    .handle<display_insufficient_funds>(
                        [&](const display_insufficient_funds &msg)
                        {
                            {
                                std::lock_guard<std::mutex> lk(iom);
                                std::cout << "Insufficient funds" << std::endl;
                            }
                        })
                    .handle<display_enter_pin>(
                        [&](const display_enter_pin &msg)
                        {
                            {
                                std::lock_guard<std::mutex> lk(iom);
                                std::cout << "Please enter your PIN (0-9)" << std::endl;
                            }
                        })
                    .handle<display_enter_card>(
                        [&](const display_enter_card &msg)
                        {
                            {
                                std::lock_guard<std::mutex> lk(iom);
                                std::cout << "Please enter your card (I)" << std::endl;
                            }
                        })
                    .handle<display_balance>(
                        [&](const display_balance &msg)
                        {
                            {
                                std::lock_guard<std::mutex> lk(iom);
                                std::cout << "The balance of your account is " << msg.amount << std::endl;
                            }
                        })
                    .handle<display_withdrawal_options>(
                        [&](const display_withdrawal_options &msg)
                        {
                            {
                                std::lock_guard<std::mutex> lk(iom);
                                std::cout << "Withdraw 50? (w)" << std::endl;
                                std::cout << "Display Balance? (b)" << std::endl;
                                std::cout << "Cancel? (c)" << std::endl;
                            }
                        })
                    .handle<display_withdrawal_cancelled>(
                        [&](const display_withdrawal_cancelled &msg)
                        {
                            {
                                std::lock_guard<std::mutex> lk(iom);
                                std::cout << "Withdrawal cancelled" << std::endl;
                            }
                        })
                    .handle<display_pin_incorrect_message>(
                        [&](const display_pin_incorrect_message &msg)
                        {
                            {
                                std::lock_guard<std::mutex> lk(iom);
                                std::cout << "PIN incorrect" << std::endl;
                            }
                        })
                    .handle<eject_card>(
                        [&](const eject_card &msg)
                        {
                            {
                                std::lock_guard<std::mutex> lk(iom);
                                std::cout << "Ejecting card" << std::endl;
                            }
                        });
            }
        }
        catch (const messaging::close_queue &)
        {
            std::mutex m;
            {
                std::lock_guard<std::mutex> lk(m);
                std::cout << "user interface done!" << std::endl;
            }
        }
    }
    messaging::sender get_sender()
    {
        return incoming;
    }
};
