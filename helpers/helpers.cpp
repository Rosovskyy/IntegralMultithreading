//
// Created by Yevhenii on 28/03/2019.
//

#include "helpers.h"

configuration read_conf(std::istream &cf) {
    std::ios::fmtflags flags(cf.flags()); // Save stream state
    cf.exceptions(std::ifstream::failbit); // Enable exception on fail

    configuration res{};
    std::string temp;

    try {
        cf >> res.rel_err;
        getline(cf, temp); // Відкидаємо комент.
        cf >> res.abs_err;
        getline(cf, temp);
        cf >> res.x1;
        getline(cf, temp);
        cf >> res.x2;
        getline(cf, temp);
        cf >> res.y1;
        getline(cf, temp);
        cf >> res.y2;
        getline(cf, temp);
        cf >> res.initial_steps;
        getline(cf, temp);
        cf >> res.max_steps;
        getline(cf, temp);
    } catch (std::ios_base::failure &fail) // Life without RAII is hard...
    {
        cf.flags(flags);
        throw; // re-throw exception
    }
    cf.flags(flags);
    if (res.x2 <= res.x1) {
        throw std::runtime_error("x1 should be <= x2");
    }
    if (res.y2 <= res.y1) {
        throw std::runtime_error("y1 should be <= y2");
    }
    if (res.initial_steps < 10) {
        throw std::runtime_error("Too few initial_steps");
    }
    if (res.max_steps < 10) {
        throw std::runtime_error("Too small max_steps");
    }
    if (res.abs_err <= 0 || res.rel_err <= 0) {
        throw std::runtime_error("Errors should be positive");
    }

    return res;
}
