#include <string>
#include <vector>
#include <string_view>

#pragma once

class BigInt 
{
public:
    explicit BigInt();
    explicit BigInt(const std::string& bigNumber);
    BigInt(const uint_fast64_t number);
    BigInt(const BigInt& bigNumber) = default;
    BigInt(BigInt&& bigNumber) noexcept = default;
    ~BigInt() = default;
    
    std::size_t size() const noexcept;
    std::size_t digitsCount() const noexcept;

    BigInt square() const noexcept;
    BigInt longPower(const BigInt& power) noexcept;

    int64_t msb() const noexcept;

    BigInt operator +(const BigInt& rhs) const;
    BigInt operator -(const BigInt& rhs) const;
    BigInt operator *(const BigInt& rhs) const;
    BigInt operator /(const BigInt& rhs) const;
    BigInt operator %(const BigInt& rhs) const;

    BigInt& operator =(const BigInt& rhs) = default;
    BigInt& operator =(BigInt&& rhs) noexcept = default;

    bool operator > (const BigInt& rhs) const;
    bool operator < (const BigInt& rhs) const;
    bool operator >=(const BigInt& rhs) const;
    bool operator <=(const BigInt& rhs) const;
    bool operator ==(const BigInt& rhs) const;
    bool operator !=(const BigInt& rhs) const;

    friend std::ostream& operator <<(std::ostream& out, const BigInt& bigInt);

    BigInt gcd(const BigInt& rhs) const;
    BigInt lcm(const BigInt& rhs) const;

    std::string binaryString() const noexcept;
    BigInt randomNumber(const int64_t digitsCount) noexcept;

    BigInt barretReduction(const BigInt& module, const BigInt& mu);

    static BigInt evaluateMu(const BigInt& module) noexcept;
    void evaluateTime() const noexcept;

private:
    /* functions */
    std::vector<std::string> splitNumberToParts(const std::string_view& str) const noexcept;
    BigInt longMulOneDigit(const BigInt& lhs, const uint_fast64_t number) const noexcept;
    BigInt longShiftDigitsToHigh(const BigInt& number, const uint_fast64_t widthShift) const noexcept;
    BigInt longShiftBitsToHigh(const BigInt& number, int_fast64_t widthShift) const noexcept;
    BigInt longShiftBitsToDown(const BigInt& number, int_fast64_t widthShift) const noexcept;

    std::pair<BigInt, BigInt> longDivMod(const BigInt& rhs) const noexcept;


    std::size_t bitLength() const noexcept;
    void normalize() noexcept;
    std::string normalize(const std::string& str) const noexcept;


    /* magic constants */
    static constexpr std::size_t digitsCountInNumber = 8; // number system divide to the number of bits in one hex ( 64 / 4 )
    static constexpr std::size_t bitsCount = 32;

    /* members */
    std::vector<uint_fast64_t> m_data;
    std::size_t m_countOfPartsInNumber;
};