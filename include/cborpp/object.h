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
#include "exceptions.h"
#include "constants.h"
#include "adaptor/adaptor_base.h"
#include <boost/bimap.hpp>

namespace cborpp {

class object {
public:
	enum type {
		UINT,
		NEGATIVE_INT,
		BYTE_STRING,
		TEXT_STRING,
		ARRAY,
		MAP,
		/*TAG,*/
		FLOAT,
		// Other are subtypes of FLOAT
		BOOL,
		UNDEFINED,
		NULL_VALUE,
		SIMPLE
	};

	object() {reset(UNDEFINED);}
	object(type type) {reset(type);}
	virtual ~object(){}

	void add_tag(tag tag){
		tags_.push_back(tag);
	}

	/* Definite payload types */
	void set_payload(const void* data, size_t size, type type){
		reset(type);
		payload_.assign((uint8_t*)data, ((uint8_t*)data) + size);
	}

	void add_payload(const void* data, size_t size){
		payload_.insert(payload_.end(), (uint8_t*)data, ((uint8_t*)data) + size);
	}

	// Binary string (aka Binary Large OBject aka BLOB)
	void set_bin(const void* data, size_t size){
		set_payload(data, size, type::BYTE_STRING);
	}

	// UTF-8 string
	void set_string(const char* data, size_t size){
		set_payload(data, size, type::TEXT_STRING);
	}

	/* Definite sequence types */
	object& add_item(){
		if(!is_sequence()) throw add_mismatch();

		items_.emplace_back();
		items_.back();
		return items_.back();
	}

	void add_item(object child){
		object& child_ref = add_item();
		child_ref = child;
	}

	template<class T>
	void add_item(T value){
		add_item().set(value);
	}

	template<class T>
	void add_item(const T* data, size_t size){
		add_item().set(data, size);
	}

	// Unsigned integer (uint8_t, uint16_t, uint32_t, uint64_t)
	void set_uint(uint64_t value){
		reset(type::UINT); content_.uint_content_ = value;
	}

	// Negative integer in format std::abs(integer)-1
	void set_neg_int(uint64_t value){
		reset(type::NEGATIVE_INT); content_.uint_content_ = value;
	}

	// Simple values
	void set_simple(simple_value value){
		switch(value){
			case simple_value::TRUE_VALUE:
			case simple_value::FALSE_VALUE:
				reset(type::BOOL); break;

			case simple_value::UNDEFINED:
				reset(type::UNDEFINED); break;

			case simple_value::NULL_VALUE:
				reset(type::NULL_VALUE); break;

			case simple_value::BREAK_CODE:
			case simple_value::HALF:
			case simple_value::SINGLE:
			case simple_value::DOUBLE:
				reset(type::UNDEFINED); break;
			default:
				reset(type::SIMPLE);
		}
		content_.uint_content_ = (uint64_t)value;
	}

	// Boolean
	void set_bool(bool value){
		set_simple(value ? simple_value::TRUE_VALUE : simple_value::FALSE_VALUE);
	}

	// Null value
	void set_null(){
		set_simple(simple_value::NULL_VALUE);
	}

	// Undefined value
	void set_undefined(){
		set_simple(simple_value::UNDEFINED);
	}

	// Floating-point number
	void set_float(double value){
		reset(type::FLOAT); content_.float_content_ = value;
	}

	void reset(type type){
		type_ = type;
		payload_.clear();
		items_.clear();
		content_ = {0};
	}

	/* Getters */
	const std::vector<tag>& tags() const {return tags_;}
	type get_type() const {return type_;}

	uint64_t get_uint() const {return content_.uint_content_;}
	double get_float() const {return content_.float_content_;}

	const std::vector<uint8_t>& payload() const {return payload_;}

	std::vector<object>& items() {return items_;}
	const std::vector<object>& items() const {return items_;}

	/* Generic "set" functions */
	void set(const void* data, size_t size){set_bin(data, size);}
	void set(const char* data, size_t size){set_string(data, size);}

	template<class T>
	void set(T value){
		adaptor::set<T> setter;
		setter(*this, value);
	}

	template<class T>
	T as() const {
		adaptor::as<T> getter;
		return getter(*this);
	}

	template<class T>
	object(T value) : object(){set(value);}

	template<class T>
	object(const T* data, size_t size) : object(){set(data, size);}

	/* Properties */
	bool is_sequence() const{return type_ == ARRAY || type_ == MAP;}
	bool is_payload_type() const{return type_ == BYTE_STRING || type_ == TEXT_STRING;}

private:
	std::vector<tag> tags_;
	type type_;
	union object_union {
		uint64_t uint_content_;
		double float_content_;
	} content_;
	std::vector<uint8_t> payload_;
	std::vector<object> items_;
};

} /* namespace cborpp */
