/* Copyright (C) 2015 Alexander Shishenko <GamePad64@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <unicbor/packer.h>
#include <gtest/gtest.h>

#include "testdata.h"

class TestPacker : public ::testing::Test {
protected:
    void SetUp() {
        packer_ = new unicbor::packer(os_);
    }

    void TearDown() {
        delete packer_;
        os_.str("");
    }

    std::ostringstream os_;
    unicbor::packer *packer_;
};

TEST_F(TestPacker, int1) {
    packer_->pack(0);
	ASSERT_EQ(int1, os_.str());
}

TEST_F(TestPacker, int2) {
    packer_->pack(1);
    ASSERT_EQ(int2, os_.str());
}

TEST_F(TestPacker, int3) {
    packer_->pack(10);
    ASSERT_EQ(int3, os_.str());
}

TEST_F(TestPacker, int4) {
    packer_->pack(23);
    ASSERT_EQ(int4, os_.str());
}

TEST_F(TestPacker, int5) {
    packer_->pack(24);
    ASSERT_EQ(int5, os_.str());
}

TEST_F(TestPacker, int6) {
    packer_->pack(25);
    ASSERT_EQ(int6, os_.str());
}

TEST_F(TestPacker, int7) {
    packer_->pack(100);
    ASSERT_EQ(int7, os_.str());
}

TEST_F(TestPacker, int8) {
    packer_->pack(1000);
    ASSERT_EQ(int8, os_.str());
}

TEST_F(TestPacker, int9) {
    packer_->pack(1000000ul);
    ASSERT_EQ(int9, os_.str());
}

TEST_F(TestPacker, int10) {
    packer_->pack(1000000000000ull);
    ASSERT_EQ(int10, os_.str());
}

TEST_F(TestPacker, int11) {
    packer_->pack(18446744073709551615ull);
    ASSERT_EQ(int11, os_.str());
}

TEST_F(TestPacker, bigint1) {
    packer_->pack_tag(unicbor::tag::BIGNUM_POSITIVE);
    packer_->pack_bin("\x01\0\0\0\0\0\0\0\0", 9);
    ASSERT_EQ(bigint1, os_.str());
}


TEST_F(TestPacker, neg_bigint1) {
    packer_->pack_neg_int(18446744073709551615ull);
    ASSERT_EQ(neg_bigint1, os_.str());
}

TEST_F(TestPacker, neg_bigint2) {
    packer_->pack_tag(unicbor::tag::BIGNUM_NEGATIVE);
    packer_->pack_bin("\x01\0\0\0\0\0\0\0\0", 9);
    ASSERT_EQ(neg_bigint2, os_.str());
}

TEST_F(TestPacker, neg_int1) {
    packer_->pack(-1);
    ASSERT_EQ(neg_int1, os_.str());
}

TEST_F(TestPacker, neg_int2) {
    packer_->pack(-10);
    ASSERT_EQ(neg_int2, os_.str());
}

TEST_F(TestPacker, neg_int3) {
    packer_->pack(-100);
    ASSERT_EQ(neg_int3, os_.str());
}

TEST_F(TestPacker, neg_int4) {
    packer_->pack(-1000);
    ASSERT_EQ(neg_int4, os_.str());
}

TEST_F(TestPacker, float1) {
    packer_->pack(0.0);
    ASSERT_EQ(float1, os_.str());
}

TEST_F(TestPacker, float2) {
    packer_->pack(-0.0);
    ASSERT_EQ(float2, os_.str());
}

TEST_F(TestPacker, float3) {
    packer_->pack(1.0);
    ASSERT_EQ(float3, os_.str());
}

TEST_F(TestPacker, float4) {
    packer_->pack(1.1);
    ASSERT_EQ(float4, os_.str());
}

TEST_F(TestPacker, float5) {
    packer_->pack(1.5);
    ASSERT_EQ(float5, os_.str());
}

TEST_F(TestPacker, float6) {
    packer_->pack(65504.0);
    ASSERT_EQ(float6, os_.str());
}

TEST_F(TestPacker, float7) {
    packer_->pack(100000.0);
    ASSERT_EQ(float7, os_.str());
}

TEST_F(TestPacker, float8) {
    packer_->pack(3.4028234663852886e+38);
    ASSERT_EQ(float8, os_.str());
}

TEST_F(TestPacker, float9) {
    packer_->pack(1.0e+300);
    ASSERT_EQ(float9, os_.str());
}

TEST_F(TestPacker, float10) {
    packer_->pack(5.960464477539063e-8);
    ASSERT_EQ(float10, os_.str());
}

TEST_F(TestPacker, float11) {
    packer_->pack(0.00006103515625);
    ASSERT_EQ(float11, os_.str());
}

TEST_F(TestPacker, float12) {
    packer_->pack(-4.0);
    ASSERT_EQ(float12, os_.str());
}

TEST_F(TestPacker, float13) {
    packer_->pack(-4.1);
    ASSERT_EQ(float13, os_.str());
}

TEST_F(TestPacker, half_limits1) {
    packer_->pack_half(unicbor::half(INFINITY));
    ASSERT_EQ(half_limits1, os_.str());
}

TEST_F(TestPacker, half_limits2) {
    packer_->pack_half(unicbor::half(NAN));
    ASSERT_EQ(half_limits2, os_.str());
}

TEST_F(TestPacker, half_limits3) {
    packer_->pack_half(unicbor::half(-INFINITY));
    ASSERT_EQ(half_limits3, os_.str());
}

TEST_F(TestPacker, float_limits1) {
    packer_->pack_float(float(INFINITY));
    ASSERT_EQ(float_limits1, os_.str());
}

TEST_F(TestPacker, float_limits2) {
    packer_->pack_float(float(NAN));
    ASSERT_EQ(float_limits2, os_.str());
}

TEST_F(TestPacker, float_limits3) {
    packer_->pack_float(float(-INFINITY));
    ASSERT_EQ(float_limits3, os_.str());
}

TEST_F(TestPacker, double_limits1) {
    packer_->pack_double(double(INFINITY));
    ASSERT_EQ(double_limits1, os_.str());
}

TEST_F(TestPacker, double_limits2) {
    packer_->pack_double(double(NAN));
    ASSERT_EQ(double_limits2, os_.str());
}

TEST_F(TestPacker, double_limits3) {
    packer_->pack_double(double(-INFINITY));
    ASSERT_EQ(double_limits3, os_.str());
}

TEST_F(TestPacker, simple1) {
    packer_->pack(false);
    ASSERT_EQ(simple1, os_.str());
}

TEST_F(TestPacker, simple2) {
    packer_->pack(true);
    ASSERT_EQ(simple2, os_.str());
}

TEST_F(TestPacker, simple3) {
    packer_->pack(nullptr);
    ASSERT_EQ(simple3, os_.str());
}

TEST_F(TestPacker, simple4) {
    packer_->pack(unicbor::undefined);
    ASSERT_EQ(simple4, os_.str());
}

TEST_F(TestPacker, simple5) {
    packer_->pack(unicbor::simple_value(16));
    ASSERT_EQ(simple5, os_.str());
}

TEST_F(TestPacker, simple6) {
    packer_->pack(unicbor::simple_value(24));
    ASSERT_EQ(simple6, os_.str());
}

TEST_F(TestPacker, simple7) {
    packer_->pack(unicbor::simple_value(255));
    ASSERT_EQ(simple7, os_.str());
}

TEST_F(TestPacker, tag1) {
    packer_->pack(unicbor::tag::DATETIME_STRING);
    packer_->pack("2013-03-21T20:04:00Z");
    ASSERT_EQ(tag1, os_.str());
}

TEST_F(TestPacker, tag2) {
    packer_->pack(unicbor::tag::DATETIME_NUMERIC);
    packer_->pack(1363896240);
    ASSERT_EQ(tag2, os_.str());
}

TEST_F(TestPacker, tag3) {
    packer_->pack(unicbor::tag::DATETIME_NUMERIC);
    packer_->pack(1363896240.5);
    ASSERT_EQ(tag3, os_.str());
}

TEST_F(TestPacker, tag4) {
    packer_->pack(unicbor::tag::EXPECT_HEX);
    packer_->pack_bin("\x01\x02\x03\x04", 4);
    ASSERT_EQ(tag4, os_.str());
}

TEST_F(TestPacker, tag5) {
    packer_->pack(unicbor::tag::ENCODED_CBOR);
    packer_->pack_bin("\x64\x49\x45\x54\x46", 5);
    ASSERT_EQ(tag5, os_.str());
}

TEST_F(TestPacker, tag6) {
    packer_->pack(unicbor::tag::URI);
    packer_->pack("http://www.example.com", 22);
    ASSERT_EQ(tag6, os_.str());
}

TEST_F(TestPacker, bin_string1) {
    packer_->pack_bin("", 0);
    ASSERT_EQ(bin_string1, os_.str());
}

TEST_F(TestPacker, bin_string2) {
    packer_->pack_bin("\x01\x02\x03\x04", 4);
    ASSERT_EQ(bin_string2, os_.str());
}

TEST_F(TestPacker, string1) {
    packer_->pack_string("", 0);
    ASSERT_EQ(string1, os_.str());
}

TEST_F(TestPacker, string2) {
    packer_->pack_string("a", 1);
    ASSERT_EQ(string2, os_.str());
}

TEST_F(TestPacker, string3) {
    packer_->pack_string("IETF", 4);
    ASSERT_EQ(string3, os_.str());
}

TEST_F(TestPacker, string4) {
    packer_->pack_string("\"\\", 2);
    ASSERT_EQ(string4, os_.str());
}

TEST_F(TestPacker, string5) {
    packer_->pack_string("\u00fc", 2);
    ASSERT_EQ(string5, os_.str());
}

TEST_F(TestPacker, string6) {
    packer_->pack_string("\u6c34", 3);
    ASSERT_EQ(string6, os_.str());
}

TEST_F(TestPacker, string7) {
    packer_->pack_string("\xf0\x90\x85\x91", 4);
    ASSERT_EQ(string7, os_.str());
}

TEST_F(TestPacker, array1) {
    packer_->start_array(0);
    ASSERT_EQ(array1, os_.str());
}

TEST_F(TestPacker, array2) {
    packer_->start_array(3);
    packer_->pack(1);
    packer_->pack(2);
    packer_->pack(3);
    ASSERT_EQ(array2, os_.str());
}

TEST_F(TestPacker, array3) {
    packer_->start_array(3);
    packer_->pack(1);
    packer_->start_array(2);
    packer_->pack(2);
    packer_->pack(3);
    packer_->start_array(2);
    packer_->pack(4);
    packer_->pack(5);
    ASSERT_EQ(array3, os_.str());
}

TEST_F(TestPacker, array4) {
    packer_->start_array(25);
    for (int i = 1; i <= 25; i++)
        packer_->pack(i);
    ASSERT_EQ(array4, os_.str());
}

TEST_F(TestPacker, map1) {
    packer_->start_map(0);
    ASSERT_EQ(map1, os_.str());
}

TEST_F(TestPacker, map2) {
    packer_->pack(std::map<int, int>({{1, 2},
                                      {3, 4}}));
    ASSERT_EQ(map2, os_.str());
}

TEST_F(TestPacker, map3) {
    packer_->start_map(2);
    packer_->pack("a");
    packer_->pack(1);
    packer_->pack("b");
    packer_->start_array(2);
    packer_->pack(2);
    packer_->pack(3);
    ASSERT_EQ(map3, os_.str());
}

TEST_F(TestPacker, map4) {
    packer_->start_array(2);
    packer_->pack("a");
    packer_->start_map(1);
    packer_->pack("b");
    packer_->pack("c");
    ASSERT_EQ(map4, os_.str());
}

TEST_F(TestPacker, map5) {
    packer_->pack(std::map<std::string, std::string>({
                                                             {"a", "A"},
                                                             {"b", "B"},
                                                             {"c", "C"},
                                                             {"d", "D"},
                                                             {"e", "E"}
                                                     }));
    ASSERT_EQ(map5, os_.str());
}

TEST_F(TestPacker, indef1) {
	packer_->start_bin();
	packer_->pack_bin("\x01\x02", 2);
    packer_->pack_bin("\x03\x04\x05", 3);
    packer_->stop_bin();
    ASSERT_EQ(indef1, os_.str());
}

TEST_F(TestPacker, indef2) {
    packer_->start_string();
    packer_->pack_string("strea", 5);
    packer_->pack_string("ming", 4);
    packer_->stop_string();
    ASSERT_EQ(indef2, os_.str());
}

TEST_F(TestPacker, indef3) {
    packer_->start_array();
    packer_->stop_array();
    ASSERT_EQ(indef3, os_.str());
}

TEST_F(TestPacker, indef4) {
    packer_->start_array();
    packer_->pack(1);
    packer_->start_array(2);
    packer_->pack(2);
    packer_->pack(3);
    packer_->start_array();
    packer_->pack(4);
    packer_->pack(5);
    packer_->stop_array();
    packer_->stop_array();
    ASSERT_EQ(indef4, os_.str());
}

TEST_F(TestPacker, indef5) {
    packer_->start_array();
    packer_->pack(1);
    packer_->start_array(2);
    packer_->pack(2);
    packer_->pack(3);
    packer_->start_array(2);
    packer_->pack(4);
    packer_->pack(5);
    packer_->stop_array();
    ASSERT_EQ(indef5, os_.str());
}

TEST_F(TestPacker, indef6) {
    packer_->start_array(3);
    packer_->pack(1);
    packer_->start_array(2);
    packer_->pack(2);
    packer_->pack(3);
    packer_->start_array();
    packer_->pack(4);
    packer_->pack(5);
    packer_->stop_array();
    ASSERT_EQ(indef6, os_.str());
}

TEST_F(TestPacker, indef7) {
    packer_->start_array(3);
    packer_->pack(1);
    packer_->start_array();
    packer_->pack(2);
    packer_->pack(3);
    packer_->stop_array();
    packer_->start_array(2);
    packer_->pack(4);
    packer_->pack(5);
    ASSERT_EQ(indef7, os_.str());
}

TEST_F(TestPacker, indef8) {
    packer_->start_array();
    for (auto i = 1; i <= 25; i++)
        packer_->pack(i);
    packer_->stop_array();
    ASSERT_EQ(indef8, os_.str());
}

TEST_F(TestPacker, indef9) {
    packer_->start_map();
    packer_->pack("a");
    packer_->pack(1);
    packer_->pack("b");
    packer_->start_array();
    packer_->pack(2);
    packer_->pack(3);
    packer_->stop_array();
    packer_->stop_map();
    ASSERT_EQ(indef9, os_.str());
}

TEST_F(TestPacker, indef10) {
    packer_->start_array(2);
    packer_->pack("a");
    packer_->start_map();
    packer_->pack("b");
    packer_->pack("c");
    packer_->stop_map();
    ASSERT_EQ(indef10, os_.str());
}

TEST_F(TestPacker, indef11) {
    packer_->start_map();
    packer_->pack("Fun");
    packer_->pack(true);
    packer_->pack("Amt");
    packer_->pack(-2);
    packer_->stop_map();
    ASSERT_EQ(indef11, os_.str());
}
