#include "../include/BigIntModule.h"
#include "BigIntModule.h"

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
    BigInt minus = m_data - (rhs.m_data);
    BigInt result = minus % m_module;

    return BigIntModule(result);
}

BigIntModule BigIntModule::operator*(const BigIntModule& rhs) const
{
    BigInt multiplication = m_data * (rhs.m_data);
    BigInt result = multiplication % m_module;

    return BigIntModule(result);
}

BigIntModule BigIntModule::operator/(const BigIntModule& rhs) const
{
    BigInt divide = m_data + (rhs.m_data);
    BigInt result = divide % m_module;

    return BigIntModule(result);
}

std::ostream& operator<<(std::ostream& out, const BigIntModule& bigInt)
{
    out << bigInt.m_data;
    return out;
}