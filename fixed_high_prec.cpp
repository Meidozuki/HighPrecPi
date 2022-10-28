//
// Created by user on 2022/10/25.
//

#include "fixed_high_prec.hpp"

#include <cassert>
#include <array>
#include <sstream>
#include <fstream>

// -----
// I/Os
// -----


std::string FixedInteger::string(unsigned trunc, bool drop_zero) const {
    std::stringstream ss;
    bool first0=drop_zero;
    int cnt = 0;
    for (int i = 0; i < size(); ++i) {
        Digit p = val.at(i);
        if (p == 0 && first0)
            continue;

        first0=false;
        ss << static_cast<int>(p);
        if (cnt++ >= trunc) break;
    }
    if (first0) ss << 0;
    return ss.str();
}

void FixedInteger::save_ckpt(std::string path) {
    std::ifstream is(path);
    int prev = 0;
    if (is >> prev) {
        if (prev > size()) {
            return;
        }
    }
    std::ofstream fs(path);
    fs << size() << '\n' << string(-1, false);
    fs.close();
}
bool FixedInteger::load_ckpt(std::string path, int prec) {
    std::string str;
    std::ifstream fs(path);
    int prev = 0;
    if (fs >> prev) {
        fs.ignore();
        fs >> str;
        setValue(str, prec);
        return true;
    }
    else {
        std::cerr << "File doesn't exist." << std::endl;
        return false;
    }

}

// -----
// Compare Operator
// -----

bool FixedInteger::operator<(const FixedInteger &ano) const {
    assert(size() == ano.size());
    for (auto p=val.cbegin(),pano=ano.val.cbegin();
         p != val.cend() && pano != ano.val.cend();++p,++pano) {
        if (*p != *pano) {
            return (*p < *pano);
        }
    }
    return false;
}

bool FixedInteger::operator==(const FixedInteger &ano) const {
    assert(size() == ano.size());
    for (auto p=val.cbegin(),pano=ano.val.cbegin();
         p != val.cend() && pano != ano.val.cend();++p,++pano) {
        if (*p != *pano) {
            return false;
        }
    }

    return true;
}


// -----
// Arithmetic Operator
// -----

FixedInteger FixedInteger::operator+(const FixedInteger &ano) const {
    if (val.size() != ano.val.size()) {
        throw std::runtime_error("FixedInt trying to add 2 int with different size\n");
    }

    const int len = val.size();
    FixedInteger re(len);

    Digit acc=0;
    for (int i=len-1;i >= 0;--i) {
        int sum = acc + val.at(i) + ano.val.at(i);
        if (sum == 0)
            continue;
        acc = sum > 9 ? 1 : 0;
        re.val.at(i) = sum%10;
    }

    return std::move(re);
}

FixedInteger FixedInteger::operator-(const FixedInteger &ano) const {
    if (val.size() != ano.val.size()) {
        throw std::runtime_error("FixedInt trying to subtract 2 int with different size\n");
    }

    const int len = val.size();
    FixedInteger re(*this);

    if (*this < ano) {
        std::cout << "left (" << this->string() << ") will subtract " << ano.string();
        throw std::runtime_error("subtract will get a negative number");
    }


    Digit borrow=0;
    for (int i=len-1;i >= 0;--i) {
        int sub = ano.val.at(i) + borrow;
        if (sub == 0)
            continue;

        int diff = val.at(i) - sub;
        borrow = 0;
        if (diff < 0) {
            borrow = 1;
            diff += 10;
        }
        re.val.at(i) = diff;
    }


    return std::move(re);
}

FixedInteger FixedInteger::operator*(int n) const {
    FixedInteger re(val.size());

    for (int i=0;i < n;++i) {
        re = re + *this;
    }
    return std::move(re);
}

FixedInteger FixedInteger::operator*(const FixedInteger &ano) const {
    if (val.size() != ano.val.size()) {
        throw std::runtime_error("FixedInt trying to multiply 2 int with different size\n");
    }

    std::array<FixedInteger,10> prod;
    prod.at(1) = *this;
    for (int i=2;i < 10;++i) {
        prod.at(i) = (*this * i);
    }

    int len = 0; //len为第一个非0数字前的0数量
    auto &prod9 = prod.at(9).val;
    for (int i=prod9.size()-1;i >= 0;--i) {
        if (prod9.at(i) != 0) break;
        ++len;
    }
    int rev_len = size() - len;

    //重复小数点前的0，需要-1
    FixedInteger re(size()*2 - 1);
    auto &v = ano.val;
    for (int ano_idx=ano.size()-1; ano_idx >= 0; --ano_idx) {
        auto digit = v.at(ano_idx);
        if (digit == 0) continue;

        auto &prodx = prod.at(digit).val;

        Digit acc=0;
        for (int prod_idx=rev_len-1; prod_idx >= 0 || acc; --prod_idx) {
            auto &re_digit = re.val.at(ano_idx + prod_idx);
            int sum = acc + prodx.at(prod_idx) + re_digit;

            acc = sum / 10;
            re_digit = sum%10;
        }

//        std::clog << re.string(-1,false) << '\n';
    }

    auto it = re.val.cend();
    //re为1+x+x位，this位1+x位，去掉x位
    for (int i=1;i < val.size();++i) --it;
    re.val.erase(it,re.val.cend());

    return std::move(re);
}

template<class T>
inline int find_quotient(const T &residual, std::array<T,10> &prod) {
    int factor;
    for (factor = 1;factor <= 9;++factor) {
        if (prod.at(factor) > residual) {
            break;
        }
    }
    factor-=1;
    return factor;
}

FixedInteger FixedInteger::divideOne(long long int dividor, unsigned int precision) {
    if (dividor < 0) {
        throw std::runtime_error("long long int overflow!");
    }


//    int len = 0;
//    for (long long t=dividor;t > 0;t/=10) ++len;
    std::array<long long,10> prod;
    prod.at(0)=0;
    for (int i=1;i < 10;++i) {
        prod.at(i) = (dividor * i);
    }


    FixedInteger re(precision);
    int idx = 0;

    long long int residual = 1;
    while (idx < precision) {
        int factor = find_quotient(residual,prod);

        re.val.at(idx) = factor;
        residual = residual - prod.at(factor);

        ++idx;
        residual *= 10;
        if (residual < 0)
            throw std::runtime_error("long long int overflow!");
    }

    return std::move(re);
}

FixedInteger FixedInteger::divideOne(const FixedInteger &dividor) {
    //分析，当dividor <= 1.1..时，9*div = 9.9..，但残差可能为10.*..
    //当dividor > 1.1时，9*div进位，残差也同位
    //因此需要截断到之前的一个0来模拟整数除法

    if (dividor.size() == dividor.valid_length()) {
        throw std::runtime_error("Dividor > 1, which is not expected during this program.");
    }

    int len;
    std::array<FixedInteger,10> prod;
    {
        FixedInteger trunc(dividor);
        len = trunc.valid_length() - 1;
        auto iter = trunc.val.begin() + len;
        trunc.val.erase(trunc.val.begin(),iter);
        prod.at(0).val.resize(trunc.size());
        prod.at(1) = std::move(trunc);
    }
    for (int i=2;i < 10;++i) {
        prod.at(i) = (prod.at(1) * i);
    }


    FixedInteger re(dividor.size());
    int idx = 0;

    FixedInteger residual(prod.at(1).size());
    residual.val.at(0) = 1;
    residual.val.reserve(residual.size() + dividor.size());
    for (;idx < dividor.size();++idx) {
        int factor = find_quotient(residual,prod);

        re.val.at(idx) = factor;
        residual = residual - prod.at(factor);


        residual.val.emplace_back(0);
        residual.val.erase(residual.val.begin());
    }

    return std::move(re);
}

FixedInteger FixedInteger::operator/(const FixedInteger &ano) const {
    if (val.size() != ano.val.size()) {
        throw std::runtime_error("FixedInt trying to divide 2 int with different size\n");
    }

    int len;
    std::array<FixedInteger,10> prod;
    {
        FixedInteger trunc(ano);
        len = trunc.valid_length() - 1;
        auto iter = trunc.val.begin() + len;
        trunc.val.erase(trunc.val.begin(),iter);
        prod.at(0).val.resize(trunc.size());
        prod.at(1) = std::move(trunc);
    }
    for (int i=2;i < 10;++i) {
        prod.at(i) = (prod.at(1) * i);
    }


    FixedInteger re(size());
    int idx = 0;

    FixedInteger residual;
    int bias = prod.at(1).size();
    residual.val = std::vector<Digit>(val.begin(),val.begin()+bias);

    residual.val.reserve(residual.size() + ano.size());
    for (;idx < ano.size();++idx) {
        int factor = find_quotient(residual,prod);

        re.val.at(idx) = factor;
        residual = residual - prod.at(factor);


        Digit temp = idx+bias >= size() ? 0 : val.at(idx+bias);
        residual.val.emplace_back(temp);
        residual.val.erase(residual.val.begin());
    }

    return std::move(re);
}


FixedInteger FixedInteger::sqrt(std::optional<const FixedInteger *> ref) {
    FixedInteger guess;
    if (ref.has_value()) {
        guess = FixedInteger(*ref.value());
    }
    guess.val.resize(size());

    int prec =   (val.size() - 2);

    int t=0;
    while (true) {
//        FixedInteger &&fix = (*this) * divideOne(guess);
        FixedInteger &&fix = (*this) / guess;
//        std::cout << guess.string() << '\n' << fix.string() << std::endl;
//        _sleep(100);

        int cnt=0;
        for (auto p=guess.val.cbegin(),pano=fix.val.cbegin();
             p != guess.val.cend() && pano != fix.val.cend();++p,++pano,++cnt) {
            if (*p != *pano) {
                break;
            }
        }
        if (cnt >= prec) break;

        guess = (guess + fix).half();
        t++;
    }
    std::cout << t << std::endl;
    return guess;
}
