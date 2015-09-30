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
#include <cborpp/packer.h>
#include <cborpp/unpacker.h>
#include <cborpp/debug_printer.h>
#include <gtest/gtest.h>

#include "testdata.h"

class TestObjectPrinter : public ::testing::Test {
protected:
	void SetUp(){
		object_ = new cborpp::object();
		unpacker_ = new cborpp::unpacker(*object_);
	}

	void TearDown(){
		delete object_;
	}

	void unpack(const std::string& packed_str) {
		unpacker_->add_bytes(packed_str.data(), packed_str.size());
		ASSERT_TRUE(false) << cborpp::debug_string(*object_);
	}

	cborpp::object* object_;
	cborpp::unpacker* unpacker_;
};
/*
TEST_F(TestObjectPrinter, int1){
	unpack(int1);
}
TEST_F(TestObjectPrinter, int2){
	unpack(int2);
}
TEST_F(TestObjectPrinter, int3){
	unpack(int3);
}
TEST_F(TestObjectPrinter, int4){
	unpack(int4);
}
TEST_F(TestObjectPrinter, int5){
	unpack(int5);
}
TEST_F(TestObjectPrinter, int6){
	unpack(int6);
}
TEST_F(TestObjectPrinter, int7){
	unpack(int7);
}
TEST_F(TestObjectPrinter, int8){
	unpack(int8);
}
TEST_F(TestObjectPrinter, int9){
	unpack(int9);
}
TEST_F(TestObjectPrinter, int10){
	unpack(int10);
}
TEST_F(TestObjectPrinter, int11){
	unpack(int11);
}
TEST_F(TestObjectPrinter, bigint1){
	unpack(bigint1);
}
TEST_F(TestObjectPrinter, neg_bigint1){
	unpack(neg_bigint1);
}
TEST_F(TestObjectPrinter, neg_bigint2){
	unpack(neg_bigint2);
}
TEST_F(TestObjectPrinter, neg_int1){
	unpack(neg_int1);
}
TEST_F(TestObjectPrinter, neg_int2){
	unpack(neg_int2);
}
TEST_F(TestObjectPrinter, neg_int3){
	unpack(neg_int3);
}
TEST_F(TestObjectPrinter, neg_int4){
	unpack(neg_int4);
}
TEST_F(TestObjectPrinter, float1){
	unpack(float1);
}
TEST_F(TestObjectPrinter, float2){
	unpack(float2);
}
TEST_F(TestObjectPrinter, float3){
	unpack(float3);
}
TEST_F(TestObjectPrinter, float4){
	unpack(float4);
}
TEST_F(TestObjectPrinter, float5){
	unpack(float5);
}
TEST_F(TestObjectPrinter, float6){
	unpack(float6);
}
TEST_F(TestObjectPrinter, float7){
	unpack(float7);
}
TEST_F(TestObjectPrinter, float8){
	unpack(float8);
}
TEST_F(TestObjectPrinter, float9){
	unpack(float9);
}
TEST_F(TestObjectPrinter, float10){
	unpack(float10);
}
TEST_F(TestObjectPrinter, float11){
	unpack(float11);
}
TEST_F(TestObjectPrinter, float12){
	unpack(float12);
}
TEST_F(TestObjectPrinter, float13){
	unpack(float13);
}
TEST_F(TestObjectPrinter, half_limits1){
	unpack(half_limits1);
}
TEST_F(TestObjectPrinter, half_limits2){
	unpack(half_limits2);
}
TEST_F(TestObjectPrinter, half_limits3){
	unpack(half_limits3);
}
TEST_F(TestObjectPrinter, float_limits1){
	unpack(half_limits1);
}
TEST_F(TestObjectPrinter, float_limits2){
	unpack(half_limits2);
}
TEST_F(TestObjectPrinter, float_limits3){
	unpack(half_limits3);
}
TEST_F(TestObjectPrinter, double_limits1){
	unpack(half_limits1);
}
TEST_F(TestObjectPrinter, double_limits2){
	unpack(half_limits2);
}
TEST_F(TestObjectPrinter, double_limits3){
	unpack(half_limits3);
}
TEST_F(TestObjectPrinter, simple1){
	unpack(simple1);
}
TEST_F(TestObjectPrinter, simple2){
	unpack(simple2);
}
TEST_F(TestObjectPrinter, simple3){
	unpack(simple3);
}
TEST_F(TestObjectPrinter, simple4){
	unpack(simple4);
}
TEST_F(TestObjectPrinter, simple5){
	unpack(simple5);
}
TEST_F(TestObjectPrinter, simple6){
	unpack(simple6);
}
TEST_F(TestObjectPrinter, simple7){
	unpack(simple7);
}
TEST_F(TestObjectPrinter, tag1){
	unpack(tag1);
}
TEST_F(TestObjectPrinter, tag2){
	unpack(tag2);
}
TEST_F(TestObjectPrinter, tag3){
	unpack(tag3);
}
TEST_F(TestObjectPrinter, tag4){
	unpack(tag4);
}
TEST_F(TestObjectPrinter, tag5){
	unpack(tag5);
}
TEST_F(TestObjectPrinter, tag6){
	unpack(tag6);
}
TEST_F(TestObjectPrinter, bin_string1){
	unpack(bin_string1);
}
TEST_F(TestObjectPrinter, bin_string2){
	unpack(bin_string2);
}
TEST_F(TestObjectPrinter, string1){
	unpack(string1);
}
TEST_F(TestObjectPrinter, string2){
	unpack(string2);
}
TEST_F(TestObjectPrinter, string3){
	unpack(string3);
}
TEST_F(TestObjectPrinter, string4){
	unpack(string4);
}
TEST_F(TestObjectPrinter, string5){
	unpack(string5);
}
TEST_F(TestObjectPrinter, string6){
	unpack(string6);
}
TEST_F(TestObjectPrinter, string7){
	unpack(string7);
}
TEST_F(TestObjectPrinter, array1){
	unpack(array1);
}
TEST_F(TestObjectPrinter, array2){
	unpack(array2);
}
TEST_F(TestObjectPrinter, array3){
	unpack(array3);
}
TEST_F(TestObjectPrinter, array4){
	unpack(array4);
}
TEST_F(TestObjectPrinter, map1){
	unpack(map1);
}
TEST_F(TestObjectPrinter, map2){
	unpack(map2);
}
TEST_F(TestObjectPrinter, map3){
	unpack(map3);
}
TEST_F(TestObjectPrinter, map4){
	unpack(map4);
}
TEST_F(TestObjectPrinter, map5){
	unpack(map5);
}*/
TEST_F(TestObjectPrinter, DISABLED_indef1){
	unpack(indef1);
}
TEST_F(TestObjectPrinter, DISABLED_indef2){
	unpack(indef2);
}
TEST_F(TestObjectPrinter, DISABLED_indef3){
	unpack(indef3);
}
TEST_F(TestObjectPrinter, DISABLED_indef4){
	unpack(indef4);
}
TEST_F(TestObjectPrinter, DISABLED_indef5){
	unpack(indef5);
}
TEST_F(TestObjectPrinter, DISABLED_indef6){
	unpack(indef6);
}
TEST_F(TestObjectPrinter, DISABLED_indef7){
	unpack(indef7);
}
TEST_F(TestObjectPrinter, DISABLED_indef8){
	unpack(indef8);
}
TEST_F(TestObjectPrinter, DISABLED_indef9){
	unpack(indef9);
}
TEST_F(TestObjectPrinter, DISABLED_indef10){
	unpack(indef10);
}
TEST_F(TestObjectPrinter, DISABLED_indef11){
	unpack(indef11);
}
