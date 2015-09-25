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
#include <cborpp/packer.h>
#include <gtest/gtest.h>

#include "testdata.h"

class TestPacker : public ::testing::Test {
protected:
    void SetUp() {
        packer_ = new cborpp::packer<std::ostream>(os_);
    }

    void TearDown() {
        delete packer_;
        os_.str("");
    }

    std::ostringstream os_;
    cborpp::packer<std::ostream> *packer_;
};

TEST_F(TestPacker, int1) {
    packer_->pack(0);
	ASSERT_EQ(int1, os_.str());
}

TEST_F(TestPacker, int2) {
    packer_->pack(1);
    ASSERT_EQ("\x1", os_.str());
}

TEST_F(TestPacker, int3) {
    packer_->pack(10);
    ASSERT_EQ("\xA", os_.str());
}

TEST_F(TestPacker, int4) {
    packer_->pack(23);
    ASSERT_EQ("\x17", os_.str());
}

TEST_F(TestPacker, int5) {
    packer_->pack(24);
    ASSERT_EQ("\x18\x18", os_.str());
}

TEST_F(TestPacker, int6) {
    packer_->pack(25);
    ASSERT_EQ("\x18\x19", os_.str());
}

TEST_F(TestPacker, int7) {
    packer_->pack(100);
    ASSERT_EQ("\x18\x64", os_.str());
}

TEST_F(TestPacker, int8) {
    packer_->pack(1000);
    ASSERT_EQ("\x19\x03\xe8", os_.str());
}

TEST_F(TestPacker, int9) {
    packer_->pack(1000000ul);
    ASSERT_EQ(std::string("\x1a\0\x0f\x42\x40", 5), os_.str());
}

TEST_F(TestPacker, int10) {
    packer_->pack(1000000000000ull);
    ASSERT_EQ(std::string("\x1b\0\0\0\xe8\xd4\xa5\x10\0", 9), os_.str());
}

TEST_F(TestPacker, int11) {
    packer_->pack(18446744073709551615ull);
    ASSERT_EQ(std::string("\x1b\xff\xff\xff\xff\xff\xff\xff\xff", 9), os_.str());
}

TEST_F(TestPacker, bigint1) {
    packer_->pack_tag(cborpp::tag::BIGNUM_POSITIVE);
    packer_->pack_bin("\x01\0\0\0\0\0\0\0\0", 9);
    ASSERT_EQ(std::string("\xc2\x49\x01\0\0\0\0\0\0\0\0", 11), os_.str());
}

TEST_F(TestPacker, DISABLED_neg_bigint1) {
    packer_->pack(-18446744073709551615ll);
    ASSERT_EQ(std::string("\x3b\xff\xff\xff\xff\xff\xff\xff\xff", 9), os_.str());
}

TEST_F(TestPacker, neg_bigint2) {
    packer_->pack_tag(cborpp::tag::BIGNUM_NEGATIVE);
    packer_->pack_bin("\x01\0\0\0\0\0\0\0\0", 9);
    ASSERT_EQ(std::string("\xc3\x49\x01\0\0\0\0\0\0\0\0", 11), os_.str());
}

TEST_F(TestPacker, neg_int1) {
    packer_->pack(-1);
    ASSERT_EQ(std::string("\x20", 1), os_.str());
}

TEST_F(TestPacker, neg_int2) {
    packer_->pack(-10);
    ASSERT_EQ(std::string("\x29", 1), os_.str());
}

TEST_F(TestPacker, neg_int3) {
    packer_->pack(-100);
    ASSERT_EQ(std::string("\x38\x63", 2), os_.str());
}

TEST_F(TestPacker, neg_int4) {
    packer_->pack(-1000);
    ASSERT_EQ(std::string("\x39\x03\xe7", 3), os_.str());
}

TEST_F(TestPacker, float1) {
    packer_->pack(0.0);
    ASSERT_EQ(std::string("\xf9\0\0", 3), os_.str());
}

TEST_F(TestPacker, float2) {
    packer_->pack(-0.0);
    ASSERT_EQ(std::string("\xf9\x80\0", 3), os_.str());
}

TEST_F(TestPacker, float3) {
    packer_->pack(1.0);
    ASSERT_EQ(std::string("\xf9\x3c\0", 3), os_.str());
}

TEST_F(TestPacker, float4) {
    packer_->pack(1.1);
    ASSERT_EQ(std::string("\xfb\x3f\xf1\x99\x99\x99\x99\x99\x9a", 9), os_.str());
}

TEST_F(TestPacker, float5) {
    packer_->pack(1.5);
    ASSERT_EQ(std::string("\xf9\x3e\0", 3), os_.str());
}

TEST_F(TestPacker, float6) {
    packer_->pack(65504.0);
    ASSERT_EQ(std::string("\xf9\x7b\xff", 3), os_.str());
}

TEST_F(TestPacker, float7) {
    packer_->pack(100000.0);
    ASSERT_EQ(std::string("\xfa\x47\xc3\x50\0", 5), os_.str());
}

TEST_F(TestPacker, float8) {
    packer_->pack(3.4028234663852886e+38);
    ASSERT_EQ(std::string("\xfa\x7f\x7f\xff\xff", 5), os_.str());
}

TEST_F(TestPacker, float9) {
    packer_->pack(1.0e+300);
    ASSERT_EQ(std::string("\xfb\x7e\x37\xe4\x3c\x88\0\x75\x9c", 9), os_.str());
}

TEST_F(TestPacker, float10) {
    packer_->pack(5.960464477539063e-8);
    ASSERT_EQ(std::string("\xf9\0\x01", 3), os_.str());
}

TEST_F(TestPacker, float11) {
    packer_->pack(0.00006103515625);
    ASSERT_EQ(std::string("\xf9\x04\0", 3), os_.str());
}

TEST_F(TestPacker, float12) {
    packer_->pack(-4.0);
    ASSERT_EQ(std::string("\xf9\xc4\0", 3), os_.str());
}

TEST_F(TestPacker, float13) {
    packer_->pack(-4.1);
    ASSERT_EQ(std::string("\xfb\xc0\x10\x66\x66\x66\x66\x66\x66", 9), os_.str());
}

TEST_F(TestPacker, half_limits1) {
    packer_->pack_half(cborpp::half(INFINITY));
    ASSERT_EQ(std::string("\xf9\x7c\0", 3), os_.str());
}

TEST_F(TestPacker, half_limits2) {
    packer_->pack_half(cborpp::half(NAN));
    ASSERT_EQ(std::string("\xf9\x7e\0", 3), os_.str());
}

TEST_F(TestPacker, half_limits3) {
    packer_->pack_half(cborpp::half(-INFINITY));
    ASSERT_EQ(std::string("\xf9\xfc\0", 3), os_.str());
}

TEST_F(TestPacker, float_limits1) {
    packer_->pack_float(float(INFINITY));
    ASSERT_EQ(std::string("\xfa\x7f\x80\0\0", 5), os_.str());
}

TEST_F(TestPacker, float_limits2) {
    packer_->pack_float(float(NAN));
    ASSERT_EQ(std::string("\xfa\x7f\xc0\0\0", 5), os_.str());
}

TEST_F(TestPacker, float_limits3) {
    packer_->pack_float(float(-INFINITY));
    ASSERT_EQ(std::string("\xfa\xff\x80\0\0", 5), os_.str());
}

TEST_F(TestPacker, double_limits1) {
    packer_->pack_double(double(INFINITY));
    ASSERT_EQ(std::string("\xfb\x7f\xf0\0\0\0\0\0\0", 9), os_.str());
}

TEST_F(TestPacker, double_limits2) {
    packer_->pack_double(double(NAN));
    ASSERT_EQ(std::string("\xfb\x7f\xf8\0\0\0\0\0\0", 9), os_.str());
}

TEST_F(TestPacker, double_limits3) {
    packer_->pack_double(double(-INFINITY));
    ASSERT_EQ(std::string("\xfb\xff\xf0\0\0\0\0\0\0", 9), os_.str());
}

TEST_F(TestPacker, simple1) {
    packer_->pack(false);
    ASSERT_EQ(std::string("\xf4", 1), os_.str());
}

TEST_F(TestPacker, simple2) {
    packer_->pack(true);
    ASSERT_EQ(std::string("\xf5", 1), os_.str());
}

TEST_F(TestPacker, simple3) {
    packer_->pack(nullptr);
    ASSERT_EQ(std::string("\xf6", 1), os_.str());
}

TEST_F(TestPacker, simple4) {
    packer_->pack(cborpp::undefined);
    ASSERT_EQ(std::string("\xf7", 1), os_.str());
}

TEST_F(TestPacker, simple5) {
    packer_->pack(cborpp::simple_value(16));
    ASSERT_EQ(std::string("\xf0", 1), os_.str());
}

TEST_F(TestPacker, simple6) {
    packer_->pack(cborpp::simple_value(24));
    ASSERT_EQ(std::string("\xf8\x18", 2), os_.str());
}

TEST_F(TestPacker, simple7) {
    packer_->pack(cborpp::simple_value(255));
    ASSERT_EQ(std::string("\xf8\xff", 2), os_.str());
}

TEST_F(TestPacker, tag1) {
    packer_->pack(cborpp::tag::DATETIME_STRING);
    packer_->pack("2013-03-21T20:04:00Z");
    ASSERT_EQ(std::string("\xc0\x74\x32\x30\x31\x33\x2d\x30\x33\x2d\x32\x31\x54\x32\x30\x3a\x30\x34\x3a\x30\x30\x5a ",
                          22), os_.str());
}

TEST_F(TestPacker, tag2) {
    packer_->pack(cborpp::tag::DATETIME_NUMERIC);
    packer_->pack(1363896240);
    ASSERT_EQ(std::string("\xc1\x1a\x51\x4b\x67\xb0 ", 6), os_.str());
}

TEST_F(TestPacker, tag3) {
    packer_->pack(cborpp::tag::DATETIME_NUMERIC);
    packer_->pack(1363896240.5);
    ASSERT_EQ(std::string("\xc1\xfb\x41\xd4\x52\xd9\xec\x20\0\0 ", 10), os_.str());
}

TEST_F(TestPacker, tag4) {
    packer_->pack(cborpp::tag::EXPECT_HEX);
    packer_->pack_bin("\x01\x02\x03\x04", 4);
    ASSERT_EQ(std::string("\xd7\x44\x01\x02\x03\x04", 6), os_.str());
}

TEST_F(TestPacker, tag5) {
    packer_->pack(cborpp::tag::ENCODED_CBOR);
    packer_->pack_bin("\x64\x49\x45\x54\x46", 5);
    ASSERT_EQ(std::string("\xd8\x18\x45\x64\x49\x45\x54\x46", 8), os_.str());
}

TEST_F(TestPacker, tag6) {
    packer_->pack(cborpp::tag::URI);
    packer_->pack("http://www.example.com", 22);
    ASSERT_EQ(std::string(
                      "\xd8\x20\x76\x68\x74\x74\x70\x3a\x2f\x2f\x77\x77\x77\x2e\x65\x78\x61\x6d\x70\x6c\x65\x2e\x63\x6f\x6d", 25),
              os_.str());
}

TEST_F(TestPacker, tag7) {
    packer_->pack(cborpp::tag::URI);
    packer_->pack("http://www.example.com", 22);
    ASSERT_EQ(std::string(
                      "\xd8\x20\x76\x68\x74\x74\x70\x3a\x2f\x2f\x77\x77\x77\x2e\x65\x78\x61\x6d\x70\x6c\x65\x2e\x63\x6f\x6d", 25),
              os_.str());
}

TEST_F(TestPacker, tag8) {
    packer_->pack_bin("", 0);
    ASSERT_EQ(std::string("\x40", 1), os_.str());
}

TEST_F(TestPacker, tag9) {
    packer_->pack_bin("\x01\x02\x03\x04", 4);
    ASSERT_EQ(std::string("\x44\x01\x02\x03\x04", 5), os_.str());
}

TEST_F(TestPacker, tag10) {
    packer_->pack_string("", 0);
    ASSERT_EQ(std::string("\x60", 1), os_.str());
}

TEST_F(TestPacker, tag11) {
    packer_->pack_string("a", 1);
    ASSERT_EQ(std::string("\x61\x61", 2), os_.str());
}

TEST_F(TestPacker, tag12) {
    packer_->pack_string("IETF", 4);
    ASSERT_EQ(std::string("\x64\x49\x45\x54\x46", 5), os_.str());
}

TEST_F(TestPacker, tag13) {
    packer_->pack_string("\"\\", 2);
    ASSERT_EQ(std::string("\x62\x22\x5c", 3), os_.str());
}

TEST_F(TestPacker, tag14) {
    packer_->pack_string("\u00fc", 2);
    ASSERT_EQ(std::string("\x62\xc3\xbc", 3), os_.str());
}

TEST_F(TestPacker, tag15) {
    packer_->pack_string("\u6c34", 3);
    ASSERT_EQ(std::string("\x63\xe6\xb0\xb4", 4), os_.str());
}

TEST_F(TestPacker, tag16) {
    packer_->pack_string("\xf0\x90\x85\x91", 4);
    ASSERT_EQ(std::string("\x64\xf0\x90\x85\x91", 5), os_.str());
}

TEST_F(TestPacker, array1) {
    packer_->pack_array(0);
    ASSERT_EQ(std::string("\x80", 1), os_.str());
}

TEST_F(TestPacker, array2) {
    packer_->pack_array(3);
    packer_->pack(1);
    packer_->pack(2);
    packer_->pack(3);
    ASSERT_EQ(std::string("\x83\x01\x02\x03", 4), os_.str());
}

TEST_F(TestPacker, array3) {
    packer_->pack_array(3);
    packer_->pack(1);
    packer_->pack_array(2);
    packer_->pack(2);
    packer_->pack(3);
    packer_->pack_array(2);
    packer_->pack(4);
    packer_->pack(5);
    ASSERT_EQ(std::string("\x83\x01\x82\x02\x03\x82\x04\x05", 8), os_.str());
}

TEST_F(TestPacker, array4) {
    packer_->pack_array(25);
    for (int i = 1; i <= 25; i++)
        packer_->pack(i);
    ASSERT_EQ(std::string(
                      "\x98\x19\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x18\x18\x19"),
              os_.str());
}

TEST_F(TestPacker, map1) {
    packer_->pack_map(0);
    ASSERT_EQ(std::string("\xa0"), os_.str());
}

TEST_F(TestPacker, map2) {
    packer_->pack(std::map<int, int>({{1, 2},
                                      {3, 4}}));
    ASSERT_EQ(std::string("\xa2\x01\x02\x03\x04"), os_.str());
}

TEST_F(TestPacker, map3) {
    packer_->pack_map(2);
    packer_->pack("a");
    packer_->pack(1);
    packer_->pack("b");
    packer_->pack_array(2);
    packer_->pack(2);
    packer_->pack(3);
    ASSERT_EQ(std::string("\xa2\x61\x61\x01\x61\x62\x82\x02\x03", 9), os_.str());
}

TEST_F(TestPacker, map4) {
    packer_->pack_array(2);
    packer_->pack("a");
    packer_->pack_map(1);
    packer_->pack("b");
    packer_->pack("c");
    ASSERT_EQ(std::string("\x82\x61\x61\xa1\x61\x62\x61\x63", 8), os_.str());
}

TEST_F(TestPacker, map5) {
    packer_->pack(std::map<std::string, std::string>({
                                                             {"a", "A"},
                                                             {"b", "B"},
                                                             {"c", "C"},
                                                             {"d", "D"},
                                                             {"e", "E"}
                                                     }));
    ASSERT_EQ(std::string("\xa5\x61\x61\x61\x41\x61\x62\x61\x42\x61\x63\x61\x43\x61\x64\x61\x44\x61\x65\x61\x45", 21),
              os_.str());
}

TEST_F(TestPacker, indef1) {
    packer_->pack_indefinite_bin();
    packer_->pack_bin("\x01\x02", 2);
    packer_->pack_bin("\x03\x04\x05", 3);
    packer_->pack_indefinite_break();
    ASSERT_EQ(std::string("\x5f\x42\x01\x02\x43\x03\x04\x05\xff", 9), os_.str());
}

TEST_F(TestPacker, indef2) {
    packer_->pack_indefinite_string();
    packer_->pack("strea");
    packer_->pack("ming");
    packer_->pack_indefinite_break();
    ASSERT_EQ(std::string("\x7f\x65\x73\x74\x72\x65\x61\x64\x6d\x69\x6e\x67\xff", 13), os_.str());
}

TEST_F(TestPacker, indef3) {
    packer_->pack_indefinite_array();
    packer_->pack_indefinite_break();
    ASSERT_EQ(std::string("\x9f\xff", 2), os_.str());
}

TEST_F(TestPacker, indef4) {
    packer_->pack_indefinite_array();
    packer_->pack(1);
    packer_->pack_array(2);
    packer_->pack(2);
    packer_->pack(3);
    packer_->pack_indefinite_array();
    packer_->pack(4);
    packer_->pack(5);
    packer_->pack_indefinite_break();
    packer_->pack_indefinite_break();
    ASSERT_EQ(std::string("\x9f\x01\x82\x02\x03\x9f\x04\x05\xff\xff", 10), os_.str());
}

TEST_F(TestPacker, indef5) {
    packer_->pack_indefinite_array();
    packer_->pack(1);
    packer_->pack_array(2);
    packer_->pack(2);
    packer_->pack(3);
    packer_->pack_array(2);
    packer_->pack(4);
    packer_->pack(5);
    packer_->pack_indefinite_break();
    ASSERT_EQ(std::string("\x9f\x01\x82\x02\x03\x82\x04\x05\xff", 9), os_.str());
}

TEST_F(TestPacker, indef6) {
    packer_->pack_array(3);
    packer_->pack(1);
    packer_->pack_array(2);
    packer_->pack(2);
    packer_->pack(3);
    packer_->pack_indefinite_array();
    packer_->pack(4);
    packer_->pack(5);
    packer_->pack_indefinite_break();
    ASSERT_EQ(std::string("\x83\x01\x82\x02\x03\x9f\x04\x05\xff", 9), os_.str());
}

TEST_F(TestPacker, indef7) {
    packer_->pack_array(3);
    packer_->pack(1);
    packer_->pack_indefinite_array();
    packer_->pack(2);
    packer_->pack(3);
    packer_->pack_indefinite_break();
    packer_->pack_array(2);
    packer_->pack(4);
    packer_->pack(5);
    ASSERT_EQ(std::string("\x83\x01\x9f\x02\x03\xff\x82\x04\x05", 9), os_.str());
}

TEST_F(TestPacker, indef8) {
    packer_->pack_indefinite_array();
    for (auto i = 1; i <= 25; i++)
        packer_->pack(i);
    packer_->pack_indefinite_break();
    ASSERT_EQ(std::string(
                      "\x9f\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x18\x18\x19\xff",
                              29), os_.str());
}

TEST_F(TestPacker, indef9) {
    packer_->pack_indefinite_map();
    packer_->pack("a");
    packer_->pack(1);
    packer_->pack("b");
    packer_->pack_indefinite_array();
    packer_->pack(2);
    packer_->pack(3);
    packer_->pack_indefinite_break();
    packer_->pack_indefinite_break();
    ASSERT_EQ(std::string("\xbf\x61\x61\x01\x61\x62\x9f\x02\x03\xff\xff", 11), os_.str());
}

TEST_F(TestPacker, indef10) {
    packer_->pack_array(2);
    packer_->pack("a");
    packer_->pack_indefinite_map();
    packer_->pack("b");
    packer_->pack("c");
    packer_->pack_indefinite_break();
    ASSERT_EQ(std::string("\x82\x61\x61\xbf\x61\x62\x61\x63\xff", 9), os_.str());
}

TEST_F(TestPacker, indef11) {
    packer_->pack_indefinite_map();
    packer_->pack("Fun");
    packer_->pack(true);
    packer_->pack("Amt");
    packer_->pack(-2);
    packer_->pack_indefinite_break();
    ASSERT_EQ(std::string("\xbf\x63\x46\x75\x6e\xf5\x63\x41\x6d\x74\x21\xff", 12), os_.str());
}
