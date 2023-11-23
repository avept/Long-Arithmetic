find_package(GTest REQUIRED)

add_executable(LongArithmeticTests
    BigIntTests.cpp
    BigIntNumTests.cpp
)

target_link_libraries(LongArithmeticTests
    LongArithmetic
    GTest::Main  
    pthread
)

include(GoogleTest)
gtest_discover_tests(LongArithmeticTests)   