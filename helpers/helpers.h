//
// Created by Yevhenii on 28/03/2019.
//

#ifndef INTEGRATE_HELPERS_H
#define INTEGRATE_HELPERS_H

#include <iostream>
#include <fstream>
#include <chrono>
#include <cassert>
#include <atomic>
#include <string>

inline std::chrono::steady_clock::time_point get_current_time_fenced() {
    assert(std::chrono::steady_clock::is_steady &&
           "Timer should be steady (monotonic).");
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::steady_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

template<class D>
inline long long to_us(const D &d) {
    return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
}

struct configuration {
    double rel_err, abs_err;
    double x1, x2;
    double y1, y2;
    size_t initial_steps;
    size_t max_steps;
};

configuration read_conf(std::istream &cf);

#endif //INTEGRATE_HELPERS_H
