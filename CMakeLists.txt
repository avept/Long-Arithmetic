cmake_minimum_required(VERSION 3.10)

project(LongArithmetic)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    BigInt.cpp
    main.cpp
)

set(HEADERS
    BigInt.h
)

add_executable(LongArithmetic ${SOURCES} ${HEADERS})
