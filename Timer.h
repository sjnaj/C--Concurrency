#include <chrono>
#include <iostream>
struct Timer
{
    std::chrono::time_point<std::chrono::steady_clock> start, end;
    std::string name = "Timer";
    // the high_resolution_clock is not implemented consistently across different standard library implementations, and its use should be avoided.
    // It is often just an alias for std::chrono::steady_clock or std::chrono::system_clock, but which one it is depends on the library or configuration.
    // When it is a system_clock, it is not monotonic (e.g., the time can go backwards). For example, for gcc's libstdc++ it is system_clock,
    // for MSVC it is steady_clock, and for clang's libc++ it depends on configuration.

    // Generally one should just use std::chrono::steady_clock or std::chrono::system_clock directly instead of std::chrono::high_resolution_clock: use steady_clock for duration measurements, and system_clock for wall-clock time.

    Timer() : start(std::chrono::steady_clock::now()) {}

    Timer(std::string name_) : start(std::chrono::steady_clock::now()), name(name_)
    {
    }
    ~Timer()
    {
        // using namespace  std::chrono_literals;
        // std::this_thread::sleep_for(1000ms);
        end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << name << " took" << duration.count() << "ms" << std::endl;
    }
};