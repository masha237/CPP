#pragma once

#include <iosfwd>
#include <string>
#include <vector>

struct big_integer {
    big_integer();
    big_integer(big_integer const& other);
    big_integer(int32_t a);
    big_integer(uint32_t a);
    big_integer(int64_t a);
    big_integer(uint64_t a);
    explicit big_integer(std::string const& str);
    big_integer(int16_t a);
    big_integer(uint16_t a);
    ~big_integer();

    big_integer& operator=(big_integer const& other);

    big_integer& operator+=(big_integer const& rhs);
    big_integer& operator-=(big_integer const& rhs);
    big_integer& operator*=(big_integer const& rhs);
    big_integer& operator/=(big_integer const& rhs);
    big_integer& operator%=(big_integer const& rhs);

    big_integer& operator&=(big_integer const& rhs);
    big_integer& operator|=(big_integer const& rhs);
    big_integer& operator^=(big_integer const& rhs);

    big_integer& operator<<=(int rhs);
    big_integer& operator>>=(int rhs);

    big_integer operator+() const;
    big_integer operator-() const;
    big_integer operator~() const;

    big_integer& operator++();
    big_integer operator++(int);

    big_integer& operator--();
    big_integer operator--(int);

    big_integer abs() const;
    uint32_t get_digit(size_t ind) const;
    bool is_zero() const;

    friend bool operator==(big_integer const& a, big_integer const& b);
    friend bool operator!=(big_integer const& a, big_integer const& b);
    friend bool operator<(big_integer const& a, big_integer const& b);
    friend bool operator>(big_integer const& a, big_integer const& b);
    friend bool operator<=(big_integer const& a, big_integer const& b);
    friend bool operator>=(big_integer const& a, big_integer const& b);

    friend std::string to_string(big_integer const& a);

private:
    big_integer(std::vector<uint32_t>& number, uint32_t sign);
    void format_number();
    big_integer& make_shift(int rhs, bool b);
    big_integer& bitwise(const big_integer& integer,
                         void (*opration)(uint32_t&, uint32_t));
    static void div_and_mod(big_integer const& x, big_integer const& y,
                            big_integer& div, big_integer& mod, uint32_t s_x,
                            uint32_t s_y);
    void swap(big_integer& integer);
    static uint32_t get_number(const std::string& basicString, size_t i,
                               size_t i1);
    static void short_div(const big_integer& a, const big_integer& b,
                          big_integer& div, big_integer& mul, uint32_t sign_a,
                          uint32_t sign_b);

private:
    uint32_t sign{0};
    std::vector<uint32_t> number;
};

big_integer operator+(big_integer a, big_integer const& b);
big_integer operator-(big_integer a, big_integer const& b);
big_integer operator*(big_integer a, big_integer const& b);
big_integer operator/(big_integer a, big_integer const& b);
big_integer operator%(big_integer a, big_integer const& b);

big_integer operator&(big_integer a, big_integer const& b);
big_integer operator|(big_integer a, big_integer const& b);
big_integer operator^(big_integer a, big_integer const& b);

big_integer operator<<(big_integer a, int b);
big_integer operator>>(big_integer a, int b);

bool operator==(big_integer const& a, big_integer const& b);
bool operator!=(big_integer const& a, big_integer const& b);
bool operator<(big_integer const& a, big_integer const& b);
bool operator>(big_integer const& a, big_integer const& b);
bool operator<=(big_integer const& a, big_integer const& b);
bool operator>=(big_integer const& a, big_integer const& b);

std::string to_string(big_integer const& a);
std::ostream& operator<<(std::ostream& s, big_integer const& a);