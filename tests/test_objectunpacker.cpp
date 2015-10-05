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
#include <unicbor/object.h>
#include <unicbor/packer.h>
#include <unicbor/unpacker.h>
#include <gtest/gtest.h>

#include "testdata.h"

class TestObjectUnpacker : public ::testing::Test {
protected:
	void SetUp(){
		object_ = new unicbor::object();
		unpacker_ = new unicbor::unpacker(*object_);
	}

	void TearDown(){
		delete object_;
	}

	void unpack(const std::string& packed_str) {
		unpacker_->add_bytes(packed_str.data(), packed_str.size());
	}

	unicbor::object* object_;
	unicbor::unpacker* unpacker_;
};

TEST_F(TestObjectUnpacker, int1){
	unpack(int1);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::UINT);
	ASSERT_EQ(object_->as<uint64_t>(), 0);
}
TEST_F(TestObjectUnpacker, int2){
	unpack(int2);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::UINT);
	ASSERT_EQ(object_->as<uint64_t>(), 1);
}
TEST_F(TestObjectUnpacker, int3){
	unpack(int3);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::UINT);
	ASSERT_EQ(object_->as<uint64_t>(), 10);
}

TEST_F(TestObjectUnpacker, int4){
	unpack(int4);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::UINT);
	ASSERT_EQ(object_->as<uint64_t>(), 23);
}

TEST_F(TestObjectUnpacker, int5){
	unpack(int5);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::UINT);
	ASSERT_EQ(object_->as<uint64_t>(), 24);
}

TEST_F(TestObjectUnpacker, int6){
	unpack(int6);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::UINT);
	ASSERT_EQ(object_->as<uint64_t>(), 25);
}

TEST_F(TestObjectUnpacker, int7){
	unpack(int7);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::UINT);
	ASSERT_EQ(object_->as<uint64_t>(), 100);
}

TEST_F(TestObjectUnpacker, int8){
	unpack(int8);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::UINT);
	ASSERT_EQ(object_->as<uint64_t>(), 1000);
}

TEST_F(TestObjectUnpacker, int9){
	unpack(int9);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::UINT);
	ASSERT_EQ(object_->as<uint64_t>(), 1000000ul);
}

TEST_F(TestObjectUnpacker, int10){
	unpack(int10);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::UINT);
	ASSERT_EQ(object_->as<uint64_t>(), 1000000000000ull);
}

TEST_F(TestObjectUnpacker, int11){
	unpack(int11);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::UINT);
	ASSERT_EQ(object_->as<uint64_t>(), 18446744073709551615ull);
}

TEST_F(TestObjectUnpacker, bigint1){
	unpack(bigint1);
	ASSERT_EQ(object_->tags(), std::vector<unicbor::tag>({unicbor::tag::BIGNUM_POSITIVE}));
	ASSERT_EQ(object_->get_type(), unicbor::object::type::BYTE_STRING);
	ASSERT_EQ(object_->as<std::string>(), std::string("\x01\0\0\0\0\0\0\0\0", 9));
}

TEST_F(TestObjectUnpacker, neg_bigint1){
	unpack(neg_bigint1);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::NEGATIVE_INT);
	ASSERT_EQ(object_->as<uint64_t>(), 18446744073709551615ull);
}

TEST_F(TestObjectUnpacker, neg_bigint2){
	unpack(neg_bigint2);
	ASSERT_EQ(object_->tags(), std::vector<unicbor::tag>({unicbor::tag::BIGNUM_NEGATIVE}));
	ASSERT_EQ(object_->get_type(), unicbor::object::type::BYTE_STRING);
	ASSERT_EQ(object_->as<std::string>(), std::string("\x01\0\0\0\0\0\0\0\0", 9));
}

TEST_F(TestObjectUnpacker, neg_int1){
	unpack(neg_int1);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::NEGATIVE_INT);
	ASSERT_EQ(object_->as<int64_t>(), -1);
}

TEST_F(TestObjectUnpacker, neg_int2){
	unpack(neg_int2);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::NEGATIVE_INT);
	ASSERT_EQ(object_->as<int64_t>(), -10);
}

TEST_F(TestObjectUnpacker, neg_int3){
	unpack(neg_int3);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::NEGATIVE_INT);
	ASSERT_EQ(object_->as<int64_t>(), -100);
}

TEST_F(TestObjectUnpacker, neg_int4){
	unpack(neg_int4);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::NEGATIVE_INT);
	ASSERT_EQ(-1000, object_->as<int64_t>());
}

TEST_F(TestObjectUnpacker, float1){
	unpack(float1);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::FLOAT);
	ASSERT_EQ(object_->as<double>(), 0.0);
}

TEST_F(TestObjectUnpacker, float2){
	unpack(float2);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::FLOAT);
	ASSERT_EQ(object_->as<double>(), -0.0);
}

TEST_F(TestObjectUnpacker, float3){
	unpack(float3);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::FLOAT);
	ASSERT_EQ(object_->as<double>(), 1.0);
}

TEST_F(TestObjectUnpacker, float4){
	unpack(float4);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::FLOAT);
	ASSERT_EQ(object_->as<double>(), 1.1);
}

TEST_F(TestObjectUnpacker, float5){
	unpack(float5);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::FLOAT);
	ASSERT_EQ(object_->as<double>(), 1.5);
}

TEST_F(TestObjectUnpacker, float6){
	unpack(float6);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::FLOAT);
	ASSERT_EQ(object_->as<double>(), 65504.0);
}

TEST_F(TestObjectUnpacker, float7){
	unpack(float7);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::FLOAT);
	ASSERT_EQ(object_->as<double>(), 100000.0);
}

TEST_F(TestObjectUnpacker, float8){
	unpack(float8);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::FLOAT);
	ASSERT_EQ(object_->as<double>(), 3.4028234663852886e+38);
}

TEST_F(TestObjectUnpacker, float9){
	unpack(float9);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::FLOAT);
	ASSERT_EQ(object_->as<double>(), 1.0e+300);
}

TEST_F(TestObjectUnpacker, float10){
	unpack(float10);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::FLOAT);
	ASSERT_EQ(object_->as<double>(), 5.960464477539063e-8);
}

TEST_F(TestObjectUnpacker, float11){
	unpack(float11);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::FLOAT);
	ASSERT_EQ(object_->as<double>(), 0.00006103515625);
}

TEST_F(TestObjectUnpacker, float12){
	unpack(float12);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::FLOAT);
	ASSERT_EQ(object_->as<double>(), -4.0);
}

TEST_F(TestObjectUnpacker, float13){
	unpack(float13);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::FLOAT);
	ASSERT_EQ(object_->as<double>(), -4.1);
}

TEST_F(TestObjectUnpacker, half_limits1){
	unpack(half_limits1);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::FLOAT);
	ASSERT_EQ(object_->as<double>(), INFINITY);
}

TEST_F(TestObjectUnpacker, half_limits2){
	unpack(half_limits2);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::FLOAT);
	ASSERT_TRUE(isnan(object_->as<double>()));
}

TEST_F(TestObjectUnpacker, half_limits3){
	unpack(half_limits3);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::FLOAT);
	ASSERT_EQ(object_->as<double>(), -INFINITY);
}

TEST_F(TestObjectUnpacker, float_limits1){
	unpack(half_limits1);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::FLOAT);
	ASSERT_EQ(object_->as<double>(), INFINITY);
}

TEST_F(TestObjectUnpacker, float_limits2){
	unpack(half_limits2);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::FLOAT);
	ASSERT_TRUE(isnan(object_->as<double>()));
}

TEST_F(TestObjectUnpacker, float_limits3){
	unpack(half_limits3);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::FLOAT);
	ASSERT_EQ(object_->as<double>(), -INFINITY);
}

TEST_F(TestObjectUnpacker, double_limits1){
	unpack(half_limits1);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::FLOAT);
	ASSERT_EQ(object_->as<double>(), INFINITY);
}

TEST_F(TestObjectUnpacker, double_limits2){
	unpack(half_limits2);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::FLOAT);
	ASSERT_TRUE(isnan(object_->as<double>()));
}

TEST_F(TestObjectUnpacker, double_limits3){
	unpack(half_limits3);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::FLOAT);
	ASSERT_EQ(object_->as<double>(), -INFINITY);
}

TEST_F(TestObjectUnpacker, simple1){
	unpack(simple1);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::BOOL);
	ASSERT_FALSE(object_->as<bool>());
}

TEST_F(TestObjectUnpacker, simple2){
	unpack(simple2);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::BOOL);
	ASSERT_TRUE(object_->as<bool>());
}

TEST_F(TestObjectUnpacker, simple3){
	unpack(simple3);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::NULL_VALUE);
}

TEST_F(TestObjectUnpacker, simple4){
	unpack(simple4);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::UNDEFINED);
}

TEST_F(TestObjectUnpacker, simple5){
	unpack(simple5);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::SIMPLE);
	ASSERT_EQ(object_->as<uint8_t>(), 16);
}

TEST_F(TestObjectUnpacker, simple6){
	unpack(simple6);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::SIMPLE);
	ASSERT_EQ(object_->as<uint8_t>(), 24);
}

TEST_F(TestObjectUnpacker, simple7){
	unpack(simple7);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::SIMPLE);
	ASSERT_EQ(object_->as<uint8_t>(), 255);
}

TEST_F(TestObjectUnpacker, tag1){
	unpack(tag1);
	ASSERT_EQ(object_->tags(), std::vector<unicbor::tag>({unicbor::tag::DATETIME_STRING}));
	ASSERT_EQ(object_->get_type(), unicbor::object::type::TEXT_STRING);
	ASSERT_EQ(object_->as<std::string>(), "2013-03-21T20:04:00Z");
}

TEST_F(TestObjectUnpacker, tag2){
	unpack(tag2);
	ASSERT_EQ(object_->tags(), std::vector<unicbor::tag>({unicbor::tag::DATETIME_NUMERIC}));
	ASSERT_EQ(object_->get_type(), unicbor::object::type::UINT);
	ASSERT_EQ(object_->as<uint64_t>(), 1363896240);
}

TEST_F(TestObjectUnpacker, tag3){
	unpack(tag3);
	ASSERT_EQ(object_->tags(), std::vector<unicbor::tag>({unicbor::tag::DATETIME_NUMERIC}));
	ASSERT_EQ(object_->get_type(), unicbor::object::type::FLOAT);
	ASSERT_EQ(object_->as<double>(), 1363896240.5);
}

TEST_F(TestObjectUnpacker, tag4){
	unpack(tag4);
	ASSERT_EQ(object_->tags(), std::vector<unicbor::tag>({unicbor::tag::EXPECT_HEX}));
	ASSERT_EQ(object_->get_type(), unicbor::object::type::BYTE_STRING);
	ASSERT_EQ(object_->as<std::string>(), "\x01\x02\x03\x04");
}

TEST_F(TestObjectUnpacker, tag5){
	unpack(tag5);
	ASSERT_EQ(object_->tags(), std::vector<unicbor::tag>({unicbor::tag::ENCODED_CBOR}));
	ASSERT_EQ(object_->get_type(), unicbor::object::type::BYTE_STRING);
	ASSERT_EQ(object_->as<std::string>(), "\x64\x49\x45\x54\x46");
}

TEST_F(TestObjectUnpacker, tag6){
	unpack(tag6);
	ASSERT_EQ(object_->tags(), std::vector<unicbor::tag>({unicbor::tag::URI}));
	ASSERT_EQ(object_->get_type(), unicbor::object::type::TEXT_STRING);
	ASSERT_EQ(object_->as<std::string>(), "http://www.example.com");
}

TEST_F(TestObjectUnpacker, bin_string1){
	unpack(bin_string1);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::BYTE_STRING);
	ASSERT_EQ(object_->as<std::string>(), "");
}

TEST_F(TestObjectUnpacker, bin_string2){
	unpack(bin_string2);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::BYTE_STRING);
	ASSERT_EQ(object_->as<std::string>(), "\x01\x02\x03\x04");
}

TEST_F(TestObjectUnpacker, string1){
	unpack(string1);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::TEXT_STRING);
	ASSERT_EQ(object_->as<std::string>(), "");
}

TEST_F(TestObjectUnpacker, string2){
	unpack(string2);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::TEXT_STRING);
	ASSERT_EQ(object_->as<std::string>(), "a");
}

TEST_F(TestObjectUnpacker, string3){
	unpack(string3);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::TEXT_STRING);
	ASSERT_EQ(object_->as<std::string>(), "IETF");
}

TEST_F(TestObjectUnpacker, string4){
	unpack(string4);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::TEXT_STRING);
	ASSERT_EQ(object_->as<std::string>(), "\"\\");
}

TEST_F(TestObjectUnpacker, string5){
	unpack(string5);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::TEXT_STRING);
	ASSERT_EQ(object_->as<std::string>(), "\u00fc");
}

TEST_F(TestObjectUnpacker, string6){
	unpack(string6);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::TEXT_STRING);
	ASSERT_EQ(object_->as<std::string>(), "\u6c34");
}

TEST_F(TestObjectUnpacker, string7){
	unpack(string7);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::TEXT_STRING);
	ASSERT_EQ(object_->as<std::string>(), "\xf0\x90\x85\x91");
}

TEST_F(TestObjectUnpacker, array1){
	unpack(array1);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::ARRAY);
	ASSERT_EQ(object_->items().size(), 0);
}

TEST_F(TestObjectUnpacker, array2){
	unpack(array2);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::ARRAY);
	ASSERT_EQ(object_->items().size(), 3);
	ASSERT_EQ(object_->as<std::vector<int8_t>>(), std::vector<int8_t>({1, 2, 3}));
}

TEST_F(TestObjectUnpacker, array3){
	unpack(array3);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::ARRAY);
	ASSERT_EQ(object_->items().size(), 3);
	ASSERT_EQ(object_->items().at(0).as<int8_t>(), 1);
	ASSERT_EQ(object_->items().at(1).as<std::vector<int8_t>>(), std::vector<int8_t>({2, 3}));
	ASSERT_EQ(object_->items().at(2).as<std::vector<int8_t>>(), std::vector<int8_t>({4, 5}));
}

TEST_F(TestObjectUnpacker, array4){
	unpack(array4);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::ARRAY);
	ASSERT_EQ(object_->items().size(), 25);
	ASSERT_EQ(object_->as<std::vector<int8_t>>(), std::vector<int8_t>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25}));
}

TEST_F(TestObjectUnpacker, map1){
	unpack(map1);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::MAP);
	ASSERT_EQ(object_->items().size(), 0);
	auto actual = object_->as<std::map<uint8_t, uint8_t>>();
	auto expected = std::map<uint8_t, uint8_t>();
	ASSERT_EQ(actual, expected);
}

TEST_F(TestObjectUnpacker, map2){
	unpack(map2);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::MAP);
	ASSERT_EQ(object_->items().size(), 4);
	auto actual = object_->as<std::map<uint8_t, uint8_t>>();
	auto expected = std::map<uint8_t, uint8_t>({{1, 2},{3, 4}});
	ASSERT_EQ(actual, expected);
}

TEST_F(TestObjectUnpacker, map3){
	unpack(map3);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::MAP);
	ASSERT_EQ(object_->items().size(), 4);

	ASSERT_EQ(object_->items().at(0).as<std::string>(), "a");
	ASSERT_EQ(object_->items().at(1).as<int>(), 1);
	ASSERT_EQ(object_->items().at(2).as<std::string>(), "b");
	ASSERT_EQ(object_->items().at(3).as<std::vector<int8_t>>(), std::vector<int8_t>({2, 3}));
}

TEST_F(TestObjectUnpacker, map4){
	unpack(map4);
	ASSERT_EQ(object_->get_type(), unicbor::object::type::ARRAY);
	ASSERT_EQ(object_->items().size(), 2);

	ASSERT_EQ(object_->items().at(0).as<std::string>(), "a");
	ASSERT_EQ(object_->items().at(1).items().at(0).as<std::string>(), "b");
	ASSERT_EQ(object_->items().at(1).items().at(1).as<std::string>(), "c");
}

TEST_F(TestObjectUnpacker, map5){
	unpack(map5);

	std::map<std::string, std::string> actual = object_->as<std::map<std::string, std::string>>();
	std::map<std::string, std::string> expected({
													{"a", "A"},
													{"b", "B"},
													{"c", "C"},
													{"d", "D"},
													{"e", "E"}
												});

	ASSERT_EQ(object_->get_type(), unicbor::object::type::MAP);
	ASSERT_EQ(object_->items().size(), 10);
	ASSERT_EQ(actual, expected);
}

TEST_F(TestObjectUnpacker, DISABLED_indef1){
	unpack(indef1);

	ASSERT_EQ(object_->get_type(), unicbor::object::type::ARRAY);
	ASSERT_EQ(object_->payload().size(), 5);
	ASSERT_EQ(object_->as<std::string>(), "\x01\x02\x03\x04\x05");
}

/*
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

	unicbor::object& nested = object_->add_element();
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

	unicbor::object& nested = object_->add_element();
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

	unicbor::object& nested = object_->add_element();
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

	unicbor::object& nested = object_->add_element();
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

	unicbor::object& nested = object_->add_element();
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