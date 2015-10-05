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
#include "exceptions.h"
#include "adaptor/array.h"
#include "adaptor/bool.h"
#include "adaptor/char_ptr.h"
#include "adaptor/float.h"
#include "adaptor/int.h"
#include "adaptor/map.h"
#include "adaptor/nullptr.h"
#include "adaptor/object.h"
#include "adaptor/simple_value.h"
#include "adaptor/string.h"
#include "adaptor/undefined.h"
#include "adaptor/vector.h"
#include "object.h"
#include "parser_base.h"

namespace unicbor {

class packer : public parser_base {
public:
	enum class int_format {
		CANONICAL = 0,	/// Most efficent encoding. Used as standard encoding for CBOR recommended by RFC 7049 §2.1.
		EXTENDED = 1,	/// This encoding doesn't use additional bits in first initial byte. So, first bit acts only for determining type and for HALF, SINGLE, DOUBLE, BREAK_CODE simple values. For values >23 acts like AUTO.
		EXACT = 2,	/// Inefficent encoding. Doesn't use additional bytes 0b000xxxxx, forcing all CBOR items to be 2 bytes minimum. Not recommended by standard, so may be incompatible with other decoders.

		COMPACT = 255,	/// TODO: Tries to compress 64-bit integers more aggressively, packing them as floats (if 32-bit float numerically equivalent to 64-bit integer). This leads to losing the type information in serialized object, but RFC 7049 §3.6. states, that encoder can use this trick to reduce size of serialized object.

		AUTO = CANONICAL
	};
	enum class float_format {
		CANONICAL = 0,	/// Packs float format in compact form of IEEE 754 floating point number. Converts NaN, Int and -Inf to smallest format possible. Used as cannonical encoding for CBOR recommended by RFC 7049 §3.6.
		EXACT = 1,	/// Inefficent encoding. Doesn't use additional bytes 0b000xxxxx, forcing all CBOR items to be 2 bytes minimum. Not recommended by standard, so may be incompatible with other decoders.

		COMPACT = 255,	/// TODO: Tries to pack the numbers as integers, if it is possible to do that without losing precision.

		AUTO = CANONICAL
	};
	enum class map_format {
		CANONICAL = 0,	/// RFC 7049 §3.9. states, that canonical CBOR maps cannot be indefinite and must be binary-sorted. Also, keys must not repeat. Reuires buffering of output.
		GENERIC = 1,	/// Non-canonical format, described in RFC 7049 §2.1., that can use same keys, random order of items and indefinite-length items.

		AUTO = GENERIC
	};
	enum class string_format {
		CANONICAL = 0,	/// RFC 7049 §3.9. states, that canonical CBOR strings cannot be indefinite. Requires buffering of output
		GENERIC = 1,	/// Non-canonical format, described in RFC 7049 §2.1., that can use same indefinite-length items.

		AUTO = GENERIC
	};
	enum class stringref_support {
		DISABLED = 0,	/// Ignore stringref tags.
		ENABLED = 1,	/// Detect tags 25 and 256 and pack strings in compact form
		AGGRESSIVE = 2,	/// TODO: Detect strings and apply stringref extension only if appending stringref tags actually reduces size

		AUTO = DISABLED
	};

	packer(std::ostream& buffer) : buffer_(buffer) {}
	virtual ~packer() {}

	// Unsigned integer (uint8_t, uint16_t, uint32_t, uint64_t)
	template<class Integer>
	void pack_uint(Integer value) {
		pack_init(value, type_mask::UINT);
		commit_item();
	}

	template<class Integer>
	void pack_neg_int(Integer value) {
		pack_init(value, type_mask::NEGATIVE_INT);
		commit_item();
	}

	// Signed integer (int8_t, int16_t, int32_t, int64_t)
	template<class Integer>
	void pack_int(Integer value) {
		if(value >= 0)
			pack_uint(static_cast<typename std::make_unsigned<Integer>::type>(value));
		else
			pack_neg_int(static_cast<typename std::make_unsigned<Integer>::type>(std::abs(value+1)));
	}

	/* Indefinite sequence types */
	void start_bin() {start_sequence(indefinite_marker::INDEFINITE_BYTE_STRING);}
	void stop_bin() {stop_sequence(indefinite_marker::INDEFINITE_BYTE_STRING);}

	void start_string() {start_sequence(indefinite_marker::INDEFINITE_TEXT_STRING);}
	void stop_string() {stop_sequence(indefinite_marker::INDEFINITE_TEXT_STRING);}

	void start_array() {start_sequence(indefinite_marker::INDEFINITE_ARRAY);}
	void stop_array() {stop_sequence(indefinite_marker::INDEFINITE_ARRAY);}

	void start_map() {start_sequence(indefinite_marker::INDEFINITE_MAP);}
	void stop_map() {stop_sequence(indefinite_marker::INDEFINITE_MAP);}

	/* Definite payload types */
	void start_payload(uint64_t payload_size, type_mask type) {
		if(!current_container_.empty() && is_payload_type(current_container_.top().first)) throw multiple_payloads_started();
		current_container_.push({(current_container)type, payload_size});
		pack_init(payload_size, type);
	}
	void start_bin(uint64_t payload_size) {
		start_payload(payload_size, type_mask::BYTE_STRING);
	}
	void start_string(uint64_t payload_size) {
		start_payload(payload_size, type_mask::TEXT_STRING);
	}

	void add_payload(const void* data, size_t size) {
		if(size == 0) return;
		if(!is_payload_type(current_container_.top().first)) throw add_mismatch();

		size_t& payload_left_ = current_container_.top().second;

		if(payload_left_ < size) throw payload_too_big();
		buffer_.write((char*)data, size);
		payload_left_ -= size;
		if(payload_left_ == 0) current_container_.pop();
		commit_item();
	}

	void add_payload(const char* payload) {add_payload(payload, strlen(payload));}

	// Binary string (aka Binary Large OBject aka BLOB)
	void pack_bin(const void* data, size_t size) {
		start_bin(size);
		add_payload(data, size);
	}

	// UTF-8 string
	void pack_string(const char* data, size_t size) {
		start_string(size);
		add_payload(data, size);
	}

	/* Definite sequence types */
	void start_array(uint64_t sequence_size) {
		current_container_.push({(current_container)type_mask::ARRAY, sequence_size});
		pack_init(sequence_size, type_mask::ARRAY);
	}

	void start_map(uint64_t sequence_size) {
		current_container_.push({(current_container)type_mask::MAP, sequence_size*2});
		pack_init(sequence_size, type_mask::MAP);
	}

	// Tag
	void pack_tag(tag value) {
		pack_init((uint64_t) value, type_mask::TAG);
	}

	void pack_simple(simple_value value) {
		pack_init((uint8_t)value, type_mask::FLOAT);
		commit_item();
	}

	// Boolean
	void pack_bool(bool value) {
		pack_simple(value ? simple_value::TRUE_VALUE : simple_value::FALSE_VALUE);
		commit_item();
	}

	// Null value
	void pack_null() {
		pack_simple(simple_value::NULL_VALUE);
		commit_item();
	}

	// Undefined value
	void pack_undefined() {
		pack_simple(simple_value::UNDEFINED);
		commit_item();
	}

	// Floating-point number
	void pack_half(half value) {
		pack_init_exact(*(reinterpret_cast<uint16_t*>(&value)), type_mask::FLOAT);
		commit_item();
	}

	void pack_float(float value) {
		pack_init_exact(*(reinterpret_cast<uint32_t*>(&value)), type_mask::FLOAT);
		commit_item();
	}

	void pack_double(double value) {
		pack_init_exact(*(reinterpret_cast<uint64_t*>(&value)), type_mask::FLOAT);
		commit_item();
	}

	template<class FPValue>
	void pack_fp(FPValue value) {
		if(float_format_ == float_format::CANONICAL || float_format_ == float_format::COMPACT){
			if(std::isnan(value))
				pack_half(half(NAN));
			else if(std::isinf(value))
				pack_half(half(value));
			else if(value == half(value))
				pack_half(half(value));
			else if(value == float(value))
				pack_float(float(value));
			else
				pack_double(value);
		}else if(float_format_ == float_format::EXACT) {
			if(sizeof(value) == 16)
				pack_half(half(value));
			else if(sizeof(value) == 32)
				pack_float(float(value));
			else
				pack_double(value);
		}
	}

	bool complete() const {
		if(!current_container_.empty()) return false;
	}

	// Generic pack routines
	packer& pack(const object& obj) {
		// Writing tags
		for(tag tag1 : obj.tags()){pack_tag(tag1);}

		// Writing init_bytes
		switch(obj.get_type()){
			case object::type::UINT: pack_uint(obj.get_uint()); break;
			case object::type::NEGATIVE_INT: pack_neg_int(obj.get_uint()); break;
			case object::type::BYTE_STRING: pack_bin(obj.payload().data(), obj.payload().size()); break;
			case object::type::TEXT_STRING: pack_string((const char*)obj.payload().data(), obj.payload().size()); break;
			case object::type::ARRAY: {
				start_array(obj.items().size());
				for(const auto& item : obj.items()){pack(item);}
			} break;
			case object::type::MAP: {
				start_map(obj.items().size()/2);
				for(const auto& item : obj.items()){pack(item);}
			} break;
			case object::type::FLOAT: pack_fp(obj.get_float()); break;
			case object::type::BOOL: pack_bool(obj.as<bool>()); break;
			case object::type::UNDEFINED: pack_undefined(); break;
			case object::type::NULL_VALUE: pack_null(); break;
			case object::type::SIMPLE: pack_simple(simple_value(obj.get_uint()));
		}
		return *this;
	}

	packer& pack(tag tag) {pack_tag(tag);}

	packer& pack(const void* data, size_t size) {pack_bin(data, size);}
	packer& pack(const char* data, size_t size) {pack_string(data, size);}

	template <class T>
	packer& pack(T value) {
		object obj(value);
		pack(obj);
	}

private:
	std::ostream& buffer_;

	int_format int_format_ = int_format::AUTO;
	float_format float_format_ = float_format::AUTO;
	map_format map_format_ = map_format::AUTO;
	string_format string_format_ = string_format::AUTO;
	stringref_support stringref_support_ = stringref_support::AUTO;

	/* Indefinite sequence start/stop */
	void start_sequence(indefinite_marker sequence_type) {
		current_container_.push({(current_container)sequence_type, 0});
		buffer_.write((char*)&sequence_type, 1);
	}
	void stop_sequence(indefinite_marker sequence_type) {
		if(current_container_.empty() || current_container_.top().first != (current_container)sequence_type) throw indefinite_sequence_mismatch();
		current_container_.pop();
		char break_code = (char)indefinite_marker::BREAK_CODE;
		buffer_.write(&break_code, 1);
	}

	void commit_item() {
		if(!current_container_.empty() && is_definite_sequence(current_container_.top().first)){
			current_container_.top().second--;
			if(current_container_.top().second == 0)
				current_container_.pop();
		}
	}

	template<class Integer>
	void pack_init(Integer value, type_mask major_type_mask) {
		static_assert(std::is_integral<Integer>::value && !std::is_signed<Integer>::value, "Unsigned integral type expected");
		if(int_format_ == int_format::AUTO){
			if(value <= 23)
				pack_init_compact(value, major_type_mask);
			else
				pack_init_extended(value, major_type_mask);
		}else if(int_format_ == int_format::EXTENDED) {
			pack_init_extended(value, major_type_mask);
		}else if(int_format_ == int_format::EXACT) {
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

	void pack_init_compact(uint8_t value, type_mask major_type_mask) {
		value = (uint8_t) major_type_mask | uint8_t(value & 0b00011111);
		buffer_.write((char*)&value, sizeof(value));
	}

	template<class Integer>
	void pack_init_extended(Integer value, type_mask major_type_mask) {
		static_assert(std::is_integral<Integer>::value && !std::is_signed<Integer>::value, "Unsigned integral type expected");
		if (value <= 0xff)
			pack_init_exact<uint8_t>((uint8_t) value, major_type_mask);
		else if (value <= 0xffff)
			pack_init_exact<uint16_t>((uint16_t) value, major_type_mask);
		else if (value <= 0xffffffff)
			pack_init_exact<uint32_t>((uint32_t) value, major_type_mask);
		else if (value <= 0xffffffffffffffff)
			pack_init_exact<uint64_t>((uint64_t) value, major_type_mask);
	}

	template<class Integer>
	void pack_init_exact(Integer value, type_mask mask) {
		static_assert(std::is_integral<Integer>::value && !std::is_signed<Integer>::value, "Unsigned integral type expected");
		static_assert(sizeof(Integer) <= sizeof(uint64_t), "Integral type of 1, 2, 4 or 8 bytes expected");

		boost::endian::endian_arithmetic<boost::endian::order::big, Integer, sizeof(Integer)*8, boost::endian::align::yes> value_be = value;
		std::array<uint8_t, 1+sizeof(Integer)> packed;

		uint8_t size_mask;
		switch(sizeof(Integer)) {
			case 1: size_mask = 24; break;
			case 2: size_mask = 25; break;
			case 4: size_mask = 26; break;
			case 8: size_mask = 27; break;
		}

		packed[0] = uint8_t(mask) | size_mask;
		std::copy((uint8_t*)&value_be, (uint8_t*)&value_be+sizeof(Integer), packed.data()+1);

		buffer_.write((char*)packed.data(), packed.size());
	}
};

} /* namespace unicbor */
