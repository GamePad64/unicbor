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
#include <cborpp/object.h>
#include <gtest/gtest.h>

#include "testdata.h"

class TestObjectUnpacker : public ::testing::Test {
protected:
	void SetUp(){
		object_ = new cborpp::object();
	}

	void TearDown(){
		delete object_;
	}

	cborpp::object* object_;
};

TEST_F(TestObjectUnpacker, int1){
	object_->add_bytes(int1.data(), int1.size());
	ASSERT_EQ(0, object_->as_uint64());
}
TEST_F(TestObjectUnpacker, int2){
	object_->add_bytes(int2.data(), int2.size());
	ASSERT_EQ(1, object_->as_uint64());
}
TEST_F(TestObjectUnpacker, int3){
	object_->add_bytes(int3.data(), int3.size());
	ASSERT_EQ(10, object_->as_uint64());
}
/*
TEST_F(TestObjectUnpacker, int1){
	object_->set(0);
	ASSERT_EQ(int1, os_->str());
}

TEST_F(TestObjectUnpacker, int2){
	object_->set(1);
	ASSERT_EQ(int2, os_->str());
}

TEST_F(TestObjectUnpacker, int3){
	object_->set(10);
	ASSERT_EQ(int3, os_->str());
}

TEST_F(TestObjectUnpacker, int4){
	object_->set(23);
	ASSERT_EQ(int4, os_->str());
}

TEST_F(TestObjectUnpacker, int5){
	object_->set(24);
	ASSERT_EQ(int5, os_->str());
}

TEST_F(TestObjectUnpacker, int6){
	object_->set(25);
	ASSERT_EQ(int6, os_->str());
}

TEST_F(TestObjectUnpacker, int7){
	object_->set(100);
	ASSERT_EQ(int7, os_->str());
}

TEST_F(TestObjectUnpacker, int8){
	object_->set(1000);
	ASSERT_EQ(int8, os_->str());
}

TEST_F(TestObjectUnpacker, int9){
	object_->set(1000000ul);
	ASSERT_EQ(int9, os_->str());
}

TEST_F(TestObjectUnpacker, int10){
	object_->set(1000000000000ull);
	ASSERT_EQ(int10, os_->str());
}

TEST_F(TestObjectUnpacker, int11){
	object_->set(18446744073709551615ull);
	ASSERT_EQ(int11, os_->str());
}

TEST_F(TestObjectUnpacker, bigint1){
	object_->add_tag(cborpp::tag::BIGNUM_POSITIVE);
	object_->set_bin("\x01\0\0\0\0\0\0\0\0", 9);
	ASSERT_EQ(bigint1, os_->str());
}

TEST_F(TestObjectUnpacker, neg_bigint1){
	object_
		->set_neg_int(18446744073709551615ull);    // C and C++ don't have a type to hold -18446744073709551615, but this is not considered bigint in CBOR, so...
	ASSERT_EQ(neg_bigint1, os_->str());
}

TEST_F(TestObjectUnpacker, neg_bigint2){
	object_->add_tag(cborpp::tag::BIGNUM_NEGATIVE);
	object_->set_bin("\x01\0\0\0\0\0\0\0\0", 9);
	ASSERT_EQ(neg_bigint2, os_->str());
}

TEST_F(TestObjectUnpacker, neg_int1){
	object_->set(-1);
	ASSERT_EQ(neg_int1, os_->str());
}

TEST_F(TestObjectUnpacker, neg_int2){
	object_->set(-10);
	ASSERT_EQ(neg_int2, os_->str());
}

TEST_F(TestObjectUnpacker, neg_int3){
	object_->set(-100);
	ASSERT_EQ(neg_int3, os_->str());
}

TEST_F(TestObjectUnpacker, neg_int4){
	object_->set(-1000);
	ASSERT_EQ(neg_int4, os_->str());
}

TEST_F(TestObjectUnpacker, float1){
	object_->set(0.0);
	ASSERT_EQ(float1, os_->str());
}

TEST_F(TestObjectUnpacker, float2){
	object_->set(-0.0);
	ASSERT_EQ(float2, os_->str());
}

TEST_F(TestObjectUnpacker, float3){
	object_->set(1.0);
	ASSERT_EQ(float3, os_->str());
}

TEST_F(TestObjectUnpacker, float4){
	object_->set(1.1);
	ASSERT_EQ(float4, os_->str());
}

TEST_F(TestObjectUnpacker, float5){
	object_->set(1.5);
	ASSERT_EQ(float5, os_->str());
}

TEST_F(TestObjectUnpacker, float6){
	object_->set(65504.0);
	ASSERT_EQ(float6, os_->str());
}

TEST_F(TestObjectUnpacker, float7){
	object_->set(100000.0);
	ASSERT_EQ(float7, os_->str());
}

TEST_F(TestObjectUnpacker, float8){
	object_->set(3.4028234663852886e+38);
	ASSERT_EQ(float8, os_->str());
}

TEST_F(TestObjectUnpacker, float9){
	object_->set(1.0e+300);
	ASSERT_EQ(float9, os_->str());
}

TEST_F(TestObjectUnpacker, float10){
	object_->set(5.960464477539063e-8);
	ASSERT_EQ(float10, os_->str());
}

TEST_F(TestObjectUnpacker, float11){
	object_->set(0.00006103515625);
	ASSERT_EQ(float11, os_->str());
}

TEST_F(TestObjectUnpacker, float12){
	object_->set(-4.0);
	ASSERT_EQ(float12, os_->str());
}

TEST_F(TestObjectUnpacker, float13){
	object_->set(-4.1);
	ASSERT_EQ(float13, os_->str());
}

TEST_F(TestObjectUnpacker, half_limits1){
	object_->set_half(cborpp::half(INFINITY));
	ASSERT_EQ(half_limits1, os_->str());
}

TEST_F(TestObjectUnpacker, half_limits2){
	object_->set_half(cborpp::half(NAN));
	ASSERT_EQ(half_limits2, os_->str());
}

TEST_F(TestObjectUnpacker, half_limits3){
	object_->set_half(cborpp::half(-INFINITY));
	ASSERT_EQ(half_limits3, os_->str());
}

TEST_F(TestObjectUnpacker, float_limits1){
	object_->set_float(float(INFINITY));
	ASSERT_EQ(float_limits1, os_->str());
}

TEST_F(TestObjectUnpacker, float_limits2){
	object_->set_float(float(NAN));
	ASSERT_EQ(float_limits2, os_->str());
}

TEST_F(TestObjectUnpacker, float_limits3){
	object_->set_float(float(-INFINITY));
	ASSERT_EQ(float_limits3, os_->str());
}

TEST_F(TestObjectUnpacker, double_limits1){
	object_->set_double(double(INFINITY));
	ASSERT_EQ(double_limits1, os_->str());
}

TEST_F(TestObjectUnpacker, double_limits2){
	object_->set_double(double(NAN));
	ASSERT_EQ(double_limits2, os_->str());
}

TEST_F(TestObjectUnpacker, double_limits3){
	object_->set_double(double(-INFINITY));
	ASSERT_EQ(double_limits3, os_->str());
}

TEST_F(TestObjectUnpacker, simple1){
	object_->set(false);
	ASSERT_EQ(simple1, os_->str());
}

TEST_F(TestObjectUnpacker, simple2){
	object_->set(true);
	ASSERT_EQ(simple2, os_->str());
}

TEST_F(TestObjectUnpacker, simple3){
	object_->set(nullptr);
	ASSERT_EQ(simple3, os_->str());
}

TEST_F(TestObjectUnpacker, simple4){
	object_->set(cborpp::undefined);
	ASSERT_EQ(simple4, os_->str());
}

TEST_F(TestObjectUnpacker, simple5){
	object_->set(cborpp::simple_value(16));
	ASSERT_EQ(simple5, os_->str());
}

TEST_F(TestObjectUnpacker, simple6){
	object_->set(cborpp::simple_value(24));
	ASSERT_EQ(simple6, os_->str());
}

TEST_F(TestObjectUnpacker, simple7){
	object_->set(cborpp::simple_value(255));
	ASSERT_EQ(simple7, os_->str());
}

TEST_F(TestObjectUnpacker, tag1){
	object_->add_tag(cborpp::tag::DATETIME_STRING);
	object_->set("2013-03-21T20:04:00Z");
	ASSERT_EQ(tag1, os_->str());
}

TEST_F(TestObjectUnpacker, tag2){
	object_->add_tag(cborpp::tag::DATETIME_NUMERIC);
	object_->set(1363896240);
	ASSERT_EQ(tag2, os_->str());
}

TEST_F(TestObjectUnpacker, tag3){
	object_->add_tag(cborpp::tag::DATETIME_NUMERIC);
	object_->set(1363896240.5);
	ASSERT_EQ(tag3, os_->str());
}

TEST_F(TestObjectUnpacker, tag4){
	object_->add_tag(cborpp::tag::EXPECT_HEX);
	object_->set_bin("\x01\x02\x03\x04", 4);
	ASSERT_EQ(tag4, os_->str());
}

TEST_F(TestObjectUnpacker, tag5){
	object_->add_tag(cborpp::tag::ENCODED_CBOR);
	object_->set_bin("\x64\x49\x45\x54\x46", 5);
	ASSERT_EQ(tag5, os_->str());
}

TEST_F(TestObjectUnpacker, tag6){
	object_->add_tag(cborpp::tag::URI);
	object_->set("http://www.example.com");
	ASSERT_EQ(tag6, os_->str());
}

TEST_F(TestObjectUnpacker, bin_string1){
	object_->set_bin("", 0);
	ASSERT_EQ(bin_string1, os_->str());
}

TEST_F(TestObjectUnpacker, bin_string2){
	object_->set_bin("\x01\x02\x03\x04", 4);
	ASSERT_EQ(bin_string2, os_->str());
}

TEST_F(TestObjectUnpacker, string1){
	object_->set("");
	ASSERT_EQ(string1, os_->str());
}

TEST_F(TestObjectUnpacker, string2){
	object_->set("a");
	ASSERT_EQ(string2, os_->str());
}

TEST_F(TestObjectUnpacker, string3){
	object_->set("IETF");
	ASSERT_EQ(string3, os_->str());
}

TEST_F(TestObjectUnpacker, string4){
	object_->set("\"\\");
	ASSERT_EQ(string4, os_->str());
}

TEST_F(TestObjectUnpacker, string5){
	object_->set("\u00fc");
	ASSERT_EQ(string5, os_->str());
}

TEST_F(TestObjectUnpacker, string6){
	object_->set("\u6c34");
	ASSERT_EQ(string6, os_->str());
}

TEST_F(TestObjectUnpacker, string7){
	object_->set("\xf0\x90\x85\x91");
	ASSERT_EQ(string7, os_->str());
}

TEST_F(TestObjectUnpacker, array1){
	object_->set_array(std::vector<uint8_t>());    // Explicitly set_array, because std::vector<uint8_t> is BINARY_STRING type here.
	ASSERT_EQ(array1, os_->str());
}

TEST_F(TestObjectUnpacker, array2){
	//object_->set(std::vector<uint8_t>());	// std::vector<uint8_t> is BINARY_STRING type here.
	object_->set(std::vector<uint8_t>({1, 2, 3}));
	ASSERT_EQ(array2, os_->str());
}

TEST_F(TestObjectUnpacker, array3){
	object_->start_array(3);
	object_->add_element(1);
	object_->add_element(std::vector<uint8_t>({2, 3}));
	object_->add_element(std::vector<uint8_t>({4, 5}));
	ASSERT_EQ(array3, os_->str());
}

TEST_F(TestObjectUnpacker, array4){
	object_->set(std::vector<uint8_t>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25}));
	ASSERT_EQ(array4, os_->str());
}

TEST_F(TestObjectUnpacker, array4_for){
	object_->start_array(25);
	for(int i = 1; i <= 25; i++)
		object_->add_element(i);
	ASSERT_EQ(array4, os_->str());
}

TEST_F(TestObjectUnpacker, map1){
	object_->set(std::map<uint8_t, uint8_t>());
	ASSERT_EQ(map1, os_->str());
}

TEST_F(TestObjectUnpacker, map2){
	object_->set(std::map<int, int>({{1, 2},
									 {3, 4}}));
	ASSERT_EQ(map2, os_->str());
}

TEST_F(TestObjectUnpacker, map3){
	object_->start_map(2);
	object_->add_element(std::make_pair("a", 1));
	object_->add_element(std::make_pair("b", std::vector<uint8_t>({2, 3})));

	ASSERT_EQ(map3, os_->str());
}

TEST_F(TestObjectUnpacker, map4){
	object_->start_array(2);
	object_->add_element("a");
	object_->add_element(std::map<std::string, std::string>({{"b", "c"}}));

	ASSERT_EQ(map4, os_->str());
}

TEST_F(TestObjectUnpacker, map5){
	object_->set(std::map<std::string, std::string>({
														{"a", "A"},
														{"b", "B"},
														{"c", "C"},
														{"d", "D"},
														{"e", "E"}
													}));

	ASSERT_EQ(map5, os_->str());
}

TEST_F(TestObjectUnpacker, indef1){
	object_->start_bin();
	object_->add_element((const uint8_t*)"\x01\x02", 2);
	object_->add_element((const uint8_t*)"\x03\x04\x05", 3);
	object_->stop_bin();

	ASSERT_EQ(indef1, os_->str());
}

TEST_F(TestObjectUnpacker, indef2){
	object_->start_string();
	object_->add_element("strea");
	object_->add_element("ming");
	object_->stop_string();

	ASSERT_EQ(indef2, os_->str());
}

TEST_F(TestObjectUnpacker, indef3){
	object_->start_array();
	object_->stop_array();

	ASSERT_EQ(indef3, os_->str());
}

TEST_F(TestObjectUnpacker, indef4){
	object_->start_array();
	object_->add_element(1);
	object_->add_element(std::vector<uint8_t>({2, 3}));

	cborpp::object& nested = object_->add_element();
	nested.start_array();
	nested.add_element(4);
	nested.add_element(5);
	nested.stop_array();

	object_->stop_array();

	ASSERT_EQ(indef4, os_->str());
}

TEST_F(TestObjectUnpacker, indef5){
	object_->start_array();
	object_->add_element(1);
	object_->add_element(std::vector<uint8_t>({2, 3}));
	object_->add_element(std::vector<uint8_t>({4, 5}));
	object_->stop_array();

	ASSERT_EQ(indef5, os_->str());
}

TEST_F(TestObjectUnpacker, indef6){
	object_->start_array(3);
	object_->add_element(1);
	object_->add_element(std::vector<uint8_t>({2, 3}));

	cborpp::object& nested = object_->add_element();
	nested.start_array();
	nested.add_element(4);
	nested.add_element(5);
	ASSERT_TRUE(!object_->complete());
	nested.stop_array();
	ASSERT_TRUE(object_->complete());

	ASSERT_EQ(indef6, os_->str());
}

TEST_F(TestObjectUnpacker, indef7){
	object_->start_array(3);
	object_->add_element(1);

	cborpp::object& nested = object_->add_element();
	nested.start_array();
	nested.add_element(2);
	nested.add_element(3);
	ASSERT_TRUE(!object_->complete());
	nested.stop_array();
	ASSERT_TRUE(!object_->complete());

	object_->add_element(std::vector<uint8_t>({4, 5}));
	ASSERT_TRUE(object_->complete());

	ASSERT_EQ(indef7, os_->str());
}

TEST_F(TestObjectUnpacker, indef8){
	object_->start_array();
	for(int i = 1; i <= 25; i++)
		object_->add_element(i);

	object_->stop_array();

	ASSERT_EQ(indef8, os_->str());
}

TEST_F(TestObjectUnpacker, indef9){
	object_->start_map();
	object_->add_element(std::make_pair("a", 1));
	object_->add_element("b");

	cborpp::object& nested = object_->add_element();
	nested.start_array();
	nested.add_element(2);
	nested.add_element(3);
	nested.stop_array();

	object_->stop_map();

	ASSERT_EQ(indef9, os_->str());
}

TEST_F(TestObjectUnpacker, indef10){
	object_->start_array(2);
	object_->add_element("a");

	cborpp::object& nested = object_->add_element();
	nested.start_map();
	nested.add_element("b");
	nested.add_element("c");
	nested.stop_map();

	ASSERT_EQ(indef10, os_->str());
}

TEST_F(TestObjectUnpacker, indef11){
	object_->start_map();
	object_->add_element(std::make_pair("Fun", true));
	object_->add_element(std::make_pair("Amt", -2));
	object_->stop_map();

	ASSERT_EQ(indef11, os_->str());
}*/