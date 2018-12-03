#ifndef FUNCTIONALBADGES_TIMING_H
#define FUNCTIONALBADGES_TIMING_H

#include <iostream>
#include <chrono>

template<typename TimeT = std::chrono::milliseconds>
struct Measure {
    template<typename F, typename ...Args>
    static typename TimeT::rep execution(F&& func, Args&&... args) {
        auto start = std::chrono::steady_clock::now();
        std::forward<decltype(func)>(func)(std::forward<Args>(args)...);
        auto duration = std::chrono::duration_cast< TimeT>
                            (std::chrono::steady_clock::now() - start);
        return duration.count();
    }
};

#endif //FUNCTIONALBADGES_TIMING_H
