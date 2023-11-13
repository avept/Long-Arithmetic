#include <iostream>
#include "BigInt.h"

int main()
{
    BigInt firstBigNumber("AAAAAAAAAAAAAAAAAAAAAAAAA");
    std::cout << firstBigNumber << std::endl;
    BigInt secondBigNumber("AAA");
    std::cout << secondBigNumber << std::endl;
    BigInt new_value = firstBigNumber / secondBigNumber;

    std::cout << new_value << std::endl;

    return 0;
}       