#include <iostream>
#include "../include/BigInt.h"

int main()
{
    BigInt firstBigNumber("23C0D0050AE991EF232C32AA88639EF38290F68434288F9C4BF429FE499B9220ABCA87E41472A1E15199B20E6CFDFE13CBB4E1F9358760F04A0FD9EA8A2B5A98E2CF08A5D0BFCC4C75B0F0A7F7BC57EFBDF2ACDC6CA08F63DA3B3A040CFA1AABC5F07A4B843282AD6E6FD1ECB9A00800F46446DCD357ACA4BE192CBF38282A9A");
    BigInt secondBigNumber("1604077CEA1866B49C76BA21B38F3F0DE1DA56A26E876A4C5236EA0EB2B4C9BE1CF0CAD51CD5310E778A3A464E252B7215F3BE74DCFAB0C09DEDE3FA462A06C");
    std::cout << firstBigNumber + secondBigNumber << std::endl;
    std::cout << secondBigNumber - firstBigNumber << std::endl;
    std::cout << firstBigNumber * secondBigNumber << std::endl;
    std::cout << firstBigNumber / secondBigNumber << std::endl;
    std::cout << firstBigNumber % secondBigNumber << std::endl;


    return 0;
}       