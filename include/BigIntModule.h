#pragma once

#include "BigInt.h"

class BigIntModule 
{
public:
    explicit BigIntModule() = default;
    explicit BigIntModule(const std::string& hexNumber);
    explicit BigIntModule(const BigInt& bigInt);

    static void setModule(const BigInt& module);

    BigIntModule& operator=(const BigIntModule& other) = default;
    
    BigIntModule operator+(const BigIntModule& rhs) const;
    BigIntModule operator-(const BigIntModule& rhs) const;
    BigIntModule operator*(const BigIntModule& rhs) const;
    BigIntModule operator/(const BigIntModule& rhs) const;

    friend std::ostream& operator<<(std::ostream& out, const BigIntModule& bigInt);

private:    

    /* members */
    BigInt m_data;
    static BigInt m_module;

};