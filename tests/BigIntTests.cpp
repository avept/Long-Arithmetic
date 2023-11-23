#include <gtest/gtest.h>
#include "../include/BigInt.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// TEST(BigIntTests, BasicAssertions) {
//   // Expect two strings not to be equal.
//   EXPECT_STRNE("hello", "world");
//   // Expect equality.
//   EXPECT_EQ(7 * 6, 42);
// }

// TEST_F(BigIntTests, AdditionOperatorTest) {
//     BigInt ans = bn1 + bn2;
//     EXPECT_EQ(2, 2);
// }