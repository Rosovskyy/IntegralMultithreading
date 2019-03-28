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

#include "string.h"
#include "helpers/helpers.h"

using std::cout;
using std::string;
using std::cerr;
using std::endl;
using std::ifstream;

double func_to_integrate(const double &x, const double &y) {
    double result = 0;
    for (int i = -2; i <= 2; ++i) {
        for (int j = -2; j <= 2; ++j) {
            result += 1 / (5 * (i + 2) + j + 3 + pow(x - 16 * j, 6) + pow(y - 16 * i, 6));
        }
    }
    result += 0.002;
    return pow(result, -1);
}


// template<typename func_T>
double integrate(double (*func)(double const &, double const &),
                 int start, int finish, configuration conf, size_t steps, double &result) {
    double delta_x = (conf.x2 - conf.x1) / steps;
    double delta_y = (conf.y2 - conf.y1) / steps;
    double res = 0;

    for (int i = start; i < finish; i++) {
        for (int j = 0; j < steps; j++) {
            double area = func(conf.x1 + (delta_x * i), conf.y1 + (delta_y * j));
            area += func(conf.x1 + (delta_x * (i + 1)), conf.y1 + (delta_y * j));
            area += func(conf.x1 + (delta_x * (i + 1)), conf.y1 + (delta_y * (j + 1)));
            area += func(conf.x1 + (delta_x * i), conf.y1 + (delta_y * (j + 1)));
            area /= 4;

            res += delta_x * delta_y * area;
        }
    }

    result += res;

    return res;
}


void createThreads(std::vector<std::thread> &threads, int thread_num, double (*func)(double const &, double const &),
                   configuration config, size_t steps, double &result) {
    for (int i = 0; i < thread_num; i++) {
        threads.emplace_back(
                std::thread(integrate, func_to_integrate, steps / thread_num * i, steps / thread_num * (i + 1), config,
                            steps, std::ref(result)));
    }

    for (int i = 0; i < thread_num; ++i) {
        threads[i].join();
    }
}


int main(int argc, char *argv[]) {

    string filename;
    if (argc != 3) {
        std::cout << "Wrong number of arguments" << std::endl;
        std::cout << "The format is following: <number of threads> <input file>" << std::endl;
        return 1;
    }

    filename = argv[2];
    int thread_num = std::stoi(argv[1]);
    std::vector <std::thread> threads;

    ifstream config_stream(filename);
    if (!config_stream.is_open()) {
        cerr << "Failed to open configuration file " << filename << endl;
        return 2;
    }

    configuration config{};
    try {
        config = read_conf(config_stream);
    } catch (std::exception &ex) {
        cerr << "Error: " << ex.what() << endl;
        return 3;
    }

    double result = 0;

    auto before = get_current_time_fenced();

    size_t steps = config.initial_steps;

    createThreads(std::ref(threads), thread_num, func_to_integrate, config, steps, std::ref(result));

    threads.clear();

    double cur_res = result;
    result = 0;
    std::cout << cur_res << std::endl;
    double prev_res = cur_res;
    bool to_continue = true;
    double abs_err = -1; // Just guard value
    double rel_err = -1; // Just guard value

// #define PRINT_INTERMEDIATE_STEPS
    while (to_continue) {
#ifdef PRINT_INTERMEDIATE_STEPS
        cout << cur_res << " : " << steps << " steps" << endl;
#endif
        prev_res = cur_res;
        steps *= 2;
        result = 0;

        createThreads(std::ref(threads), thread_num, func_to_integrate, config, steps, std::ref(result));

        threads.clear();

        cur_res = result;
        abs_err = fabs(cur_res - prev_res);
        rel_err = fabs((cur_res - prev_res) / cur_res);
#ifdef PRINT_INTERMEDIATE_STEPS
        cout << '\t' << "Abs err : rel err " << abs_err << " : " << rel_err << endl;
#endif

        to_continue = (abs_err > config.abs_err);
        to_continue = to_continue && (rel_err > config.rel_err);
        to_continue = to_continue && (steps < config.max_steps);
    }

    auto time_to_calculate = get_current_time_fenced() - before;

    cout << "Result: " << cur_res << endl;
    cout << "Abs err : rel err " << abs_err << " : " << rel_err << endl;
    cout << "Time: " << to_us(time_to_calculate) << endl;

    return 0;
}