cmake_minimum_required(VERSION 3.14)

project(LongArithmetic)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(googletest)

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)

add_subdirectory(examples)
