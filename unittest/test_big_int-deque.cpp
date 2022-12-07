//
// Created by user on 2022/10/14.
//

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <string>

#include "../bigint-deque.cpp"

class RedirectedCoutFixture : public ::testing::Test {
public:
    std::stringstream ss;
    std::basic_streambuf<char> *oldcout;

    virtual void SetUp() {
        oldcout = std::cout.rdbuf();
        std::cout.rdbuf(ss.rdbuf());
    }

    virtual void TearDown() {}
};

using BigIntTest = RedirectedCoutFixture;
using BigIntAdd = RedirectedCoutFixture;
using BigIntDiv = RedirectedCoutFixture;
using BigIntSqrt = RedirectedCoutFixture;

TEST_F(BigIntAdd,simple) {
//    std::stringstream ss;
//    std::cout.rdbuf(ss.rdbuf());
    BigInteger a;
    a.setValue("123");
    a.read();
    EXPECT_STREQ(ss.str().c_str(),"123");
}

TEST_F(BigIntAdd,simple_add) {
    BigInteger a, b;
    a.setValue("123");
    b.setValue("555");
    (a+b).read();
    EXPECT_STREQ(ss.str().c_str(),"678");
}

TEST_F(BigIntAdd,carry_add_same_len) {
    BigInteger a, b;
    a.setValue("129");
    b.setValue("333");
    (a+b).read();
    EXPECT_STREQ(ss.str().c_str(),"462");
}

TEST_F(BigIntAdd,carry_add_lengthen) {
    BigInteger a, b;
    a.setValue("456");
    b.setValue("700");
    (a+b).read();
    EXPECT_STREQ(ss.str().c_str(),"1156");
}

TEST_F(BigIntAdd,carry_add_diff_len1) {
    BigInteger a, b;
    a.setValue("4560");
    b.setValue("700");
    (a+b).read();
    EXPECT_STREQ(ss.str().c_str(),"5260");
}

TEST_F(BigIntAdd,carry_add_diff_len1_2) {
    BigInteger a, b;
    a.setValue("9760");
    b.setValue("700");
    (a+b).read();
    EXPECT_STREQ(ss.str().c_str(),"10460");
}

TEST_F(BigIntAdd,add0_diff_len9) {
    BigInteger a, b;
    a.setValue(std::string(9,'9'));
    b.setValue("0");
    (a+b).read();
    EXPECT_STREQ(ss.str().c_str(),"999999999");
}

TEST_F(BigIntAdd,carry_add_gt_1e9) {
    BigInteger a, b;
    a.setValue(std::string(10,'9'));
    b.setValue(std::string(10,'1'));
    (a+b).read();
    EXPECT_STREQ(ss.str().c_str(),"11111""111110");
}

TEST_F(BigIntAdd,carry_add_long_carry) {
    BigInteger a, b;
    a.setValue("9999");
    b.setValue("1");
    (a+b).read();
    EXPECT_STREQ(ss.str().c_str(),"10000");
}

TEST_F(BigIntAdd,add_assign) {
    BigInteger a, b;
    a.setValue("9760");
    b.setValue("700");
    a+=b;
    a.read();
    EXPECT_STREQ(ss.str().c_str(),"10460");
}

TEST_F(BigIntAdd,add_assign_carry) {
    BigInteger a, b;
    a.setValue("9999");
    b.setValue("1");
    a+=b;
    a.read();
    EXPECT_STREQ(ss.str().c_str(),"10000");
}

TEST_F(BigIntAdd,add_time_with_1e4) {
    int prec=10000;
    BigInteger a, b;
    std::string s(prec,'1');
    s=s+"0";
    a.setValue(std::string(prec,'9'));
    b.setValue(std::string(prec,'1'));
    (a+b).read();
    EXPECT_STREQ(ss.str().c_str(),s.c_str());
}

TEST_F(BigIntAdd,add_time_with_1e6) {
    int prec=1000000;
    BigInteger a, b;
    std::string s(prec,'1');
    s=s+"0";
    a.setValue(std::string(prec,'9'));
    b.setValue(std::string(prec,'1'));
    (a+b).read();
    EXPECT_STREQ(ss.str().c_str(),s.c_str());
}

TEST_F(BigIntAdd,add_time_with_1e7) {
    int prec=10000000;
    BigInteger a, b;
    std::string s(prec,'1');
    s=s+"0";
    a.setValue(std::string(prec,'9'));
    b.setValue(std::string(prec,'1'));
    (a+b).read();
    EXPECT_STREQ(ss.str().c_str(),s.c_str());
}

TEST_F(BigIntAdd,add_assign_time_with_1e7) {
    int prec=10000000;
    BigInteger a, b;
    std::string s(prec,'1');
    s=s+"0";
    a.setValue(std::string(prec,'9'));
    b.setValue(std::string(prec,'1'));
    a+=b;
    a.read();
    EXPECT_STREQ(ss.str().c_str(),s.c_str());
}

TEST_F(BigIntTest,sub_simple) {
    BigInteger a, b;
    a.setValue("765");
    b.setValue("321");
    (a-b).read();
    EXPECT_STREQ(ss.str().c_str(),"444");
}

TEST_F(BigIntTest,sub_borrow) {
    BigInteger a, b;
    a.setValue("1000");
    b.setValue("321");
    (a-b).strip().read();
    EXPECT_STREQ(ss.str().c_str(),"679");
}

TEST_F(BigIntTest,sub_diff_len) {
    BigInteger a, b;
    a.setValue("999");
    b.setValue("99");
    (a-b).read();
    EXPECT_STREQ(ss.str().c_str(),"900");
}

TEST_F(BigIntTest,sub_diff_len2) {
    BigInteger a, b;
    a.setValue("9999");
    b.setValue("99");
    (a-b).read();
    EXPECT_STREQ(ss.str().c_str(),"9900");
}

TEST_F(BigIntTest,sub_long_borrow) {
    BigInteger a, b;
    a.setValue("10000");
    b.setValue("33");
    (a-b).strip().read();
    EXPECT_STREQ(ss.str().c_str(),"9967");
}

TEST_F(BigIntTest,sub_result_with_0) {
    BigInteger a, b;
    a.setValue("10000");
    b.setValue("9990");
    (a-b).strip().read();
    EXPECT_STREQ(ss.str().c_str(),"10");
}

TEST_F(BigIntTest,multi_int_1) {
    BigInteger a;
    a.setValue("54321");
    (a*1).read();
    EXPECT_STREQ(ss.str().c_str(),"54321");
}

TEST_F(BigIntTest,multi_int_0) {
    BigInteger a;
    a.setValue("123");
    (a*0).read();
    EXPECT_STREQ(ss.str().c_str(),"0");
}

TEST_F(BigIntTest,multi_test) {
    BigInteger a;
    a.setValue("555");
    (a*2).read();
    EXPECT_STREQ(ss.str().c_str(),"1110");
}

TEST_F(BigIntTest,multi_test2) {
    BigInteger a;
    a.setValue("975315");
    (a*2).read();
    EXPECT_STREQ(ss.str().c_str(),"1950630");
}

TEST_F(BigIntTest,multi_test4) {
    BigInteger a;
    a.setValue("123456");
    (a*4).read();
    EXPECT_STREQ(ss.str().c_str(),"493824");
}

TEST_F(BigIntTest,multi_test8) {
    BigInteger a;
    a.setValue("98765");
    (a*8).read();
    EXPECT_STREQ(ss.str().c_str(),"790120");
}

TEST_F(BigIntTest,multi_multi9) {
    BigInteger a;
    a.setValue("99999");
    (a*9).read();
    EXPECT_STREQ(ss.str().c_str(),"899991");
}

TEST_F(BigIntTest,multi_multi11) {
    BigInteger a,b;
    a.setValue("5678");
    b.setValue("11");
    (a*b).read();
    EXPECT_STREQ(ss.str().c_str(),"62458");
}

TEST_F(BigIntTest,multi_multi1111) {
    BigInteger a,b;
    a.setValue("2333");
    b.setValue("1111");
    (a*b).read();
    EXPECT_STREQ(ss.str().c_str(),"2591963");
}

TEST_F(BigIntTest,multi_time_with_1e4) {
    int prec=10000;
    BigInteger a;
    std::string s(prec-1,'9');
    s=std::string(1,'8') + s + '1';
    a.setValue(std::string(prec,'9'));
    (a*9).read();
    EXPECT_STREQ(ss.str().c_str(),s.c_str());
}

TEST_F(BigIntTest,multi_time_with_1e6) {
    int prec=1000000;
    BigInteger a;
    std::string s(prec-1,'9');
    s=std::string(1,'8') + s + '1';
    a.setValue(std::string(prec,'9'));
    (a*9).read();
    EXPECT_STREQ(ss.str().c_str(),s.c_str());
}

TEST_F(BigIntTest,multi_between_time_1e4) {
    int prec=10000;
    BigInteger a,b;
    std::string s(prec-1,'9');
    s=std::string(1,'8') + s + '1';
    a.setValue(std::string("0")+std::string(prec,'9'));
    b.setValue(std::string("9")+std::string(prec,'0'));
    (a*b).read();
    EXPECT_STREQ(ss.str().c_str(),s.c_str());
}

TEST_F(BigIntDiv,div_simple) {
    BigInteger a;
    a.setValue("3");
    (BigInteger::divideOne(a, 5)).read();
    EXPECT_STREQ(ss.str().c_str(),"033333");
}

TEST_F(BigIntDiv,div_int_2) {
    BigInteger a;
    a.setValue("2");
    (BigInteger::divideOne(a, 5)).read();
    EXPECT_STREQ(ss.str().c_str(),"050000");
}

TEST_F(BigIntDiv,div_int_5) {
    BigInteger a;
    a.setValue("5");
    (BigInteger::divideOne(a, 5)).read();
    EXPECT_STREQ(ss.str().c_str(),"020000");
}

TEST_F(BigIntDiv,div_int_5_long) {
    int prec=1000;
    BigInteger a;
    a.setValue("5");
    std::string s("02"),zero(prec-1,'0');
    s=s+zero;
    (BigInteger::divideOne(a, prec)).read();
    EXPECT_STREQ(ss.str().c_str(),s.c_str());
}

TEST_F(BigIntDiv,div_int_2_long) {
    int prec=1000;
    BigInteger a;
    a.setValue("2");
    std::string s("05"),zero(prec-1,'0');
    s=s+zero;
    (BigInteger::divideOne(a, prec)).read();
    EXPECT_STREQ(ss.str().c_str(),s.c_str());
}

TEST_F(BigIntDiv,div_int_3_long) {
    int prec=1000;
    BigInteger a;
    a.setValue("3");
    std::string s("0");
    s=s+std::string(prec,'3');
    (BigInteger::divideOne(a, prec)).read();
    EXPECT_STREQ(ss.str().c_str(),s.c_str());
}

TEST_F(BigIntDiv,div_time_with_1e4) {
    int prec=10000;
    BigInteger a;
    a.setValue("3");
    std::string s("0");
    s=s+std::string(prec,'3');
    (BigInteger::divideOne(a, prec)).read();
    EXPECT_STREQ(ss.str().c_str(),s.c_str());
}

TEST_F(BigIntDiv,div_time_with_1e5) {
    int prec=100000;
    BigInteger a;
    a.setValue("3");
    std::string s("0");
    s=s+std::string(prec,'3');
    (BigInteger::divideOne(a, prec)).read();
    EXPECT_STREQ(ss.str().c_str(),s.c_str());
}

TEST_F(BigIntTest,half_test) {
    BigInteger a;
    a.setValue("2222");
    a.half().read();
    EXPECT_STREQ(ss.str().c_str(),"1111");
}

TEST_F(BigIntTest,half_test1) {
    BigInteger a;
    a.setValue("13");
    a.half().strip().read();
    EXPECT_STREQ(ss.str().c_str(),"6");
}

TEST_F(BigIntTest,half_test2) {
    BigInteger a;
    a.setValue("177776");
    a.half().strip().read();
    EXPECT_STREQ(ss.str().c_str(),"88888");
}

TEST_F(BigIntTest,half_test3) {
    BigInteger a;
    a.setValue("12345");
    a.half().strip().read();
    EXPECT_STREQ(ss.str().c_str(),"6172");
}

TEST_F(BigIntSqrt,sqrt2_float) {
    BigInteger a;
    a.setValue("002");
    a.sqrtPointR().read();
    EXPECT_STREQ(ss.str().c_str(),"01");
}

TEST_F(BigIntSqrt,sqrt2_float4) {
    BigInteger a;
    a.setValue("0020");
    a.sqrtPointR().read();
    EXPECT_STREQ(ss.str().c_str(),"014");
}

TEST_F(BigIntSqrt,sqrt2_float5) {
    BigInteger a;
    a.setValue("00200");
    a.sqrtPointR().read();
    EXPECT_STREQ(ss.str().c_str(),"014");
}

TEST_F(BigIntSqrt,sqrt2_float7) {
    BigInteger a;
    a.setValue("0020000");
    a.sqrtPointR().read();
    EXPECT_STREQ(ss.str().c_str(),"0141");
}

TEST_F(BigIntSqrt,sqrt2_float10) {
    BigInteger a;
    a.setValue(std::string("002")+std::string(10,'0'));
    a.sqrtPointR().read();
    EXPECT_STREQ(ss.str().c_str(),"0141421");
}

TEST_F(BigIntSqrt,sqrt2_float20) {
    BigInteger a;
    a.setValue(std::string("002")+std::string(20,'0'));
    a.sqrtPointR().read();
    EXPECT_STREQ(ss.str().c_str(),"014142135623");
}

TEST_F(BigIntSqrt,sqrt3_float20) {
    BigInteger a;
    a.setValue(std::string("003")+std::string(20,'0'));
    a.sqrtPointR().read();
    EXPECT_STREQ(ss.str().c_str(),"017320508075");
}

TEST_F(BigIntSqrt,sqrt5_float20) {
    BigInteger a;
    a.setValue(std::string("005")+std::string(20,'0'));
    a.sqrtPointR().read();
    EXPECT_STREQ(ss.str().c_str(),"022360679774");
}

TEST_F(BigIntSqrt,sqrt36) {
    BigInteger a;
    a.setValue(std::string("036")+std::string(10,'0'));
    a.sqrtPointR().read();
    EXPECT_STREQ(ss.str().c_str(),"0600000");
}

TEST_F(BigIntSqrt,sqrt81) {
    BigInteger a;
    a.setValue(std::string("081")+std::string(10,'0'));
    a.sqrtPointR().read();
    EXPECT_STREQ(ss.str().c_str(),"0900000");
}

TEST_F(BigIntSqrt,sqrt095) {
    BigInteger a;
    a.setValue(std::string("095")+std::string(20,'0'));
    a.sqrtPointR().read();
    EXPECT_STREQ(ss.str().c_str(),"097467943448");
}

int main (int argc,char** argv) {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}