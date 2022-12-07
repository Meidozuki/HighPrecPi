#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <tuple>
#include <iomanip>
#include <ctime>
#include <cmath>

#include "bigint-deque.hpp"


constexpr int dst_length = 130; //'是C++14特性
constexpr int total_length = dst_length + 1000;

void compare_episilon(BigInteger &result);
BigInteger initialize_big(double x);

inline double square(double x) {
    return x*x;
}

inline BigInteger square(const BigInteger& x, int prec) {
    auto &&sq = BigInteger::multiFloat(x, x);
    //乘法进位问题
    return std::move(sq);
}


std::tuple<double,double,double,double>
    init_iteration() {

    double a0=1, b0=1/sqrt(2), t0=1.0/4, p0=1;
    for (int i=0;i < 4;++i) {
        printf("%.14f %.14f %.14f --- ",a0,b0,t0);
        printf("%.16lf\n",square(a0+b0)/4/t0);
        double a,b,t,p;
        a = (a0+b0) / 2;
        b = sqrt(a0*b0);
        t = t0 - square(a0-a)*p0;
        p = 2*p0;
        a0=a,b0=b,t0=t,p0=p;
    }
    return {a0,b0,t0,p0};
}


int main() {
    auto start = clock();

//    std::cout << "time: " << clock() - start << "ms\n";

    int prec = dst_length/*有效数字位数*/  + 3 /*留出一些空间*/;
    auto trunc = [&prec] (BigInteger &x) {
      x.val.resize(prec);
    };

    //初始化到double最高精度
    BigInteger an,bn,tn,pn;


    {
        auto [a0,b0,t0,p0] = init_iteration();
        std::cout << "a=" << a0 << ", b=" << b0 << ", t=" << t0 \
            << ", p=" << p0 << ", pi=" << (square(a0+b0)/4/t0) << std::endl;
//        an=(initialize_big(a0));
//        bn=(initialize_big(b0));
//        tn=(initialize_big(t0));
//        pn=(initialize_big(p0));
    }
    //先到这
    return 0;

    //初始化到double精度会偏离
    prec=20;
    an.setValue("1");
    trunc(an);
    tn.setValue("025");
    trunc(tn);
    pn.setValue("1");//p不用trunc
    {
        BigInteger&& t=BigInteger();
        t.setValue("2");
        bn = BigInteger::divideOne(t, prec);
    }
    bn=bn.sqrtPointR();
    trunc(bn);




    auto pi = [&an,&bn,&tn] (int prec) {
      //divide1目前只能处理 <1的数，先当做去除小数点，然后再移动小数点
      auto &&four = tn*4;
      //fourth是1.0/0.9*，会>=1
      auto &&fourth = BigInteger::divideOne(four, prec+four.length()-1);
      fourth.strip();

      //an+bn > 1
      auto &&sq = square(an+bn,prec);

      std::cout << "pi: ";
      (sq * fourth).read(std::min(prec,50));
      std::cout << std::endl;
    };

    std::cout << "check:\n";
    an.read(20); std::cout << ' ';
    bn.read(20); std::cout << ' ';
    tn.read(20); std::cout << ' ';
    pn.read(20); std::cout << std::endl;
//    std::cout << '\n' << pn << std::endl;
    std::cout << "check end" << std::endl;

    pi(prec);

    int t=1;

    for (;t < dst_length && prec > 0;t=t<<1,prec=t+100) {
        //旧的已经能求出Pi，准备用新的prec
        trunc(an),trunc(bn),trunc(tn);

        BigInteger a,b,t;

        a = (an+bn).half();
        //half保持长度
//        trunc(a);

        //乘法没有解决开头的0的问题，进位了
        auto &&mul = an*bn;
        mul.val.push_front(0);
        b = mul.sqrtPointR();
        trunc(b);

        //算术平均>=几何平均，所以an>a_n+1>bn
        auto &&sub = square(an-a,prec);
        sub = sub * pn;
//        sub.val.push_front(0);
        trunc(sub);
        sub.read(20);std::cout << std::endl;
        tn = tn - sub;
        trunc(tn);

        pn = pn * 2;
        an = std::move(a);
        bn = std::move(b);

        an.read(20); std::cout << ' ';
        bn.read(20); std::cout << ' ';
        tn.read(20); std::cout << ' ';
        pn.read(); std::cout << '\n';

//        prec-=3;
        pi(prec);

    }







    return 0;
}


BigInteger initialize_big(double x) {
    std::string str;
    std::stringstream ss;
    ss << std::setprecision(18) << x;
    ss >> str;
    str.erase(1,1);

    BigInteger t;
    t.setValue(str);
    return std::move(t);
}


//    BigInteger result;
//    result.setValue("0");
//    BigInteger P;
//    P.setValue("1");
//    for (int i=0;i < dst_length;++i) {
//        // 莱布尼兹公式，arctan pi/4
//        //BigInteger item = BigInteger::divideOne(2*i + 1,total_length);
//        // BBP 约1.2倍精度
//        BigInteger poly = BigInteger::divideOne(8 * i + 1, total_length,4)
//                        - BigInteger::divideOne(8 * i + 4, total_length,2)
//                        - BigInteger::divideOne(8 * i + 5, total_length)
//                        - BigInteger::divideOne(8 * i + 6, total_length);
//        result += poly * BigInteger::divideOne(P, total_length);
//
//         P=P*16;
//
////        item.read(20);
//        if (i%32 == 0) std::cout << i << std::endl;
//    }

void compare_episilon(BigInteger &result) {


    auto oldcout = std::cout.rdbuf();
    {
        std::ofstream fs("pi.txt");
        std::cout.rdbuf(fs.rdbuf());
        result.read();
    }
    std::cout.rdbuf(oldcout);

    std::ifstream pred("pi.txt"), gt("../pi_10million.txt");
    int cnt;
    for (cnt = 0;pred.get() == gt.get();cnt++) ;
    std::cout << "cnt: " << cnt << std::endl;
}