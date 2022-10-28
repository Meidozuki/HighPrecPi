#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <tuple>
#include <iomanip>
#include <ctime>
#include <cmath>

#include "fixed_high_prec.hpp"

using namespace std::literals;

constexpr int dst_length = 10'000; //'是C++14特性
constexpr int total_length = dst_length + 100;

void compare_episilon(FixedInteger &result);
//FixedInteger initialize_big(double x);

template <class T>
inline T square(const T& x) {
    return std::move(x*x);
}


std::tuple<double,double,double,double>
    init_iteration() {

    double a0=1, b0=1/sqrt(2), t0=1.0/4, p0=1;
    for (int i=0;i < 5;++i) {
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


int main() {
    int prec = 10/*有效数字位数*/  + 3 /*留出一些空间*/;


    {
        //初始化到double最高精度
        auto [a0,b0,t0,p0] = init_iteration();
        std::cout << "a=" << a0 << ", b=" << b0 << ", t=" << t0 \
            << ", p=" << p0 << ", pi=" << (square(a0+b0)/4/t0) << std::endl;
    }

    prec=total_length;
    if (prec%2 == 0) prec+=1;

    //-----
    //初始化
    FixedInteger an,bn,tn;
    int pn = 1;

    an.val.resize(prec); //a0=1
    an.val.at(0) = 1;
    tn.setValue("025"); //t0=1.0/4
    tn.val.resize(prec);
    bn.setValue("05"); //b0=1/sqrt(2)
    bn.val.resize(prec);
    {
        FixedInteger ref;
        ref.setValue("0707106781186547524");
        ref.val.resize(prec);
        auto start = clock();
        bn= bn.sqrt(&ref);
        printf("b0 = 1/sqrt(2) takes %.3lf s\n",(clock() - start)/1000.0);
    }

    //初始化结束
    //-----


    auto pi = [&an,&bn,&tn] (int prec) {
      //divide1目前只能处理 <1的数，先当做去除小数点，然后再移动小数点
      //an+bn > 1
      auto &&sq = square((an+bn).half());

      std::cout << "pi: " << (sq * FixedInteger::divideOne(tn)).string(std::min(prec,50));
      std::cout << std::endl;
    };


    auto start = clock();
    std::cout << "check:\n";
    std::cout << an.string(15) << ' ' << bn.string(15, false) << ' ' << tn.string(15, false) << ' ' << pn << " --- ";
    pi(20);
    std::cout << "check end" << std::endl;


    for (int i=1;pn < dst_length;++i) {
        FixedInteger a = (an+bn).half();

        FixedInteger b;
        FixedInteger *ref = &a;
        auto pRef = std::make_unique<FixedInteger>();
        auto ckpt_path = "../b"s + std::to_string(i) + ".txt"s;
        if (i < 10){
            if (pRef->load_ckpt(ckpt_path,prec)) {
                pRef->val.resize(prec);
                ref = pRef.get();
            }
            b = (an * bn).sqrt(ref);
            b.save_ckpt(ckpt_path);
        }
        else {
            b = (an * bn).sqrt(ref);
        }

        //算术平均>=几何平均，所以an>a_n+1>bn
        tn = tn - (square(an-a) * pn);

        pn = pn * 2;
        an = std::move(a);
        bn = std::move(b);

//        std::cout << an.string(15, false) << ' ' << bn.string(15, false) << ' ' << tn.string(15, false) << ' ' << pn << "\n";
//        pi(std::min(prec,50));

        printf("pRef = %d, time: %.3lf s\n",pn,(clock() - start)/1000.0);
    }


    auto &&sq = square((an+bn).half());
    auto re = (sq * FixedInteger::divideOne(tn));
    compare_episilon(re);

    return 0;
}


FixedInteger initialize_big(double x) {
    std::string str;
    std::stringstream ss;
    ss << std::setprecision(18) << x;
    ss >> str;
    str.erase(1,1);

    FixedInteger t;
    t.setValue(str);
    return std::move(t);
}


void compare_episilon(FixedInteger &result) {


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