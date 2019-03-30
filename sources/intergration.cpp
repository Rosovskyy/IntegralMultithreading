#include "../headers/intergration.h"

double func_to_integrate(const double &x, const double &y) {
    double result = 0;
    for (int i = -2; i <= 2; ++i) {
        for (int j = -2; j <= 2; ++j) {
            try {
                result += 1 / (5 * (i + 2) + j + 3 + pow(x - 16 * j, 6) + pow(y - 16 * i, 6));
            } catch (int l) {
                std::cout << "Division by zero!" << std::endl;
            }

        }
    }
    result += 0.002;
    return pow(result, -1);
}

double integrate(double (*func)(double const &, double const &), configuration conf, size_t steps) {
    try {
        if (steps <= 0) {
            throw "Incorrect number of steps";
        }
    } catch (char *str) {
        std::cout << str << std::endl;
        return 0;
    }
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

double integrate_mutex(double (*func)(double const &, double const &),
                 int start, int finish, configuration conf, size_t steps, double& result, std::mutex& m) {
    try {
        if (steps <= 0) {
            throw "Incorrect number of steps";
        }
    } catch (char *str) {
        std::cout << str << std::endl;
        return 0;
    }
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
    m.lock();
    result = result + res;
    m.unlock();
    return res;
}

void run_threads(int thread_num, size_t steps, configuration config, double& result) {
    std::vector <std::thread> threads;
    threads.reserve(static_cast<unsigned long>(thread_num));
    std:: mutex m;

    size_t value_per_thread = steps / thread_num;
    size_t start_value = 0;
    size_t end_value = start_value + value_per_thread;

    for (int i = 0; i < thread_num; i++) {
        try {
        threads.emplace_back(
                std::thread(integrate_mutex, func_to_integrate, start_value, end_value, config,
                            steps, std::ref(result), std::ref(m)));
        } catch (std::exception &ex) {
            std::cout << "Error: " << ex.what() << std::endl;
            throw;
        }

        start_value += value_per_thread;
        end_value += value_per_thread;

        if(i == thread_num - 2)
            end_value = steps;
    }

    for (int i = 0; i < thread_num; ++i) {
        threads[i].join();
    }

    threads.clear();
}

Result run_multi_thread_solution(configuration config, int threads_num){
    double result = 0;

    size_t steps = config.initial_steps;
    auto before = get_current_time_fenced();
    run_threads(threads_num, steps, config, std::ref(result));

    double abs_err{-1};
    double rel_err{-1};
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
        try {
            rel_err = fabs((cur_res - prev_res) / cur_res);
        } catch (std::exception &ex) {
            std::cout << "cur_res can't be 0.." << std::endl;
            throw;
        }

        to_continue = (abs_err > config.abs_err);
        to_continue = to_continue || (rel_err > config.rel_err);
        to_continue = to_continue && (steps < config.max_steps);
    }

    auto time_to_calculate = get_current_time_fenced() - before;
    Result r{cur_res, abs_err, rel_err, to_us(time_to_calculate)};
    return r;
}

Result run_one_thread_solution(configuration config){
    size_t steps = config.initial_steps;
    auto before = get_current_time_fenced();

    double abs_err{-1};
    double rel_err{-1};
    double prev_res;
    double cur_res {integrate(func_to_integrate, config, steps)};
    bool to_continue {true};

    while (to_continue) {
        prev_res = cur_res;
        steps *= 2;

        cur_res = integrate(func_to_integrate, config, steps);
        abs_err = fabs(cur_res - prev_res);
        try {
            rel_err = fabs((cur_res - prev_res) / cur_res);
        } catch (std::exception &ex) {
            std::cout << "cur_res can't be 0.." << std::endl;
            throw;
        }

        to_continue = (abs_err > config.abs_err);
        to_continue = to_continue || (rel_err > config.rel_err);
        to_continue = to_continue && (steps < config.max_steps);
    }

    auto time_to_calculate = get_current_time_fenced() - before;
    Result r{cur_res, abs_err, rel_err, to_us(time_to_calculate)};
    return r;
}