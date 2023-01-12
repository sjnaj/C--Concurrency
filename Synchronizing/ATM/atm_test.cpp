#include "atm_lib.h"
#include "thread"
int main()
{
    bank_machine bank(100,"1111");
    interface_machine interface_hardware;
    atm machine(bank.get_sender(), interface_hardware.get_sender());
    std::thread bank_thread(&bank_machine::run, &bank);
    std::thread if_thread(&interface_machine::run, &interface_hardware);
    std::thread atm_thread(&atm::run, &machine);

    messaging::sender atmqueue = machine.get_sender();
    bool quit_pressed = false;
    while (!quit_pressed)
    {
        char c = getchar();
        if (isdigit(c))
        {
            atmqueue.send(digit_pressed(c));
        }
        else
        {
            switch (c)
            {
            case 'b':
                atmqueue.send(balance_pressed());
                break;
            case 'w':
                atmqueue.send(withdraw_pressed(50));
                break;
            case 'c':
                atmqueue.send(cancel_pressed());
                break;
            case 'q':
                quit_pressed = true;
                break;
            case 'i':
                atmqueue.send(card_inserted("acc1234"));
                break;
            }
        }
    }
    bank.done();
    machine.done();
    interface_hardware.done();
    atm_thread.join();
    bank_thread.join();
    if_thread.join();
}