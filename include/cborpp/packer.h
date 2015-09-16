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
#pragma once

#include "pch.h"
#include "constants.h"

namespace cborpp {

template<class Buffer>
class packer {
public:
	enum pack_method {
		AUTO=0,	/// Most efficent encoding. Used as standard encoding for CBOR recommended by RFC 7049 §2.1.
		EXACT=1	/// Inefficent encoding. Doesn't use additional bytes 0b000xxxxx, forcing all CBOR items to be 2 bytes minimum. Not recommended by standard, so may be incompatible with other decoders.
	};

	packer(Buffer& buffer) : buffer_(buffer) {}
	virtual ~packer() {}

	template<class Integer>
	void pack_init(Integer value, type_mask major_type_mask, pack_method method = pack_method::AUTO) {
		static_assert(std::is_integral<Integer>::value && !std::is_signed<Integer>::value, "Unsigned integral type expected");
		if(method == pack_method::AUTO) {
			if (value <= 23)
				pack_init_compact(value, major_type_mask);
			else if (value <= 0xff)
				pack_init_exact<uint8_t>((uint8_t) value, major_type_mask);
			else if (value <= 0xffff)
				pack_init_exact<uint16_t>((uint16_t) value, major_type_mask);
			else if (value <= 0xffffffff)
				pack_init_exact<uint32_t>((uint32_t) value, major_type_mask);
			else if (value <= 0xffffffffffffffff)
				pack_init_exact<uint64_t>((uint64_t) value, major_type_mask);
		}else if(method == pack_method::EXACT) {
			if(sizeof(Integer) == 1)
				pack_init_exact<uint8_t>((uint8_t) value, major_type_mask);
			else if (sizeof(Integer) == 2)
				pack_init_exact<uint16_t>((uint16_t) value, major_type_mask);
			else if (sizeof(Integer) == 4)
				pack_init_exact<uint32_t>((uint32_t) value, major_type_mask);
			else if (sizeof(Integer) == 8)
				pack_init_exact<uint64_t>((uint64_t) value, major_type_mask);
		}
	}

	void pack_special(uint8_t value) {
		buffer_.write((char*)&value, sizeof(value));
	}

	// Unsigned integer (uint8_t, uint16_t, uint32_t, uint64_t)
	template<class Integer>
	void pack_uint(Integer value, pack_method method = pack_method::AUTO) {
		pack_init(value, type_mask::UINT, method);
	}

	// Signed integer (int8_t, int16_t, int32_t, int64_t)
	template<class Integer>
	void pack_int(Integer value, pack_method method = pack_method::AUTO) {
		if(value >= 0)
			pack_init(static_cast<typename std::make_unsigned<Integer>::type>(value), type_mask::UINT, method);
		else
			pack_init(static_cast<typename std::make_unsigned<Integer>::type>(std::abs(value+1)), type_mask::NEGATIVE_INT, method);
	}

	// Binary string (aka Binary Large OBject aka BLOB)
	void pack_bin(const void* data, size_t size) {
		pack_init(size, type_mask::BYTE_STRING);
		buffer_.write((char*)data, size);
	}

	void pack_bin(const std::vector<uint8_t>& data) {
		pack_bin(data.data(), data.size());
	}

	// UTF-8 string
	void pack_string(const char* data, size_t size) {
		pack_init(size, type_mask::TEXT_STRING);
		buffer_.write(data, size);
	}

	void pack_string(const std::string& data) {
		pack_init(data.size(), type_mask::TEXT_STRING);
		buffer_.write(data.data(), data.size());
	}

	// Well, null-terminated strings are bad by design, but as general-purpose library we should support them.
	void pack_string(const char* data) {
		size_t size = strlen(data);
		pack_init(size, type_mask::TEXT_STRING);
		buffer_.write(data, size);
	}

	// Array
	void pack_array(size_t size) {
		pack_init(size, type_mask::ARRAY);
	}

	// Map
	void pack_map(size_t size) {
		pack_init(size, type_mask::MAP);
	}

	// Tag
	void pack_tag(tag value) {
		pack_init((uint64_t) value, type_mask::TAG);
	}

	void pack_simple(simple_value value) {
		pack_init((uint8_t)value, type_mask::FLOAT);
	}

	// Boolean
	void pack_bool(bool value) {
		pack_simple(value ? simple_value::TRUE_VALUE : simple_value::FALSE_VALUE);
	}

	// Null value
	void pack_null() {
		pack_simple(simple_value::NULL_VALUE);
	}

	// Undefined value
	void pack_undefined() {
		pack_simple(simple_value::UNDEFINED);
	}

	// Floating-point number
	// C library doesn't have half-precision format, so we accept pointer to half-precision float variable. Endianess: native.
	void pack_half(half value) {
		pack_init(*(reinterpret_cast<uint16_t*>(&value)), type_mask::FLOAT, pack_method::EXACT);
	}

	void pack_float(float value) {
		pack_init(*(reinterpret_cast<uint32_t*>(&value)), type_mask::FLOAT, pack_method::EXACT);
	}

	void pack_double(double value) {
		pack_init(*(reinterpret_cast<uint64_t*>(&value)), type_mask::FLOAT, pack_method::EXACT);
	}

	template<class FPValue>
	void pack_fp(FPValue value) {
		if(value == half(value))
			pack_half(half(value));
		else if(value == float(value))
			pack_float(float(value));
		else
			pack_double(value);
	}

	// Indefinite elements support. Well, it is called 'Streaming' in RFC. And in this implementation it ACTUALLY streams to std::ostream.
	void pack_indefinite_bin() {pack_special((uint8_t)indefinite_marker::INDEFINITE_BYTE_STRING);}
	void pack_indefinite_string() {pack_special((uint8_t)indefinite_marker::INDEFINITE_TEXT_STRING);}
	void pack_indefinite_array() {pack_special((uint8_t)indefinite_marker::INDEFINITE_ARRAY);}
	void pack_indefinite_map() {pack_special((uint8_t)indefinite_marker::INDEFINITE_MAP);}
	void pack_indefinite_break() {pack_special((uint8_t)indefinite_marker::BREAK_CODE);}

	// Syntax sugar
	void pack(const void* data, size_t size){pack_bin(data, size);}
	void pack(const std::vector<uint8_t>& data){pack_bin(data);}
	void pack(const char* data, size_t size){pack_string(data, size);}
	void pack(const std::string& data){pack_string(data);}
	void pack(const char* data){pack_string(data);}
	void pack(tag value){pack_tag(value);}
	void pack(simple_value value){pack_simple(value);}
	void pack(bool value){pack_bool(value);}
	void pack(nullptr_t){pack_null();}
	void pack(undefined_t){pack_undefined();}

	template<typename X, typename Y>
	void pack(std::pair<X, Y> value){pack(value.first); pack(value.second);}

	template<typename X, typename Y>
	void pack(const std::map<X, Y>& map) {
		pack_map(map.size());
		for(auto& pair : map){
			pack(pair);
		}
	};

	template<class FPValue>
	typename std::enable_if<std::numeric_limits<FPValue>::has_infinity>::type pack(FPValue value) {pack_fp(value);}

	template<class Integer>
	typename std::enable_if<std::is_integral<Integer>::value>::type pack(Integer value) {pack_int(value);}

private:
	Buffer& buffer_;

	void pack_init_compact(uint8_t value, type_mask major_type_mask) {
		value = (uint8_t) major_type_mask | uint8_t(value & 0b00011111);
		buffer_.write((char*)&value, sizeof(value));
	}

	template<class Integer>
	void pack_init_exact(Integer value, type_mask mask) {
		static_assert(std::is_integral<Integer>::value && !std::is_signed<Integer>::value, "Unsigned integral type expected");
		static_assert(sizeof(Integer) <= sizeof(uint64_t), "Integral type of 1, 2, 4 or 8 bytes expected");

		boost::endian::endian_arithmetic<boost::endian::order::big, Integer, sizeof(Integer)*8, boost::endian::align::yes> value_be = value;
		std::array<uint8_t, 1+sizeof(Integer)> packed;

		uint8_t size_mask;
		switch(sizeof(Integer)) {
			case 1:
				size_mask = 24;
				break;
			case 2:
				size_mask = 25;
				break;
			case 4:
				size_mask = 26;
				break;
			case 8:
				size_mask = 27;
				break;
		}

		packed[0] = uint8_t(mask) | size_mask;
		std::copy((uint8_t*)&value_be, (uint8_t*)&value_be+sizeof(Integer), packed.data()+1);

		buffer_.write((char*)packed.data(), packed.size());
	}
};

} /* namespace cborpp */
