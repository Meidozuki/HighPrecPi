//
// Created by user on 2022/10/25.
//

#ifndef DS_PI__FIXED_HIGH_PREC_HPP_
#define DS_PI__FIXED_HIGH_PREC_HPP_

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cinttypes>
#include <memory>
#include <optional>

using Digit = uint8_t;

//FixedInteger需要指定长度，并且将最低位存储在idx0
struct FixedInteger {
    std::vector<Digit> val;

    FixedInteger() = default;
    explicit FixedInteger(int len): FixedInteger() { val.resize(len); }
    FixedInteger(const FixedInteger &) = default;
    FixedInteger &operator=(const FixedInteger &) = default;
    FixedInteger(FixedInteger &&ano) noexcept = default;
    FixedInteger &operator=(FixedInteger &&ano) noexcept = default;

    inline int size() const { return val.size(); }

    void setValue(const std::string &s,int len = -1) {
        if (len < 0)
            len = s.length();
        val.clear();
        val.resize(len);
        int i=0;
        for (auto p = s.begin();p != s.end() && i < len;++p) {
            auto t = *p - '0';
            if (t >= 0 && t <= 9) {
                val.at(i++) = t;
            } else {
                throw std::runtime_error("0-9 expected");
            }
        }
    }

    static FixedInteger fromInt(long long n,int len) {
        FixedInteger t(len);
        for (int i=len-1;n && i >= 0;--i) {
            t.val.at(i) = n % 10;
            n /= 10;
        }
        return std::move(t);
    }

    std::string string(unsigned trunc = -1,bool drop_zero=true) const;
    void read() { std::cout << string(); }
    void save_ckpt(std::string path);
    bool load_ckpt(std::string path, int prec);

    int valid_length() const {
        int i;
        for (i = 0; i < size(); ++i) {
            if (val.at(i) != 0) break;
        }
        return i;
    }


    bool operator<(const FixedInteger &ano) const;
    bool operator==(const FixedInteger &ano) const;
    bool operator<=(const FixedInteger &ano) const {
        return *this == ano || *this < ano;
    }
    bool operator>(const FixedInteger &ano) const {
        return !(this->operator<=(ano));
    }

    FixedInteger operator+(const FixedInteger &ano) const;
    FixedInteger operator-(const FixedInteger &ano) const;

    FixedInteger operator*(int n) const;
    FixedInteger operator*(const FixedInteger &ano) const;
    FixedInteger operator/(const FixedInteger &ano) const;

    static FixedInteger divideOne(long long int dividor, unsigned int precision);
    static FixedInteger divideOne(const FixedInteger &dividor);

    FixedInteger sqrt(std::optional<const FixedInteger*> ref = std::nullopt);
    FixedInteger &half(){
        int s = 0;
        for (auto &it : val) {
            s = s*10 + it;

            it = s/2;
            s = s&1;
        }
        return *this;
    }

};
#endif //DS_PI__FIXED_HIGH_PREC_HPP_
