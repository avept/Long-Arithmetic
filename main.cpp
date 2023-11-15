#include <iostream>
#include "BigInt.h"

int main()
{
    BigInt firstBigNumber("eb9136060911cf8b5bcab72f6f2ecb8c80613a6e4f837ce218f8f8c22e9be28bd1cba1b4e11aca595e528d5282ff2bee0112023f2f916d76924757fe17c3543236718e548eff3e4a84446f698f0cf1dbf8a3c7bfa1ce0018a48126666051ad354b50dd69e73ad0d283cb546a8d3a8758dc980e59731e07e20b9f6a08162701e45");
    std::cout << firstBigNumber << std::endl;
    BigInt secondBigNumber("70af5a9ccba2abce66636df11827665615f17388da46e7f12f92b82ed87ab0ac2a0e401e82803af810649381d72a724a7792ca18eebec84861e9c2533e1794014dbd80e2ffa2f946c7b09acc14c90863853adc439bc267f695c0a9bfea7ebc2ed1a461276667e801fd1ed7102c2f9f52ebcfef5001cd291492d859dab95b9882");
    std::cout << secondBigNumber << std::endl;

    BigInt result = firstBigNumber / secondBigNumber;
    std::cout << result << std::endl;

    return 0;
}       