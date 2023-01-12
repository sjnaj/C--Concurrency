#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
std::condition_variable cv;
bool done;
std::mutex m;
bool wait_loop()
{
    const auto timeout = std::chrono::steady_clock::now() + std::chrono::milliseconds(500);
    std::unique_lock<std::mutex> lk(m);
    while (!done) // need to loop when don't pass the predicate ,in order to handle spurious wake
    {
        if (cv.wait_until(lk, timeout) == std::cv_status::timeout)
            // if use wait_for ,spurious wake will make it wait again,may be Longer than expected
            break;
    }
    return done;
}
