//
// Created by user on 2022/10/14.
//

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <string>

#include "../fixed_high_prec.cpp"
using BigInteger = FixedInteger;

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

using FixedIntTest = RedirectedCoutFixture;
using BigIntAdd = RedirectedCoutFixture;
using FixedIntMult = RedirectedCoutFixture;
using BigIntDiv = RedirectedCoutFixture;
using BigIntSqrt = RedirectedCoutFixture;

TEST_F(BigIntAdd,simple) {
//    std::stringstream ss;
//    std::cout.rdbuf(ss.rdbuf());
    FixedInteger a;
    a.setValue("123",3);
    a.read();
    EXPECT_STREQ(ss.str().c_str(),"123");
}

TEST_F(BigIntAdd,simple_add) {
    FixedInteger a, b;
    a.setValue("123");
    b.setValue("555");
    (a+b).read();
    EXPECT_STREQ(ss.str().c_str(),"678");
}

TEST_F(BigIntAdd,carry_add_same_len) {
    FixedInteger a, b;
    a.setValue("129");
    b.setValue("333");
    (a+b).read();
    EXPECT_STREQ(ss.str().c_str(),"462");
}

TEST_F(BigIntAdd,add_time_with_1e4) {
    int prec=10000;
    FixedInteger a, b;
    std::string s(prec,'9');
    a.setValue(std::string(prec,'8'));
    b.setValue(std::string(prec,'1'));
    (a+b).read();
    EXPECT_STREQ(ss.str().c_str(),s.c_str());
}

TEST_F(BigIntAdd,add_time_with_1e6) {
    int prec=1000000;
    FixedInteger a, b;
    std::string s(prec,'9');
    a.setValue(std::string(prec,'8'));
    b.setValue(std::string(prec,'1'));
    (a+b).read();
    EXPECT_STREQ(ss.str().c_str(),s.c_str());
}

TEST_F(BigIntAdd,add_time_with_1e7) {
    int prec=10000000;
    FixedInteger a, b;
    std::string s(prec,'9');
    a.setValue(std::string(prec,'8'));
    b.setValue(std::string(prec,'1'));
    (a+b).read();
    EXPECT_STREQ(ss.str().c_str(),s.c_str());
}

TEST_F(FixedIntTest,sub_simple) {
    BigInteger a, b;
    a.setValue("765");
    b.setValue("321");
    (a-b).read();
    EXPECT_STREQ(ss.str().c_str(),"444");
}

TEST_F(FixedIntTest,sub_borrow) {
    BigInteger a, b;
    a.setValue("1000");
    b.setValue("0321");
    (a-b).read();
    EXPECT_STREQ(ss.str().c_str(),"679");
}

TEST_F(FixedIntTest,sub_diff_len) {
    BigInteger a, b;
    a.setValue("999");
    b.setValue("099");
    (a-b).read();
    EXPECT_STREQ(ss.str().c_str(),"900");
}

TEST_F(FixedIntTest,sub_diff_len2) {
    BigInteger a, b;
    a.setValue("9999");
    b.setValue("0099");
    (a-b).read();
    EXPECT_STREQ(ss.str().c_str(),"9900");
}

TEST_F(FixedIntTest,sub_long_borrow) {
    BigInteger a, b;
    a.setValue("10000");
    b.setValue("00033");
    (a-b).read();
    EXPECT_STREQ(ss.str().c_str(),"9967");
}

TEST_F(FixedIntTest,sub_result_with_0) {
    BigInteger a, b;
    a.setValue("10000");
    b.setValue("09990");
    (a-b).read();
    EXPECT_STREQ(ss.str().c_str(),"10");
}

TEST_F(FixedIntMult,multi_int_1) {
    BigInteger a;
    a.setValue("54321");
    (a*1).read();
    EXPECT_STREQ(ss.str().c_str(),"54321");
}

TEST_F(FixedIntMult,multi_int_0) {
    BigInteger a;
    a.setValue("123");
    (a*0).read();
    EXPECT_STREQ(ss.str().c_str(),"0");
}

TEST_F(FixedIntMult,multi_test) {
    BigInteger a;
    a.setValue("0555");
    (a*2).read();
    EXPECT_STREQ(ss.str().c_str(),"1110");
}

TEST_F(FixedIntMult,multi_test2) {
    BigInteger a;
    a.setValue("0975315");
    (a*2).read();
    EXPECT_STREQ(ss.str().c_str(),"1950630");
}

TEST_F(FixedIntMult,multi_test4) {
    BigInteger a;
    a.setValue("123456");
    (a*4).read();
    EXPECT_STREQ(ss.str().c_str(),"493824");
}

TEST_F(FixedIntMult,multi_test8) {
    BigInteger a;
    a.setValue("098765");
    (a*8).read();
    EXPECT_STREQ(ss.str().c_str(),"790120");
}

TEST_F(FixedIntMult,multi_multi9) {
    BigInteger a;
    a.setValue("099999");
    (a*9).read();
    EXPECT_STREQ(ss.str().c_str(),"899991");
}

TEST_F(FixedIntMult,multi_time_with_1e4) {
    int prec=10000;
    BigInteger a;
    std::string s(prec-1,'9');
    s=std::string(1,'8') + s + '1';
    a.setValue(std::string("0")+std::string(prec,'9'));
    (a*9).read();
    EXPECT_STREQ(ss.str().c_str(),s.c_str());
}

TEST_F(FixedIntMult,multi_time_with_1e6) {
    int prec=1000000;
    BigInteger a;
    std::string s(prec-1,'9');
    s=std::string(1,'8') + s + '1';
    a.setValue(std::string("0")+std::string(prec,'9'));
    (a*9).read();
    EXPECT_STREQ(ss.str().c_str(),s.c_str());
}


TEST(FixedIntMulti,multi_between_multi2) {
    FixedInteger a,b;
    a.setValue("0555");
    b.setValue("0200");
    EXPECT_STREQ((a*b).string(-1,false).c_str(),"0111");
}

TEST(FixedIntMulti,multi_between_multi2_) {
    FixedInteger a,b;
    a.setValue("0975315");
    b.setValue("2000000");
    EXPECT_STREQ((a*b).string(-1,false).c_str(),"1950630");
}

TEST(FixedIntMulti,multi_between_multi2__) {
    FixedInteger a,b;
    a.setValue("0975315");
    b.setValue("0200000");
    EXPECT_STREQ((a*b).string(-1,false).c_str(),"0195063");
}

TEST(FixedIntMulti,multi_between_multi4) {
    FixedInteger a,b;
    a.setValue("123456");
    b.setValue("400000");
    EXPECT_STREQ((a*b).string(-1,false).c_str(),"493824");
}

TEST(FixedIntMulti,multi_between_multi4_) {
    FixedInteger a,b;
    a.setValue("012345");
    b.setValue("040000");
    EXPECT_STREQ((a*b).string(-1,false).c_str(),"004938");
}


TEST(FixedIntMulti,multi_between_time_1e4) {
    int prec=10000;
    FixedInteger a,b;
    std::string s(prec-1,'9');
    s=std::string(1,'8') + s + '1';
    a.setValue(std::string("0")+std::string(prec,'9'));
    b.val.resize(a.size());
    b.val.at(0) = 9;
    EXPECT_STREQ((a*b).string(-1,false).c_str(),s.c_str());
}

TEST(FixedIntMulti,multi_between_time_1e6) {
    int prec=1000000;
    FixedInteger a,b;
    std::string s(prec-1,'9');
    s=std::string(1,'8') + s + '1';
    a.setValue(std::string("0")+std::string(prec,'9'));
    b.val.resize(a.size());
    b.val.at(0) = 9;
    EXPECT_STREQ((a*b).string(-1,false).c_str(),s.c_str());
}

TEST(FixedIntMulti,multi_between_half) {
    FixedInteger a,b;
    a.setValue("0123456");
    b.setValue("0500000");
    EXPECT_STREQ((a*b).string(-1,false).c_str(),"0061728");
}


TEST_F(BigIntDiv,div_simple) {
    BigInteger a;
    a.setValue("3");
    EXPECT_STREQ(BigInteger::divideOne(3, 5).string(-1,false).c_str(),"03333");
}

TEST_F(BigIntDiv,div_int_2) {
    BigInteger a;
    a.setValue("2");
    EXPECT_STREQ(BigInteger::divideOne(2, 5).string(-1,false).c_str(),"05000");
}

TEST_F(BigIntDiv,div_int_5) {
    BigInteger a;
    a.setValue("5");
    EXPECT_STREQ(BigInteger::divideOne(5, 5).string(-1,false).c_str(),"02000");
}

TEST_F(BigIntDiv,div_int_5_long) {
    int prec=1000;
    BigInteger a;
    a.setValue("5");
    std::string s("02"),zero(prec-2,'0');
    s=s+zero;
    auto str=(BigInteger::divideOne(5, prec)).string(-1,false);
    EXPECT_STREQ(str.c_str(),s.c_str());
}

TEST_F(BigIntDiv,div_int_2_long) {
    int prec=1000;
    BigInteger a;
    a.setValue("2");
    std::string s("05"),zero(prec-2,'0');
    s=s+zero;
    auto str=(BigInteger::divideOne(2, prec)).string(-1,false);
    EXPECT_STREQ(str.c_str(),s.c_str());
}

TEST_F(BigIntDiv,div_int_3_long) {
    int prec=1000;
    BigInteger a;
    a.setValue("3");
    std::string s("0");
    s=s+std::string(prec-1,'3');
    auto str=(BigInteger::divideOne(3, prec)).string(-1,false);
    EXPECT_STREQ(str.c_str(),s.c_str());
}

TEST_F(BigIntDiv,div_time_with_1e4) {
    int prec=10000;
    BigInteger a;
    a.setValue("3");
    std::string s("0");
    s=s+std::string(prec-1,'3');
    auto str=(BigInteger::divideOne(3, prec)).string(-1,false);
    EXPECT_STREQ(str.c_str(),s.c_str());
}

TEST_F(BigIntDiv,div_time_with_1e5) {
    int prec=100000;
    BigInteger a;
    a.setValue("3");
    std::string s("0");
    s=s+std::string(prec-1,'3');
    auto str=(BigInteger::divideOne(3, prec)).string(-1,false);
    EXPECT_STREQ(str.c_str(),s.c_str());
}


TEST_F(BigIntDiv,div_bet3) {
    BigInteger a;
    a.setValue("03000");
    EXPECT_STREQ(BigInteger::divideOne(a).string(-1, false).c_str(), "33333");
}

TEST_F(BigIntDiv,div_bet2) {
    BigInteger a;
    a.setValue("02000");
    EXPECT_STREQ(BigInteger::divideOne(a).string(-1, false).c_str(), "50000");
}

TEST_F(BigIntDiv,div_bet5) {
    BigInteger a;
    a.setValue("05000");
    EXPECT_STREQ(BigInteger::divideOne(a).string(-1, false).c_str(), "20000");
}

TEST_F(BigIntDiv,div_bet7) {
    BigInteger a;
    a.setValue("070000");
    EXPECT_STREQ(BigInteger::divideOne(a).string(-1, false).c_str(), "142857");
}

TEST_F(BigIntDiv,div_op_bet3) {
    BigInteger a,b;
    a.setValue("10000");
    b.setValue("03000");
    EXPECT_STREQ((a/b).string(-1, false).c_str(), "33333");
}

TEST_F(BigIntDiv,div_op_bet2) {
    BigInteger a,b;
    a.setValue("001200");
    b.setValue("002000");
    EXPECT_STREQ((a/b).string(-1, false).c_str(), "060000");
}

TEST_F(BigIntDiv,div_op_bet5) {
    BigInteger a,b;
    a.setValue("012000");
    b.setValue("050000");
    EXPECT_STREQ((a/b).string(-1, false).c_str(), "024000");
}

TEST_F(BigIntDiv,div_op_bet7) {
    BigInteger a,b;
    a.setValue("0211000");
    b.setValue("0070000");
    EXPECT_STREQ((a/b).string(-1, false).c_str(), "3014285");
}

TEST_F(BigIntDiv,div_bet_int_2_long) {
    int prec=1000;
    BigInteger a;
    a.setValue("02");
    a.val.resize(prec);
    std::string s("5"),zero(prec-1,'0');
    s=s+zero;
    auto str=(BigInteger::divideOne(a)).string(-1, false);
    EXPECT_STREQ(str.c_str(),s.c_str());
}

TEST_F(BigIntDiv,div_bet_int_3_long) {
    int prec=1000;
    BigInteger a;
    a.setValue("03");
    a.val.resize(prec);
    std::string s(prec,'3');
    auto str=(BigInteger::divideOne(a)).string(-1, false);
    EXPECT_STREQ(str.c_str(),s.c_str());
}

int main (int argc,char** argv) {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}