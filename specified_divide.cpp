//
// Created by user on 2022/10/28.
//

#include "fixed_high_prec.hpp"

inline int find_quotient(const FixedInteger &residual, int bias, std::array<FixedInteger,10> &prod) {
    int a = residual.size() - bias, b = prod.at(1).size();
    if (a != b) {
        throw std::runtime_error(std::string ("a != b (") + std::to_string(a) + std::to_string(b));
    }
    int factor;
    for (factor = 1;factor <= 9;++factor) {
        bool flag = false;
        auto &one = residual.val;
        auto &ano = prod.at(factor).val;
        for (auto p=one.cbegin() + bias,pano=ano.cbegin();
             p != one.cend() && pano != ano.cend();++p,++pano) {
            if (*p != *pano) {
                flag =  (*p < *pano);
                break;
            }
        }

//        if (prod.at(factor) > residual)
        if (flag) {
            break;
        }
    }
    factor-=1;
    return factor;
}

inline FixedInteger subtract(const FixedInteger &residual, const FixedInteger &ano, int bias) {

    const int len = residual.val.size();
    FixedInteger re(residual);

    Digit borrow=0;
    for (int i=len-1;i >= bias;--i) {
        int sub = ano.val.at(i - bias) + borrow;
        if (sub == 0)
            continue;

        int diff = residual.val.at(i) - sub;
        borrow = 0;
        if (diff < 0) {
            borrow = 1;
            diff += 10;
        }
        re.val.at(i) = diff;
    }


    return std::move(re);
}