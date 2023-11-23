# Long-Arithmetic-Lib

## Software requirements
CMake, GoogleTests and at least C++17 

## Short project description
The project contains 5 different folders:  
1. `include` - the library header is stored here
2. `src` - stores the library implementation
3. `examples` - stores the main.cpp where you can use the library
4. `tests` - contains the tests using the GoogleTests framework
## Build and Usage

Follow these steps to build and run LongArithmeticLib, including tests and examples:

1. Clone the LongArithmetic repository to your local machine:
```bash
https://github.com/avept/Long-Arithmetic.git
```

2. Navigate to the project directory:
```bash 
cd Long-Arithmetic
```

3. Create a build directory:
```bash
mkdir build
cd build
```
   
4. Generate the build system using CMake:
```bash
cmake ..
```

5. Build the library:
```bash
make -j4
```
   
### Run Tests

1. After building the library, you can run the tests using the following command:
```bash
./test/LongArithmeticTests
```
   
### Run Example Program

1. Once the library is built, you can run the example program by executing:
```bash
./example/main
```
   This program demonstrates the usage of LongArithmetic for long arithmetic operations.

### Usage

To use LongArithmetic in your own project, follow these steps:

1. Include the necessary header files in your C++ code:
```c++
#include "include/BigInt.h"
```

2. Link your project with the LongArithmetic library:
```cmake
target_link_libraries(YourProjectName LongArithmetic)
```

3. Use the provided functions for long arithmetic operations in your code.
