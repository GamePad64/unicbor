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

#include "constants.h"
#include "object.h"
#include "parser_base.h"

namespace unicbor {

struct parse_error : public error {
	parse_error() : error("Parse error") {}
};

class unpacker : public parser_base {
public:
	enum class map_format {
		CANONICAL = 0,	/// RFC 7049 §3.9. states, that canonical CBOR maps cannot be indefinite and must be binary-sorted. Also, keys must not repeat. Reuires buffering of output.
		GENERIC = 1,	/// Non-canonical format, described in RFC 7049 §2.1., that can use same keys, random order of items and indefinite-length items.

		AUTO = CANONICAL
	};
	enum class string_format {
		CANONICAL = 0,	/// RFC 7049 §3.9. states, that canonical CBOR strings cannot be indefinite. Requires buffering of output
		GENERIC = 1,	/// Non-canonical format, described in RFC 7049 §2.1., that can use same indefinite-length items.

		AUTO = CANONICAL
	};
	enum class stringref_support {
		DISABLED = 0,	/// Ignore stringref tags.
		ENABLED = 1,	/// Detect tags 25 and 256 and pack strings in compact form

		AUTO = DISABLED
	};

	unpacker(object& o) : o(&o) {
		objects_.push(&o);
	}
	virtual ~unpacker() {}

	size_t add_bytes(const void* data, size_t size) {
		//if(complete()) throw complete_object();
		input_buffer_.insert(input_buffer_.end(), (uint8_t*)data, ((uint8_t*)data)+size);

		while(parse_next()) {}
	}

	void set_map_format(map_format property) {map_format_ = property;}
	void set_string_format(string_format property) {string_format_ = property;}
	void set_stringref_support(stringref_support property) {stringref_support_ = property;}
private:
	std::stack<object*> objects_;
	object* o;

	std::deque<uint8_t> input_buffer_;
	//std::stack<boost::bimap<std::string, uint64_t>> strref_ns;

	map_format map_format_ = map_format::AUTO;
	string_format string_format_ = string_format::AUTO;
	stringref_support stringref_support_ = stringref_support::AUTO;

	void consume_buffer(size_t size) {input_buffer_.erase(input_buffer_.begin(), input_buffer_.begin()+size);}

	bool parse_numeric(const std::vector<uint8_t>& init_bytes) {
		push_item();
		switch(get_type(init_bytes[0])) {
			case type_mask::UINT: {
				o->set_uint(parse_int(init_bytes));
			} break;
			case type_mask::NEGATIVE_INT: {
				o->set_neg_int(parse_int(init_bytes));
			} break;
			case type_mask::TAG: {
				o->add_tag(tag(parse_int(init_bytes)));
			} break;
			case type_mask::FLOAT: {
				if(is_simple(input_buffer_[0]))
					o->set_simple(simple_value(parse_int(init_bytes)));
				else
					o->set_float(parse_float(init_bytes));
			} break;
			default: throw error();
		}
		pop_item();
		commit_item();
		consume_buffer(init_bytes.size());
		return true;
	}

	bool parse_payload(const std::vector<uint8_t>& init_bytes) {
		uint64_t payload_size = parse_int(init_bytes);
		if(input_buffer_.size() - init_bytes.size() < payload_size) return false;
		consume_buffer(init_bytes.size());

		std::vector<uint8_t> payload(input_buffer_.begin(), input_buffer_.begin()+payload_size);

		push_item();
		switch(get_type(init_bytes[0])) {
			case type_mask::BYTE_STRING: {
				o->set_bin(payload.data(), payload.size());
			} break;
			case type_mask::TEXT_STRING: {
				o->set_string((char*)payload.data(), payload.size());
			} break;
			default: throw error();
		}
		pop_item();
		commit_item();
		consume_buffer(payload.size());

		return true;
	}

	bool parse_complex(const std::vector<uint8_t>& init_bytes) {
		push_item();
		if(is_definite_sequence(init_bytes[0])){
			switch(get_type(init_bytes[0])) {
				case type_mask::ARRAY: {
					start_array(parse_int(init_bytes));
				} break;
				case type_mask::MAP: {
					start_map(parse_int(init_bytes));
				} break;
				default: throw parse_error();
			}
		}else{
			switch(get_type(init_bytes[0])) {
				case type_mask::BYTE_STRING:
					start_bin(); break;
				case type_mask::TEXT_STRING:
					start_string(); break;
				case type_mask::ARRAY:
					start_array(); break;
				case type_mask::MAP:
					start_map(); break;
				case type_mask::FLOAT:
					commit_item(); break;
				default: throw error();
			}
		}
		consume_buffer(init_bytes.size());
		return true;
	}

	bool parse_next() {
		if(input_buffer_.empty()) return false;

		std::vector<uint8_t> init_bytes(init_size(input_buffer_[0]));
		if (input_buffer_.size() < init_bytes.size()) return false;

		init_bytes.assign(input_buffer_.begin(), input_buffer_.begin() + init_bytes.size());

		if(is_complex(init_bytes[0]) || init_bytes[0] == (uint8_t)indefinite_marker::BREAK_CODE){
			return parse_complex(init_bytes);
		}else{
			switch(get_type(init_bytes[0])) {
				case type_mask::UINT:
				case type_mask::NEGATIVE_INT:
				case type_mask::TAG:
				case type_mask::FLOAT:
					return parse_numeric(init_bytes);

				case type_mask::BYTE_STRING:
				case type_mask::TEXT_STRING: {
					return parse_payload(init_bytes);
				}
				default: throw error();
			}
		}
	}

	void start_sequence(current_container container) {
		o->reset(object::type(((uint8_t)container & 0b11100000) >> 5));
		current_container_.push({container, 0});
	}
	void start_sequence(current_container container, uint64_t size) {
		o->reset(object::type(((uint8_t)container & 0b11100000) >> 5));
		current_container_.push({container, size});
	}

	void push_item() {
		if(!current_container_.empty() && is_sequence()){
			objects_.push(&(o->add_item()));
			o = objects_.top();
		}
	}
	void pop_item() {
		if(!current_container_.empty() && is_sequence()){
			objects_.pop();
			o = objects_.top();
		}
	}

	void commit_item() {
		if(!current_container_.empty() && parser_base::is_definite_sequence(current_container_.top().first)){
			current_container_.top().second--;
			if(current_container_.top().second == 0){
				current_container_.pop();
				pop_item();
			}
		}
	}

	void start_bin() {start_sequence(current_container::INDEFINITE_BYTE_STRING);}
	void start_string() {start_sequence(current_container::INDEFINITE_TEXT_STRING);}
	void start_array() {start_sequence(current_container::INDEFINITE_ARRAY);}
	void start_map() {start_sequence(current_container::INDEFINITE_MAP);}

	void start_array(uint64_t size) {start_sequence(current_container::ARRAY, size);}
	void start_map(uint64_t size) {start_sequence(current_container::MAP, size*2);}

	type_mask get_type(uint8_t init_byte) const {return type_mask(init_byte & (uint8_t)0b11100000);}
	uint8_t parse_compact(uint8_t init_byte) const {return init_byte & (uint8_t)0b00011111;}

	bool is_simple(uint8_t init_byte) const {
		return get_type(init_byte) == type_mask::FLOAT && parse_compact(init_byte) <= 24;
	}

	bool is_indefinite_sequence(uint8_t init_byte) const {return parse_compact(init_byte) == 31;}
	bool is_definite_sequence(uint8_t init_byte) const {
		type_mask type = get_type(init_byte);
		return !is_indefinite_sequence(init_byte) && (type == type_mask::ARRAY || type == type_mask::MAP);
	}
	bool is_complex(uint8_t init_byte) const {return is_indefinite_sequence(init_byte) || is_definite_sequence(init_byte);}
	bool is_payload_type(uint8_t init_byte) const {
		type_mask type = get_type(init_byte);
		return !is_complex(init_byte) && (type == type_mask::BYTE_STRING || type == type_mask::TEXT_STRING);
	}

	uint_fast8_t init_size(uint8_t init_byte) const {
		switch(parse_compact(init_byte)){
			case 24: return 1+1;
			case 25: return 1+2;
			case 26: return 1+4;
			case 27: return 1+8;
			default: return 1+0;
		}
	};

	uint64_t parse_int(const std::vector<uint8_t>& init_bytes) const {
		uint8_t additional_bytes = parse_compact(init_bytes[0]);
		if(additional_bytes < 24)
			return additional_bytes;

		switch(additional_bytes) {
			case 24: return init_bytes[1];
			case 25: {
				boost::endian::endian_arithmetic<boost::endian::order::big, uint16_t, sizeof(uint16_t)*8, boost::endian::align::yes> value_be;
				std::copy(init_bytes.begin()+1, init_bytes.end(), (uint8_t*)&value_be);
				return (uint16_t)value_be;
			}
			case 26: {
				boost::endian::endian_arithmetic<boost::endian::order::big, uint32_t, sizeof(uint32_t)*8, boost::endian::align::yes> value_be;
				std::copy(init_bytes.begin()+1, init_bytes.end(), (uint8_t*)&value_be);
				return (uint32_t)value_be;
			}
			case 27: {
				boost::endian::endian_arithmetic<boost::endian::order::big, uint64_t, sizeof(uint64_t)*8, boost::endian::align::yes> value_be;
				std::copy(init_bytes.begin()+1, init_bytes.end(), (uint8_t*)&value_be);
				return (uint64_t)value_be;
			}
			default: throw parse_error();
		}
	}

	double parse_float(const std::vector<uint8_t>& init_bytes) const {
		uint8_t additional_bytes = parse_compact(init_bytes[0]);
		if(additional_bytes < 25)
			return additional_bytes;

		switch(additional_bytes) {
			case 25: {
				half value;

				std::copy(init_bytes.begin()+1, init_bytes.end(), (uint8_t*)&value);
				boost::endian::big_to_native_inplace(*((uint16_t*)&value));
				return (double)value;
			}
			case 26: {
				union{
					uint32_t value_be;
					float value;
				};
				std::copy(init_bytes.begin()+1, init_bytes.end(), (uint8_t*)&value_be);
				boost::endian::big_to_native_inplace(value_be);
				return (double)value;
			}
			case 27: {
				union{
					uint64_t value_be;
					double value;
				};
				std::copy(init_bytes.begin()+1, init_bytes.end(), (uint8_t*)&value_be);
				boost::endian::big_to_native_inplace(value_be);
				return (double)value;
			}
			default: throw parse_error();
		}
	}
};

} /* namespace unicbor */
