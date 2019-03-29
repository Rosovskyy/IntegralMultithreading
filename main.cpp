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
#include "headers/helpers.h"
#include "headers/intergration.h"

using std::cout;
using std::string;
using std::cerr;
using std::endl;
using std::ifstream;

int main(int argc, char *argv[]) {
    string filename;
    if (argc != 3) {
        std::cout << "Wrong number of arguments" << std::endl;
        std::cout << "The format is following: <number of threads> <input file>" << std::endl;
        return 1;
    }

    filename = argv[2];
    int thread_num = std::stoi(argv[1]);

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

    run_multi_thread_solution(config, thread_num).print();

    return 0;
}