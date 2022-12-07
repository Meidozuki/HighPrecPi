# C++求高精度pi（1）

## 前言

（之后再写）

## BBP公式

由arctan 1展开得到的莱布尼茨级数是一个交错级数，并且条件收敛而不绝对收敛，这注定了莱布尼兹级数方法会非常低效

而BBP公式

$$
\sum_{k=0}^{\infty}[{1 \over {16^k}} (\frac 4 {8k+1} - \frac 2 {8k+4}- \frac 1 {8k+5}- \frac 1 {8k+6})]
$$

被证明是线性收敛的，每计算一项可以得到约1.2个有效数字

使用BBP公式计算$\pi$大幅提高了速度，在笔者的电脑上30分钟可以算到5k位

选择BBP的另一个原因是，它不像拉马努金公式那样，需要开根号，写起来相对容易。加减实现比较容易，乘法如果用deque实现可以直接调用加法，除法出于简易性可以只写一个求倒数的函数，然后和另一个数相乘

> BBP需要的运算符：< == + - * /
> 
> 莱布尼兹级数需要的运算符：< == + - /  *（为了求倒数）

也就是说，如果尝试过了莱布尼兹级数，那么几乎不需要更改就能使用BBP公式来求$\pi$

### 实现

最初为了写通用大数算法用了deque来存储数据，后来为了加速改用了vector来存储数据。两者在处理乘法时区别较大，deque写起来更方便。

> BBP主体

```cpp
void BBP_iter(unsigned prec) {

    FixedInteger result(prec);
    long long int P=1; //long long为2^64，每次2^4，最多撑16次
    for (int i=0;i < 15;++i) {
        // 莱布尼兹公式，arctan pi/4
        //FixedInteger item = FixedInteger::divideOne(2*i + 1,total_length);
        // BBP 约1.2倍精度
        FixedInteger poly = FixedInteger::divideOne(8 * i + 1, prec)*4
                        - FixedInteger::divideOne(8 * i + 4, prec)*2
                        - FixedInteger::divideOne(8 * i + 5, prec)
                        - FixedInteger::divideOne(8 * i + 6, prec);
        result = result + poly * FixedInteger::divideOne(P, prec);

        P=P*16;

        std::cout << result.string() << std::endl;
    }
}
```

> 大数数据结构

```cpp
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

    static FixedInteger divideOne(long long int dividor, unsigned int precision);
    static FixedInteger divideOne(const FixedInteger &dividor);
```

> 函数实现

```cpp
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
    }

    auto it = re.val.cend();
    //re为1+x+x位，this位1+x位，去掉x位
    for (int i=1;i < val.size();++i) --it;
    re.val.erase(it,re.val.cend());

    return std::move(re);
}

inline int find_quotient(long long residual, std::array<long long,10> &prod) {
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
```

