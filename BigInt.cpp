#include "BigInt.h"
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <bitset>
#include <limits>
#include <fstream>
#include <codecvt>
#include <locale>
#include <iostream>
#include <thread>
#include <sstream>
#include <iomanip>

BigInt::BigInt() : m_countOfPartsInNumber(0) 
{ 
    ///
}

BigInt::BigInt(const std::string& bigNumber) : m_countOfPartsInNumber((bigNumber.size() + digitsCountInNumber - 1) / digitsCountInNumber)
{
    m_data.resize(m_countOfPartsInNumber, 0); 

    // msb in bigNumber in left , msb in m_data in right // ABBAAB
    std::string numberInHex = bigNumber; // ABABBABA|ABBAAB
    std::reverse(numberInHex.begin(), numberInHex.end());
    auto partsOfNumber = splitNumberToParts(numberInHex);

    for(std::size_t i = 0; i < m_countOfPartsInNumber; ++i)
    {
        if(partsOfNumber.at(i)[0] == '0')
        {
            std::reverse(partsOfNumber.at(i).begin(), partsOfNumber.at(i).end());
        }
        m_data[i] = std::strtoull(partsOfNumber.at(i).data(), nullptr, 16);
    }
}

BigInt::BigInt(const uint_fast64_t number) : m_countOfPartsInNumber(1)
{
    m_data.push_back(number);
}

// BigInt::BigInt(const BigInt& bigNumber) : m_data(bigNumber.m_data), m_countOfPartsInNumber(bigNumber.m_countOfPartsInNumber)
// {
//     ///
// }

// BigInt::BigInt(BigInt&& bigNumber) noexcept : m_data(bigNumber.m_data), m_countOfPartsInNumber(bigNumber.m_countOfPartsInNumber)
// {
//     ///
// }

std::size_t BigInt::size() const noexcept
{
    return m_data.size();
}

std::vector<std::string> BigInt::splitNumberToParts(const std::string_view& str) const noexcept
{
    std::vector<std::string> splitsStr(m_countOfPartsInNumber);

    for(std::size_t i = 0; i < m_countOfPartsInNumber; ++i)
    {
        std::size_t lhs = i * BigInt::digitsCountInNumber;
        std::size_t rhs = std::min(BigInt::digitsCountInNumber, str.size() - lhs);

        splitsStr[i] = (str.substr(lhs, rhs));
    }

    return splitsStr;
}

BigInt BigInt::operator+(BigInt& rhs)
{
    uint_fast64_t carry = 0;

    if(std::size_t lhsSize = size(); lhsSize > rhs.size())
        rhs.m_data.resize(lhsSize);
    else
        m_data.resize(rhs.size());

    BigInt result;
    result.m_data.resize(size());
    
    for(std::size_t i = 0; i < size(); ++i)
    {
        uint_fast64_t temp = m_data[i] + rhs.m_data[i] + carry;
        result.m_data[i] = (temp & 0xFFFFFFFF);
        carry = temp >> BigInt::bitsCount;
    }

    if(carry > 0)
    {
        result.m_data.push_back(carry);
    } 

    result.normalize();
    return result;
}

BigInt BigInt::operator-(BigInt& rhs)
{
    if(rhs > *this) 
    {
        std::cout << "Error: Bigger number is substracted from the smaller, returning smaller one...\n";
        return *this;
    }

    BigInt result;
    uint_fast64_t borrow = 0;

    int_fast64_t l1 = rhs.size();
    int_fast64_t l2 = size();

    auto addResult = [&](const auto& temp)
    {
        if(temp >= 0)
        {
            result.m_data.push_back(temp);
            borrow = 0;
        }
        else 
        {
            result.m_data.push_back(temp + (1ULL << 32));
            borrow = 1; 
        }
    };

    for(std::size_t i = 0; i < l1; ++i)
    {
        int_fast64_t temp = static_cast<int_fast64_t>(m_data[i]) - static_cast<int_fast64_t>(rhs.m_data[i]) - 
                                static_cast<int_fast64_t>(borrow); // used signed int because temp can be negative

        addResult(temp);
    }

    for(std::size_t i = l1; i < l2; ++i)
    {
        int_fast64_t temp = static_cast<int_fast64_t>(m_data[i]) - 
                                static_cast<int_fast64_t>(borrow); // used signed int because temp can be negative

        addResult(temp);
    }

    result.normalize();
    return result;
}

BigInt BigInt::operator*(BigInt& rhs)
{
    BigInt result;

    if(std::size_t lhsSize = m_data.size(); lhsSize > rhs.size())
        rhs.m_data.resize(lhsSize);
    else
        m_data.resize(rhs.size());

    for(std::size_t i = 0; i < rhs.size(); ++i)
    {
        BigInt temp = longMulOneDigit(*this, rhs.m_data[i]);
        temp = longShiftDigitsToHigh(temp, i);
        result = result + temp;
    }

    return result;
}

BigInt BigInt::operator/(const BigInt& rhs)
{    
    if(*this == rhs)
        return BigInt(1);

    int_fast64_t bitLength = rhs.bitLength();
    BigInt remainder(*this);
    BigInt quotient(0);

    while(remainder >= rhs)
    {   
        int_fast64_t remainderLength = remainder.bitLength();
        BigInt shiftedDivisor = longShiftBitsToHigh(rhs, remainderLength - bitLength);
        if(remainder < shiftedDivisor)
        {
            remainderLength -= 1;
            int_fast64_t length = remainderLength - bitLength;
            shiftedDivisor = longShiftBitsToHigh(rhs, length);
        }

        remainder = remainder - shiftedDivisor;
        
        BigInt temp(1);
        temp = longShiftBitsToHigh(temp, remainderLength - bitLength);
        quotient = quotient + temp;
    }

    return quotient;
}

// BigInt& BigInt::operator=(const BigInt& rhs)
// {
//     m_data = rhs.m_data;
//     m_countOfPartsInNumber = rhs.m_countOfPartsInNumber;

//     return *this;
// }

// BigInt BigInt::operator=(BigInt&& rhs) noexcept
// {
//     m_data = rhs.m_data;
//     m_countOfPartsInNumber = rhs.m_countOfPartsInNumber;

//     return *this;
// }

bool BigInt::operator == (const BigInt& rhs) const
{
    if(m_data.size() != rhs.size())
        return false;

    if(std::equal(m_data.begin(), m_data.end(), rhs.m_data.begin(), rhs.m_data.end()))
        return true;

    return false;
}

bool BigInt::operator != (const BigInt& rhs) const
{
    return !(*this == rhs);
}

bool BigInt::operator > (const BigInt& rhs) const
{
    if(size() == 0 || rhs.size() == 0)
    {
        std::cout << "Unknown case" << std::endl;
        return false;
    }
        
    if(size() > rhs.size())
        return true;
    else if(rhs.size() > size())
        return false;

    int_fast64_t i = m_data.size() - 1;
    while(i >= 0)
    {
        if(m_data.at(i) != rhs.m_data.at(i))
        {
            break;
        }
        i--;
    }

    if(i == -1)
        return false;
    else
        return m_data.at(i) > rhs.m_data.at(i);
}

bool BigInt::operator < (const BigInt& rhs) const
{
    return (rhs > *this);
}

bool BigInt::operator <= (const BigInt& rhs) const
{
    return (*this < rhs) || (*this == rhs);
}

bool BigInt::operator >= (const BigInt& rhs) const
{
    return (*this > rhs) || (*this == rhs);
}

BigInt BigInt::longMulOneDigit(const BigInt& lhs, const uint_fast64_t number) const noexcept
{
    BigInt result;

    uint_fast64_t carry = 0;
    // uint_fast64_t remainder = std::pow(2, 32) - 1;

    for(std::size_t i = 0; i < lhs.size(); ++i)
    {
        uint_fast64_t temp = (lhs.m_data[i] * number) + carry;
        result.m_data.push_back(temp & 0xFFFFFFFF);
        carry = temp >> BigInt::bitsCount;
    }

    if(carry > 0)
    {
        result.m_data.push_back(carry);
    }

    return result;
}

BigInt BigInt::longShiftDigitsToHigh(const BigInt& number, const uint_fast64_t widthShift) const noexcept
{
    BigInt result = number;

    for(std::size_t i = 0; i < widthShift; ++i)
    {
        result.m_data.insert(result.m_data.begin(), 0);
    }

    return result;
}

BigInt BigInt::longShiftBitsToHigh(const BigInt& number, int_fast64_t widthShift) const noexcept
{
    if(widthShift <= 0 || number == BigInt(0))
        return number;

    int_fast64_t remainder = widthShift % BigInt::bitsCount;
    widthShift /= BigInt::bitsCount; // спочатку вставляю widthShift цифр, а потім зсуваю на remainder біт
    
    BigInt result = longShiftDigitsToHigh(number, widthShift);

    if(remainder > 0)
    {
        for(std::size_t i = 0; i < remainder; ++i)
        {
            uint_fast64_t lastBit = (result.m_data.back() >> (BigInt::bitsCount - 1)) & 1;

            for(std::size_t j = result.size() - 1; j > 0; --j)
            {
                result.m_data[j] = (result.m_data[j] << 1) ^ ((result.m_data[j - 1] >> (BigInt::bitsCount - 1)) & 1) & 0xFFFFFFFF;
            }
            result.m_data[0] = (result.m_data[0] << 1) & 0xFFFFFFFF;

            if(lastBit != 0)
            {
                result.m_data.push_back(lastBit);
            }
        }
    }
    
    result.normalize();
    return result;
}

std::size_t BigInt::bitLength() const noexcept
{
    if(*this == BigInt(0))
        return 0;

    std::size_t bitsCountWithoutLast = (size() - 1) * BigInt::bitsCount;
    auto lastElementInBinary = std::bitset<32>(m_data.back()).to_string();
    std::size_t bitsCountOfLastElement = lastElementInBinary.erase(0, std::min(lastElementInBinary.find_first_not_of('0'),
                                                                        lastElementInBinary.size() - 1)).size();

    return bitsCountWithoutLast + bitsCountOfLastElement;
}

void BigInt::normalize() noexcept
{
    while(m_data.back() == 0)
    {
        m_data.pop_back();
    }

    if(m_data.empty())
    {
        m_data.push_back(0);
    }
}

std::ostream& operator<<(std::ostream& out, const BigInt& bigInt)
{
    out << "Number: ";
    for(std::size_t i = 0; i < bigInt.size(); ++i)
    {
       out << "(2^32)^" << i << " * " << bigInt.m_data.at(i) << " + ";
    }
    out << std::endl;

    out << "Number in hex: ";

    for(auto it = bigInt.m_data.rbegin(); it != bigInt.m_data.rend(); ++it)
    {
        std::stringstream stream;
        stream << std::hex << *it;
        out << stream.str();
    }
    out << std::endl;

    // доробити вивід нулів для не останньої цифри

    return out;
}
