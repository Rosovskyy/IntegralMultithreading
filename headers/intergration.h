//
// Created by Yevhenii on 29/03/2019.
//

#ifndef INTEGRATE_INTERGRATION_H
#define INTEGRATE_INTERGRATION_

#include <vector>
#include <thread>
#include <math.h>
#include "helpers.h"

double func_to_integrate(const double &x, const double &y);
void run_threads(int thread_num, size_t steps, configuration config, double &result);
double integrate(double (*func)(double const &, double const &),
                 int start, int finish, configuration conf, size_t steps, double &result);
Result run_multi_thread_solution(configuration config, int threads_num);


#endif //INTEGRATE_INTERGRATION_H
