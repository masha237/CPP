#include "big_integer.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstring>
#include <functional>
#include <iostream>
#include <ostream>
#include <stdexcept>

constexpr uint32_t CAPACITY = 32;
constexpr uint32_t LOG_CAPACITY = 5;
bool big_integer::is_zero() const {
    return sign == 0 && number.empty();
}

void big_integer::format_number() {
    while (!number.empty() && ((number.back() == 0 && sign == 0) ||
                               (number.back() == UINT32_MAX && sign != 0))) {
        number.pop_back();
    }
}

big_integer::big_integer() = default;

big_integer::big_integer(big_integer const& other) = default;

big_integer::big_integer(std::vector<uint32_t>& number, uint32_t sign)
    : sign(sign), number(number) {
    format_number();
}

big_integer::big_integer(int32_t a) : big_integer(static_cast<int64_t>(a)) {}
big_integer::big_integer(int16_t a) : big_integer(static_cast<int64_t>(a)) {}

big_integer::big_integer(int64_t a)
    : sign(a < 0 ? UINT32_MAX : 0),
      number({static_cast<uint32_t>(a & UINT32_MAX),
              static_cast<uint32_t>(a >> CAPACITY)}) {
    format_number();
}

big_integer::big_integer(uint32_t a) : big_integer(static_cast<uint64_t>(a)) {}
big_integer::big_integer(uint16_t a) : big_integer(static_cast<uint64_t>(a)) {}
big_integer::big_integer(uint64_t a)
    : number({static_cast<uint32_t>(a & UINT32_MAX),
              static_cast<uint32_t>(a >> CAPACITY)}) {
    format_number();
}

constexpr std::array<uint32_t, 10> DEC{
    1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};

big_integer::big_integer(std::string const& str) {
    *this = 0;
    if (str.empty()) {
        return;
    }
    size_t i = 0;
    bool minus = false;
    if (str[0] == '+') {
        i++;
    }
    if (str[0] == '-') {
        minus = true;
        i++;
    }
    for (; i + 9 <= str.size(); i += 9) {
        *this *= DEC[9];
        *this += get_number(str, i, 9);
    }

    *this *= DEC[str.size() - i];
    *this += get_number(str, i, str.size() - i);

    if (minus && !is_zero()) {
        *this = -*this;
    }
}

big_integer::~big_integer() = default;

big_integer& big_integer::operator=(big_integer const& other) {
    big_integer(other).swap(*this);
    return *this;
}

big_integer& big_integer::operator+=(big_integer const& rhs) {
    size_t n = std::max(number.size(), rhs.number.size()) + 2;
    number.resize(n, sign);
    uint32_t carry = 0;
    for (size_t i = 0; i < rhs.number.size(); i++) {
        uint64_t sum = static_cast<uint64_t>(carry) + number[i] + rhs.number[i];
        number[i] = (sum & UINT32_MAX);
        carry = (sum >> CAPACITY);
    }
    size_t r = rhs.sign;
    for (size_t i = rhs.number.size(); i < number.size(); i++) {
        uint64_t sum = static_cast<uint64_t>(carry) + number[i] + r;
        number[i] = (sum & UINT32_MAX);
        carry = (sum >> CAPACITY);
    }
    sign = ((number.back() >> (CAPACITY - 1)) != 0 ? UINT32_MAX : 0);
    format_number();
    return *this;
}

big_integer& big_integer::operator-=(big_integer const& rhs) {
    return *this += (-rhs);
}

big_integer& big_integer::operator*=(big_integer const& rhs) {
    uint32_t minus = sign ^ rhs.sign;
    big_integer a = abs();
    big_integer b = rhs.abs();
    if (a == 0 || b == 0) {
        return *this = 0;
    }

    std::vector<uint32_t> cur(rhs.number.size() + number.size() + 1);
    uint64_t carry = 0, c = 0, time = 0;

    for (size_t i = 0; i < rhs.number.size(); i++) {
        carry = 0;
        for (size_t j = 0; j < number.size(); j++) {
            c = static_cast<uint64_t>(a.number[j]) * b.number[i];
            c += carry;
            time = c + cur[i + j];
            cur[i + j] = (time & UINT32_MAX);
            carry = (time >> CAPACITY);
        }
        cur[i + number.size()] += carry & UINT32_MAX;
    }
    *this = big_integer(cur, false);

    if (minus) {
        *this = -*this;
    }
    format_number();
    return *this;
}

void big_integer::div_and_mod(big_integer const& a, big_integer const& b,
                              big_integer& div, big_integer& mod, uint32_t sign_a,
                              uint32_t sign_b) {
    if (b.number.size() == 1) {
        short_div(a, b, div, mod, sign_a, sign_b);
        return;
    }
    uint32_t minus = sign_a ^ sign_b;

    std::vector<uint32_t> ans;

    mod = 0;

    for (size_t ind = a.number.size(); ind != 0; ind--) {
        mod = (mod << CAPACITY) + a.number[ind - 1];
        if (mod < b) {
            ans.push_back(0);
            continue;
        }
        int new_bit_ans = 0;
        big_integer sh = b << (CAPACITY);
        for (int L = CAPACITY - 1; L >= 0; L--) {
            sh >>= 1;
            if (sh <= mod) {
                mod -= sh;
                new_bit_ans += (1 << L);
            }
        }
        ans.push_back(new_bit_ans);
    }
    std::reverse(ans.begin(), ans.end());
    div = big_integer(ans, false);
    div.format_number();
    mod.format_number();
    if (minus) {
        div = -div;
    }
    if (sign_a) {
        mod = -mod;
    }
}

big_integer& big_integer::operator/=(big_integer const& rhs) {
    big_integer tr;
    div_and_mod(abs(), rhs.abs(), *this, tr, sign, rhs.sign);
    return *this;
}

big_integer& big_integer::operator%=(big_integer const& rhs) {
    big_integer tr;
    div_and_mod(abs(), rhs.abs(), tr, *this, sign, rhs.sign);
    return *this;
}

big_integer& big_integer::operator&=(big_integer const& rhs) {
    return bitwise(rhs, [](uint32_t& x, uint32_t y) { x &= y; });
}

big_integer& big_integer::operator|=(big_integer const& rhs) {
    return bitwise(rhs, [](uint32_t& x, uint32_t y) { x |= y; });
}

big_integer& big_integer::operator^=(big_integer const& rhs) {
    return bitwise(rhs, [](uint32_t& x, uint32_t y) { x ^= y; });
}

big_integer& big_integer::operator<<=(int rhs) {
    return make_shift(rhs, false);
}

big_integer& big_integer::operator>>=(int rhs) {
    return make_shift(rhs, true);
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    if (is_zero()) {
        return *this;
    }
    size_t n = number.size() + 1;
    std::vector<uint32_t> ans(n, sign);
    uint32_t carry = 1;
    for (size_t i = 0; i < number.size(); i++) {
        uint64_t sum = static_cast<uint64_t>(carry) + (number[i] ^ UINT32_MAX);
        ans[i] = (sum & UINT32_MAX);
        carry = (sum >> CAPACITY);
    }
    if (carry) {
        ans.back() = carry + (ans.back() ^ UINT32_MAX);
    } else {
        ans.pop_back();
    }
    return big_integer(ans, ~sign);
}

big_integer big_integer::operator~() const {
    big_integer cur = *this;
    for (size_t i = 0; i < number.size(); i++) {
        cur.number[i] ^= UINT32_MAX;
    }

    cur.sign = ~sign;
    cur.format_number();
    return cur;
}

big_integer& big_integer::operator++() {
    return (*this += 1);
}

big_integer big_integer::operator++(int) {
    big_integer cur = *this;
    *this += 1;
    return cur;
}

big_integer& big_integer::operator--() {
    return (*this -= 1);
}

big_integer big_integer::operator--(int) {
    big_integer cur = *this;
    *this -= 1;
    return cur;
}

big_integer operator+(big_integer a, big_integer const& b) {
    return a += b;
}

big_integer operator-(big_integer a, big_integer const& b) {
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const& b) {
    return a *= b;
}

big_integer operator/(big_integer a, big_integer const& b) {
    return a /= b;
}

big_integer operator%(big_integer a, big_integer const& b) {
    return a %= b;
}

big_integer operator&(big_integer a, big_integer const& b) {
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const& b) {
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const& b) {
    return a ^= b;
}

big_integer operator<<(big_integer a, int b) {
    return a <<= b;
}

big_integer operator>>(big_integer a, int b) {
    return a >>= b;
}

bool operator==(big_integer const& a, big_integer const& b) {
    return (a.sign == b.sign && a.number == b.number);
}

bool operator!=(big_integer const& a, big_integer const& b) {
    return !(a == b);
}

bool operator<(big_integer const& a, big_integer const& b) {
    if (a.sign != b.sign) {
        return a.sign != 0 && b.sign == 0;
    }
    if (a.sign != 0) {
        if (a.number.size() != b.number.size()) {
            return a.number.size() > b.number.size();
        }
    } else {
        if (a.number.size() != b.number.size()) {
            return a.number.size() < b.number.size();
        }
    }

    for (size_t i = a.number.size(); i != 0; i--) {
        if (a.number[i - 1] != b.number[i - 1]) {
            return a.number[i - 1] < b.number[i - 1];
        }
    }
    return false;
}

bool operator>(big_integer const& a, big_integer const& b) {
    return b < a;
}

bool operator<=(big_integer const& a, big_integer const& b) {
    return !(a > b);
}

bool operator>=(big_integer const& a, big_integer const& b) {
    return !(a < b);
}

big_integer big_integer::abs() const {
    if (sign != 0) {
        return -*this;
    } else {
        return *this;
    }
}

uint32_t big_integer::get_digit(size_t ind) const {
    if (ind < number.size()) {
        return number[ind];
    }
    return sign;
}

std::string to_string(big_integer const& a) {
    std::string ans;
    big_integer cur = a.abs();
    big_integer m;
    while (cur != 0) {
        big_integer::div_and_mod(cur.abs(), DEC[9], cur, m, false, false);
        std::string next =
            std::to_string((!m.number.empty() ? m.number[0] : 0));
        if (next.size() != 9 && cur != 0) {
            next = std::string(9 - next.size(), '0').append(next);
        }
        ans = next.append(ans);
    }
    if (ans.empty()) {
        ans = "0";
    }

    if (a.sign != 0) {
        ans = "-" + ans;
    }

    return ans;
}

void big_integer::swap(big_integer& integer) {
    std::swap(sign, integer.sign);
    std::swap(number, integer.number);
}
uint32_t big_integer::get_number(const std::string& basicString, size_t i,
                                 size_t i1) {
    uint32_t ans = 0;
    for (size_t k = i; k < i1 + i; k++) {
        ans = ans * DEC[1] + (basicString[k] - '0');
    }
    return ans;
}
void big_integer::short_div(const big_integer& a, const big_integer& b,
                            big_integer& div, big_integer& mod, uint32_t sign_a,
                            uint32_t sign_b) {
    uint32_t minus = sign_a ^ sign_b;

    std::vector<uint32_t> ans;

    mod = 0;
    uint32_t d = b.number[0];
    for (size_t ind = a.number.size(); ind != 0; ind--) {
        mod = (mod << CAPACITY) + a.number[ind - 1];
        if (mod < d) {
            ans.push_back(0);
            continue;
        }
        if (mod.number.size() == 1) {
            ans.push_back(mod.number[0] / d);
            mod.number[0] %= d;
        } else {
            uint64_t cur = (static_cast<uint64_t>(mod.number[1]) << CAPACITY) +
                           mod.number[0];
            ans.push_back(cur / d);
            mod.number[0] = cur % d;
        }
    }
    std::reverse(ans.begin(), ans.end());
    div = big_integer(ans, false);
    div.format_number();
    mod.format_number();
    if (minus) {
        div = -div;
    }
    if (sign_a != 0) {
        mod = -mod;
    }
}
big_integer& big_integer::bitwise(const big_integer& rhs,
                                  void (*opration)(uint32_t&, uint32_t)) {
    size_t n = std::max(number.size(), rhs.number.size());
    number.resize(n, sign);
    for (size_t i = 0; i < rhs.number.size(); i++) {
        opration(number[i], rhs.number[i]);
    }
    size_t si = rhs.sign;
    for (size_t i = rhs.number.size(); i < number.size(); i++) {
        opration(number[i], si);
    }
    opration(sign, rhs.sign);
    format_number();
    return *this;
}
big_integer& big_integer::make_shift(int rhs, bool right) {
    if (rhs == 0) {
        return *this;
    }
    size_t shift_div = rhs >> LOG_CAPACITY;
    size_t shift_mod = rhs & (CAPACITY - 1);
    size_t n = number.size() + (right ? -1 : 1) * shift_div + 1;
    number.resize(std::max(number.size(), n), sign);
    if (right) {
        n--;
        for (size_t i = 0; i < n; i++) {
            uint64_t x =
                static_cast<uint64_t>(number[i + shift_div]) >> shift_mod;
            uint64_t y = static_cast<uint64_t>(get_digit(i + shift_div + 1))
                      << (CAPACITY - shift_mod);
            number[i] = ((x | y) & UINT32_MAX);
        }
    } else {
        for (size_t i = n - 1; i > shift_div; i--) {
            uint64_t x = static_cast<uint64_t>(number[i - shift_div])
                      << shift_mod;
            uint64_t y = static_cast<uint64_t>(number[i - shift_div - 1]) >>
                         (CAPACITY - shift_mod);
            number[i] = ((x | y) & UINT32_MAX);
        }
        number[shift_div] = (number[0] << shift_mod) & UINT32_MAX;
    }
    for (size_t i = n; i < number.size(); i++) {
        number[i] = sign;
    }
    if (!right) {
        for (size_t i = shift_div; i != 0; i--) {
            number[i - 1] = 0;
        }
    }
    format_number();
    return *this;
}

std::ostream& operator<<(std::ostream& s, big_integer const& a) {
    return s << to_string(a);
}
