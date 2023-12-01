#include <iostream>
#include "../include/BigInt.h"
#include "../include/BigIntModule.h"

int main()
{
    BigIntModule firstBigNumber("7");
    BigIntModule secondBigNumber("5");

    BigIntModule::setModule(BigInt("7"));
    std::cout << firstBigNumber + secondBigNumber << std::endl;

    return 0;
}       