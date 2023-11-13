#include <string>
#include <vector>
#include <string_view>

#pragma once

class BigInt 
{
public:
    explicit BigInt();
    explicit BigInt(const std::string& bigNumber);
    explicit BigInt(const uint_fast64_t number);
    BigInt(const BigInt& bigNumber) = default;
    BigInt(BigInt&& bigNumber) noexcept = default;
    ~BigInt() = default;
    
    std::size_t size() const noexcept;

    BigInt operator+(BigInt& rhs);
    BigInt operator-(BigInt& rhs);
    BigInt operator*(BigInt& rhs);
    BigInt operator/(const BigInt& rhs);

    BigInt& operator=(const BigInt& rhs) = default;
    BigInt& operator=(BigInt&& rhs) noexcept
    {
        if(this != &rhs)  // check for self-assignment
        {
            m_data = std::move(rhs.m_data);  // use std::move to transfer ownership
            m_countOfPartsInNumber = rhs.m_countOfPartsInNumber;
        }

        return *this;   
    }

    bool operator >  (const BigInt& rhs) const;
    bool operator <  (const BigInt& rhs) const;
    bool operator >= (const BigInt& rhs) const;
    bool operator <= (const BigInt& rhs) const;
    bool operator == (const BigInt& rhs) const;
    bool operator != (const BigInt& rhs) const;

    friend std::ostream& operator <<(std::ostream& out, const BigInt& bigInt);

private:
    /* functions */
    std::vector<std::string> splitNumberToParts(const std::string_view& str) const noexcept;
    BigInt longMulOneDigit(const BigInt& lhs, const uint_fast64_t number) const noexcept;
    BigInt longShiftDigitsToHigh(const BigInt& number, const uint_fast64_t widthShift) const noexcept;
    BigInt longShiftBitsToHigh(const BigInt& number, int_fast64_t widthShift) const noexcept;

    std::size_t bitLength() const noexcept;
    void normalize() noexcept;

    /* magic constants */
    static constexpr std::size_t digitsCountInNumber = 8; // number system divide to the number of bits in one hex ( 64 / 4 )
    static constexpr std::size_t bitsCount = 32;

    /* members */
    std::vector<uint_fast64_t> m_data;
    std::size_t m_countOfPartsInNumber;
};