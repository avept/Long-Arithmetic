add_library(LongArithmetic
    BigInt.cpp
    BigIntModule.cpp
)

target_include_directories(LongArithmetic
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
)