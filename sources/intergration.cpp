//
// Created by Yevhenii on 29/03/2019.
//

#include "../headers/intergration.h"

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

double integrate(double (*func)(double const &, double const &), configuration conf, size_t steps) {
    double delta_x = (conf.x2 - conf.x1) / steps;
    double delta_y = (conf.y2 - conf.y1) / steps;
    double res = 0;

    for (int i = 0; i < steps; i++) {
        for (int j = 0; j < steps; j++) {
            double area = func(conf.x1 + (delta_x * i), conf.y1 + (delta_y * j));
            area += func(conf.x1 + (delta_x * (i + 1)), conf.y1 + (delta_y * j));
            area += func(conf.x1 + (delta_x * (i + 1)), conf.y1 + (delta_y * (j + 1)));
            area += func(conf.x1 + (delta_x * i), conf.y1 + (delta_y * (j + 1)));
            area /= 4;

            res = res + delta_x * delta_y * area;
        }
    }

    return res;
}

double integrate_atomic(double (*func)(double const &, double const &),
                 int start, int finish, configuration conf, size_t steps, std::atomic<double>& result) {
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

            res = res + delta_x * delta_y * area;
        }
    }

    result = result + res;

    return res;
}

void run_threads(int thread_num, size_t steps, configuration config, std::atomic<double>& result) {
    std::vector <std::thread> threads;
    threads.reserve(static_cast<unsigned long>(thread_num));

//    int value_per_thread = static_cast<int>(ceil(steps / static_cast<double>(thread_num)));
//    int start_value = 0;
//    int end_value = start_value + value_per_thread;

    for (int i = 0; i < thread_num; i++) {
        threads.emplace_back(
                std::thread(integrate_atomic, func_to_integrate, steps / thread_num * i, steps / thread_num * (i + 1), config,
                            steps, std::ref(result)));
//        threads.emplace_back(
//                std::thread(integrate_atomic, func_to_integrate, start_value, end_value, config,
//                            steps, std::ref(result)));
//        if(end_value >= steps)
//            break;
//        start_value += value_per_thread;
//        end_value += value_per_thread;
    }

    for (int i = 0; i < thread_num; ++i) {
        threads[i].join();
    }
    threads.clear();
}

Result run_multi_thread_solution(configuration config, int threads_num){
    std::atomic<double> result {};
    std::atomic_init(&result, 0.0);

    size_t steps = config.initial_steps;
    auto before = get_current_time_fenced();
    run_threads(threads_num, steps, config, std::ref(result));

    double abs_err{-1}; // Just guard value
    double rel_err{-1}; // Just guard value
    double prev_res;
    double cur_res {result};
    bool to_continue {true};

    while (to_continue) {
        prev_res = cur_res;
        steps *= 2;
        result = 0;

        run_threads(threads_num, steps, config, std::ref(result));

        cur_res = result;
        abs_err = fabs(cur_res - prev_res);
        rel_err = fabs((cur_res - prev_res) / cur_res);

        to_continue = (abs_err > config.abs_err);
        to_continue = to_continue && (rel_err > config.rel_err);
        to_continue = to_continue && (steps < config.max_steps);
    }

    auto time_to_calculate = get_current_time_fenced() - before;
    Result r{cur_res, abs_err, rel_err, to_us(time_to_calculate)};
    return r;
}

Result run_one_thread_solution(configuration config){
    size_t steps = config.initial_steps;
    auto before = get_current_time_fenced();

    double abs_err{-1}; // Just guard value
    double rel_err{-1}; // Just guard value
    double prev_res;
    double cur_res {integrate(func_to_integrate, config, steps)};
    bool to_continue {true};

    while (to_continue) {
        prev_res = cur_res;
        steps *= 2;

        cur_res = integrate(func_to_integrate, config, steps);
        abs_err = fabs(cur_res - prev_res);
        rel_err = fabs((cur_res - prev_res) / cur_res);

        to_continue = (abs_err > config.abs_err);
        to_continue = to_continue && (rel_err > config.rel_err);
        to_continue = to_continue && (steps < config.max_steps);
    }

    auto time_to_calculate = get_current_time_fenced() - before;
    Result r{cur_res, abs_err, rel_err, to_us(time_to_calculate)};
    return r;
}