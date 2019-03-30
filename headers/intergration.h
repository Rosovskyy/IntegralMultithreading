#ifndef INTEGRATE_INTERGRATION_H
#define INTEGRATE_INTERGRATION_

#include <vector>
#include <thread>
#include <math.h>
#include <mutex>
#include "helpers.h"

Result run_multi_thread_solution(configuration config, int threads_num);
Result run_one_thread_solution(configuration config);


#endif //INTEGRATE_INTERGRATION_H
