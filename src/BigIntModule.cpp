#include "../include/BigIntModule.h"
#include "BigIntModule.h"
#include <iostream>

BigInt BigIntModule::m_module;

BigIntModule::BigIntModule(const BigInt& bigInt) : m_data(bigInt)
{
    ///
}

BigIntModule::BigIntModule(const std::string& hexNumber) : m_data(BigInt(hexNumber))
{
    ///
}

void BigIntModule::setModule(const BigInt& module)
{
    m_module = module;
}

BigIntModule BigIntModule::operator+(const BigIntModule& rhs) const
{
    BigInt sum = m_data + (rhs.m_data);
    BigInt result = sum % m_module;

    return BigIntModule(result);
}

BigIntModule BigIntModule::operator-(const BigIntModule& rhs) const
{
    BigInt minus;
    BigInt result;
    if(m_data < rhs.m_data)
    {
        minus = rhs.m_data - m_data;

        minus = minus % m_module;
        result = m_module - minus;
    }
    else
    {
        minus = m_data - rhs.m_data;
        result = minus % m_module;
    }

    return BigIntModule(result);
}

BigIntModule BigIntModule::operator*(const BigIntModule& rhs) const
{
    BigInt v1 = m_data % m_module;
    BigInt v2 = rhs.m_data % m_module;
    BigInt multiplication = v1 * v2;
    BigInt result = multiplication % m_module;

    return BigIntModule(result);
}

BigIntModule BigIntModule::operator/(const BigIntModule& rhs) const
{
    BigInt divide = m_data / (rhs.m_data);
    BigInt result = divide % m_module;

    return BigIntModule(result);
}

BigIntModule BigIntModule::square() const
{
    BigInt square = m_data.square();
    BigInt result = square % m_module;
    
    return BigIntModule(result);
}

BigIntModule BigIntModule::longModPowerBarret(const BigIntModule& degree)
{
    BigInt C(1);
    BigInt mult = m_data % m_module;

    BigInt mu = BigInt::evaluateMu(m_module);

    std::string degreeInBinary = degree.m_data.binaryString();
    for(int64_t i = 0; i < degreeInBinary.size(); ++i)
    {
        if(degreeInBinary.at(i) == '1')
        {
            BigInt temp = C * mult;
            C = temp.barretReduction(m_module, mu);
        }
        
        BigInt multSquare = mult.square();
        mult = multSquare.barretReduction(m_module, mu);
    }
    
    return BigIntModule(C);
}

std::ostream& operator<<(std::ostream& out, const BigIntModule& bigInt)
{
    out << bigInt.m_data;
    return out;
}