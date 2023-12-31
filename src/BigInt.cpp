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
#include <random>

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
        std::reverse(partsOfNumber.at(i).begin(), partsOfNumber.at(i).end());
        m_data[i] = std::strtoull(partsOfNumber.at(i).data(), nullptr, 16);
    }
}

BigInt::BigInt(const uint_fast64_t number) : m_countOfPartsInNumber(1)
{
    m_data.push_back(number);
}

std::size_t BigInt::size() const noexcept
{
    return m_data.size();
}

std::size_t BigInt::digitsCount() const noexcept
{
    std::size_t count = bitLength();

    if(count % 32 > 0)
        return (count / 32) + 1;    

    return count / 32;
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

BigInt BigInt::operator+(const BigInt& rhs) const
{
    BigInt result;
    uint_fast64_t carry = 0;

    auto addResult = [&](const auto& temp)
    {
        result.m_data.push_back(temp & 0xFFFFFFFF);
        carry = temp >> BigInt::bitsCount;
    };

    int_fast64_t l1;
    int_fast64_t l2;
    
    auto adder = [&](auto& a, auto& b)
    {
        for(std::size_t i = 0; i < l1; ++i)
        {
            int_fast64_t temp = a.m_data[i] + b.m_data[i] + carry;

            addResult(temp);
        }

        for(std::size_t i = l1; i < l2; ++i)
        {
            int_fast64_t temp = a.m_data[i] + carry;

            addResult(temp);
        }
    };

    if(rhs.size() > size())
    {
        l1 = size();
        l2 = rhs.size();

        adder(rhs, *this);
    }
    else
    {
        l1 = rhs.size();
        l2 = size();

        adder(*this, rhs);
    }

    result.normalize();
    return result;
}

BigInt BigInt::operator-(const BigInt& rhs) const
{
    if(rhs > *this) 
    {
        std::cout << *this << std::endl;
        std::cout << rhs << std::endl;
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

BigInt BigInt::operator*(const BigInt& rhs) const
{
    BigInt result;

    for(std::size_t i = 0; i < rhs.size(); ++i)
    {
        BigInt temp = longMulOneDigit(*this, rhs.m_data[i]);
        temp = longShiftDigitsToHigh(temp, i);
        result = result + temp;
    }

    return result;
}

BigInt BigInt::operator/(const BigInt& rhs) const
{    
    return longDivMod(rhs).first;
}

BigInt BigInt::operator%(const BigInt& rhs) const
{
    return longDivMod(rhs).second;
}

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

BigInt BigInt::gcd(const BigInt& rhs) const
{
    if((*this) == BigInt(0) || rhs == BigInt(0))
    {
        return BigInt(0);
    }

    int_fast64_t d = 0;
    BigInt a = *this;
    BigInt b = rhs;

    while(((a.m_data[0] & 1) == 0) && ((b.m_data[0] & 1) == 0))
    {
        a = longShiftBitsToDown(a, 1);
        b = longShiftBitsToDown(b, 1);
        d += 1;
    }

    while((a.m_data[0] & 1) == 0)
    {
        a = longShiftBitsToDown(a, 1);
    }

    while(b != 0)
    {
        while((b.m_data[0] & 1) == 0)
        {
            b = longShiftBitsToDown(b, 1);
        }

        BigInt min = a;
        if(a > b)
        {
            min = b; 
        }

        if(a >= b)
        {
            b = a - b;
        }
        else
        {
            b = b - a;
        }
        a = min;
    }

    BigInt dd = longShiftBitsToHigh(a, d);
    return dd;
}

BigInt BigInt::lcm(const BigInt& rhs) const
{
    return ((*this) * rhs) / (*this).gcd(rhs); 
}

BigInt BigInt::barretReduction(const BigInt& module, const BigInt& mu)
{
    std::size_t k = module.digitsCount();
    BigInt q = longShiftBitsToDown((*this), ((k - 1) * 32));
    q = q * mu;
    q = longShiftBitsToDown(q, ((k + 1) * 32));

    BigInt reduction = (*this) - q * module;
    while(reduction >= module)
        reduction = reduction - module;

    return reduction;
}

bool BigInt::operator > (const BigInt& rhs) const
{
    if(size() == 0 || rhs.size() == 0)
        return false;
        
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
    if(size() == 0 || rhs.size() == 0)
    {
        std::cout << "Unknown case" << std::endl;
        return false;
    }
        
    if(size() < rhs.size())
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
        return m_data.at(i) < rhs.m_data.at(i);
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
                result.m_data[j] = ((result.m_data[j] << 1) ^ ((result.m_data[j - 1] >> (BigInt::bitsCount - 1)) & 1)) & 0xFFFFFFFF;
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

BigInt BigInt::longShiftBitsToDown(const BigInt& number, int_fast64_t widthShift) const noexcept
{
    if(widthShift <= 0 || number == BigInt(0))
        return number;

    int_fast64_t remainder = widthShift % BigInt::bitsCount;
    widthShift /= BigInt::bitsCount;
    
    BigInt result = number;
    for(std::size_t i = 0; i < widthShift; ++i)
    {
        result.m_data.erase(result.m_data.begin());
    }

    if(remainder > 0)
    {
        for(std::size_t i = 0; i < remainder; ++i)
        {
            for(std::size_t j = 0; j < result.size() - 1; ++j)
            {
                result.m_data[j] = ((result.m_data[j] >> 1) ^ ((result.m_data[j + 1] & 1) << (BigInt::bitsCount - 1))) & 0xFFFFFFFF;
            }
            result.m_data[result.size() - 1] = (result.m_data[result.size() - 1] >> 1) & 0xFFFFFFFF;
        }
    }
    
    result.normalize();
    return result;
}

std::pair<BigInt, BigInt> BigInt::longDivMod(const BigInt& rhs) const noexcept
{
    if(*this == rhs)
        return std::make_pair(BigInt(1), BigInt(0));

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

    return std::make_pair(quotient, remainder);
}

std::string BigInt::binaryString() const noexcept
{
    std::string binaryString;
    for(const auto& it : m_data)
    {
        for(int j = 0; j <= 31; ++j)
        {
            binaryString += (it & (1 << j)) ? '1' : '0';
        }
    }

    return normalize(binaryString);
}

BigInt BigInt::randomNumber(const int64_t digitsCount) noexcept
{
    if (digitsCount <= 0) 
        return BigInt(0);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> distribution(0, UINT32_MAX);

    BigInt number;
    number.m_data.resize(digitsCount);
    for (int i = 0; i < digitsCount; i++) 
        number.m_data.at(i) = distribution(gen);

    return number;
}

BigInt BigInt::square() const noexcept
{
    return (*this) * (*this);
}

BigInt BigInt::longPower(const BigInt& power) noexcept
{
    auto binaryRepresentation = power.binaryString();
    BigInt result(1);

    for(int64_t i = binaryRepresentation.size() - 1; i >= 0; --i)
    {
        if(binaryRepresentation.at(i) == '1')
            result = result * (*this);

        if(i > 0)
            *this = this->square();
    }

    return result;
}

std::size_t BigInt::bitLength() const noexcept
{
    if(*this == BigInt(0) || m_data.empty())
        return 0;

    std::size_t bitsCountWithoutLast = (size() - 1) * BigInt::bitsCount;
    auto lastElementInBinary = std::bitset<32>(m_data.back()).to_string();
    std::size_t bitsCountOfLastElement = lastElementInBinary.erase(0, std::min(lastElementInBinary.find_first_not_of('0'),
                                                                        lastElementInBinary.size() - 1)).size();
    return bitsCountWithoutLast + bitsCountOfLastElement;
}

void BigInt::normalize() noexcept
{
    if(size() > 0)
    {
        while(m_data.back() == 0 && m_data.size() > 1)
        {
            m_data.pop_back();
        }
    }
}

std::string BigInt::normalize(const std::string& str) const noexcept
{
   auto binaryString = str;
    if (std::size_t startPos = binaryString.find_last_not_of('0'); startPos != std::string::npos) 
        binaryString = binaryString.substr(0, startPos + 1);
    else 
        binaryString = "0"; 

    return binaryString;
}

BigInt BigInt::evaluateMu(const BigInt& module) noexcept
{
    std::size_t count = module.digitsCount();
    BigInt mu(1);
    mu = mu.longShiftBitsToHigh(mu, 2 * 32 * count);

    return mu / module;
}

void BigInt::evaluateTime() const noexcept
{
    std::vector<int64_t> sizes = { 31 };

    for (const auto size : sizes) 
    {
        double TotalAdditionTime = 0.0;
        double TotalSubtractionTime = 0.0;
        double TotalMultiplicationTime = 0.0;
        double TotalDivisionTime = 0.0;
        double TotalReductionTime = 0.0;

        for (int i = 0; i < 100; i++) 
        {
            BigInt bi1, bi2;
            bi1.randomNumber(size);
            bi2.randomNumber(size);

            auto AdditionStartTime = std::chrono::high_resolution_clock::now();
            BigInt resultAdd = bi1 + bi2;
            auto AdditionEndTime = std::chrono::high_resolution_clock::now();
            auto additionTime = std::chrono::duration_cast<std::chrono::nanoseconds>(AdditionEndTime - AdditionStartTime).count();
            TotalAdditionTime += additionTime;

            auto SubtractionStartTime = std::chrono::high_resolution_clock::now();
            BigInt resultSub = bi1 - bi2;
            auto SubtractionEndTime = std::chrono::high_resolution_clock::now();
            auto substractionTime = std::chrono::duration_cast<std::chrono::nanoseconds>(SubtractionEndTime - SubtractionStartTime).count();
            TotalSubtractionTime += substractionTime;

            auto MultiplicationStartTime = std::chrono::high_resolution_clock::now();
            BigInt resultMul = bi1 * bi2;
            auto MultiplicationEndTime = std::chrono::high_resolution_clock::now();
            auto multiplicationTime = std::chrono::duration_cast<std::chrono::nanoseconds>(MultiplicationEndTime - MultiplicationStartTime).count();
            TotalMultiplicationTime += multiplicationTime;

            auto DivisionStartTime = std::chrono::high_resolution_clock::now();
            BigInt resultDiv = bi1 / bi2;
            auto DivisionEndTime = std::chrono::high_resolution_clock::now();
            auto divisionTime = std::chrono::duration_cast<std::chrono::nanoseconds>(DivisionEndTime - DivisionStartTime).count();
            TotalDivisionTime += divisionTime;

            auto ReductionStartTime = std::chrono::high_resolution_clock::now();
            BigInt resultRed = bi1 % bi2;
            auto ReductionEndTime = std::chrono::high_resolution_clock::now();
            auto reductionTime = std::chrono::duration_cast<std::chrono::nanoseconds>(ReductionEndTime - ReductionStartTime).count();
            TotalReductionTime += reductionTime;

            // std::cout << "Size " << size << std::endl;
            // std::cout << "Addition Time: " << additionTime << " nanoseconds" << std::endl;
            // std::cout << "Subtraction Time: " << substractionTime << " nanoseconds" << std::endl;
            // std::cout << "Multiplication Time: " << multiplicationTime << " nanoseconds" << std::endl;
            // std::cout << "Division Time: " << divisionTime << " nanoseconds" << std::endl;
            std::cout << "Reduction Time: " << reductionTime << " nanoseconds" << std::endl;
            std::cout << std::endl;
        }

        double AverageAdditionTime = TotalAdditionTime / 100.0;
        double AverageSubtractionTime = TotalSubtractionTime / 100.0;
        double AverageMultiplicationTime = TotalMultiplicationTime / 100.0;
        double AverageDivisionTime = TotalDivisionTime / 100.0;
        double AverageReductionTime = TotalReductionTime / 100.0;

        // std::cout << "Size " << size << std::endl;
        // std::cout << "Average Addition Time: " << AverageAdditionTime << " nanoseconds" << std::endl;
        // std::cout << "Average Subtraction Time: " << AverageSubtractionTime << " nanoseconds" << std::endl;
        // std::cout << "Average Multiplication Time: " << AverageMultiplicationTime << " nanoseconds" << std::endl;
        // std::cout << "Average Division Time: " << AverageDivisionTime << " nanoseconds" << std::endl;
        std::cout << "Average Reduction Time: " << AverageReductionTime << " nanoseconds" << std::endl;
        std::cout << std::endl;
    }


    // auto AdditionStartTime = std::chrono::high_resolution_clock::now();
    // BigInt resultAdd = bi1 + bi2;
    // auto AdditionEndTime = std::chrono::high_resolution_clock::now();
    // TotalAdditionTime += std::chrono::duration_cast<std::chrono::nanoseconds>(AdditionEndTime - AdditionStartTime).count();

}

int64_t BigInt::msb() const noexcept
{
    auto binaryStr = binaryString();
    std::reverse(binaryStr.begin(), binaryStr.end());

    if(int64_t index = binaryStr.rfind("1"); index != std::string::npos)
        return index;
    
    return -1;
}

std::ostream& operator<<(std::ostream& out, const BigInt& bigInt)
{
    // out << "Number: ";
    // for(std::size_t i = 0; i < bigInt.size(); ++i)
    // {
    //    out << "(2^32)^" << i << " * " << bigInt.m_data.at(i) << " + ";
    // }
    // out << std::endl;

    // out << "Number in hex: ";

    std::stringstream stream;
    stream << std::hex << *(bigInt.m_data.rbegin());
    std::cout << stream.str();

    for(int64_t i = bigInt.size() - 2; i >= 0; --i)
    {
        stream.str(std::string());
        stream << std::hex << bigInt.m_data[i];
        std::cout << std::setfill('0') << std::setw(8) << stream.str();
    }
    std::cout << std::endl;

    return out;
}
