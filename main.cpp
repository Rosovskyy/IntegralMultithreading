#include <iostream>
#include <fstream>
#include <limits>
#include <chrono>
#include <cassert>
#include <atomic>
#include <cmath>
#include <exception>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include <tuple>
#include <fstream>
#include <chrono>

#include "string.h"


using namespace std;

inline std::chrono::steady_clock::time_point get_current_time_fenced() {
    assert(std::chrono::steady_clock::is_steady &&
           "Timer should be steady (monotonic).");
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::steady_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

template<class D>
inline long long to_us(const D& d)
{
    return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
}


struct configuration {
    double rel_err, abs_err;
    double x1, x2;
    double y1, y2;
    size_t initial_steps;
    size_t max_steps;
};


configuration read_conf(istream& cf)
{
    ios::fmtflags flags( cf.flags() ); // Save stream state
    cf.exceptions(std::ifstream::failbit); // Enable exception on fail

    configuration res;
    string temp;

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
    }catch(std::ios_base::failure &fail) // Life without RAII is hard...
    {
        cf.flags( flags );
        throw; // re-throw exception
    }
    cf.flags( flags );
    if( res.x2 <= res.x1 ) {
        throw std::runtime_error("x1 should be <= x2");
    }
    if (res.y2 <= res.y1) {
        throw std::runtime_error("y1 should be <= y2");
    }
    if( res.initial_steps < 10 ) {
        throw std::runtime_error("Too few initial_steps");
    }
    if( res.max_steps < 10 ) {
        throw std::runtime_error("Too small max_steps");
    }
    if( res.abs_err <=0 || res.rel_err <= 0 )
    {
        throw std::runtime_error("Errors should be positive");
    }

    return res;
}


double func_to_integrate(const double& x, const double& y) {
    return sqrt(pow(x, 5)) + pow(y, 6);
}


template<typename func_T>
double integrate(func_T func, double x1, const double& x2, double y1, const double& y2, size_t steps)
{
    double delta_x = (x2 - x1) / steps;
    double delta_y = (y2 - y1) / steps;
    double res = 0;

    for (int i = 0; i < steps; i++) {
        for (int j = 0; j < steps; j++) {
            double area = func(x1 + (delta_x * i), y1 + (delta_y * j));
            area += func(x1 + (delta_x * (i + 1)), y1 + (delta_y * j));
            area += func(x1 + (delta_x * (i + 1)), y1 + (delta_y * (j + 1)));
            area += func(x1 + (delta_x * i), y1 + (delta_y * (j + 1)));
            area /= 4;

            res += delta_x * delta_y * area;
        }
    }

    return res;
}


int main(int argc, char* argv[]) {

    string filename;
    if (argc != 3) {
        std::cout << "Wrong number of arguments" << std::endl;
        std::cout << "The format is following: <number of threads> <input file>" << std::endl;
        return 1;
    }

    filename = argv[2];
    // int thread_num = std::atoi(argv[1]), result = 0;
    // std::vector<std::thread> threads;
    // threads.reserve(thread_num);

    ifstream config_stream(filename);
    if(!config_stream.is_open()) {
        cerr << "Failed to open configuration file " << filename << endl;
        return 2;
    }

    configuration config;
    try{
        config = read_conf(config_stream);
    }catch (std::exception& ex)
    {
        cerr << "Error: " << ex.what() << endl;
        return 3;
    }

    auto before = get_current_time_fenced();


    size_t steps = config.initial_steps;
//    for (int i = 0; i < thread_num; ++i) {
//        threads[i] = std::thread(integrate, func_to_integrate, steps / thread_num * i, steps / thread_num * (i + 1), config.x1, config.x2, config.y1, config.y2, steps);
//        threads.emplace_back(integrate, func_to_integrate, steps / thread_num * i, steps / thread_num * (i + 1), config.x1, config.x2, config.y1, config.y2, steps);
//    }
//
//    for (int i = 0; i < thread_num; ++i) {
//        threads[i].join();
//    }
    double cur_res = integrate(func_to_integrate, config.x1, config.x2, config.y1, config.y2, steps);
    std::cout << cur_res << std::endl;
    double prev_res = cur_res;
    bool to_continue = true;
    double abs_err = -1; // Just guard value
    double rel_err = -1; // Just guard value
// #define PRINT_INTERMEDIATE_STEPS
    while( to_continue )
    {
#ifdef PRINT_INTERMEDIATE_STEPS
        cout << cur_res << " : " << steps << " steps" << endl;
#endif
        prev_res = cur_res;
        steps *= 2;
        cur_res = integrate(func_to_integrate, config.x1, config.x2, config.y1, config.y2, steps);
        abs_err = fabs(cur_res-prev_res);
        rel_err = fabs( (cur_res-prev_res)/cur_res );
#ifdef PRINT_INTERMEDIATE_STEPS
        cout << '\t' << "Abs err : rel err " << abs_err << " : " << rel_err << endl;
#endif

        to_continue = ( abs_err > config.abs_err );
        to_continue = to_continue && ( rel_err > config.rel_err );
        to_continue = to_continue && (steps < config.max_steps);
    }

    auto time_to_calculate = get_current_time_fenced() - before;

    cout << "Result: " << cur_res << endl;
    cout << "Abs err : rel err " << abs_err << " : " << rel_err << endl;
    cout << "Time: " << to_us(time_to_calculate)  << endl;

    return 0;
}