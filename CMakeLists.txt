cmake_minimum_required(VERSION 3.10)
project(integrate)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS -pthread)
find_package(Threads REQUIRED)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_executable(integrate main.cpp helpers/helpers.cpp helpers/helpers.h)

target_link_libraries(integrate Threads::Threads)