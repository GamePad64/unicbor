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
#include <boost/bimap.hpp>

namespace cborpp {

struct error : public std::runtime_error {
	error() : std::runtime_error("UniCBOR error") {}
	error(const char* what) : std::runtime_error(what) {}
};
struct incomplete_object : public error {
	incomplete_object() : error("Incomplete object") {}
};
struct complete_object : public error {
	complete_object() : error("Object is complete. You cannot add new elements to it") {}
};
struct indefinite_sequence_mismatch : public error {
	indefinite_sequence_mismatch() : error("Indefinite sequence started as one type, ended as another") {}
};
struct add_mismatch : public error {
	add_mismatch() : error("Elements can be added only to sequence types and payload to payload types") {}
};
struct payload_too_big : public error {
	payload_too_big() : error("Added payload is bigger, than payload type, holding it") {}
};

struct parse_error : public error {
	parse_error() : error("Parse error") {}
};

class object {
	enum pack_method {
		AUTO=0,	/// Most efficent encoding. Used as standard encoding for CBOR recommended by RFC 7049 §2.1.
		EXACT=1	/// Inefficent encoding. Doesn't use additional bytes 0b000xxxxx, forcing all CBOR items to be 2 bytes minimum. Not recommended by standard, so may be incompatible with other decoders.
	};
public:
	object(std::ostream* ostream_buffer = nullptr) : ostream_buffer_(ostream_buffer) {}
	virtual ~object() {}

	/* Serialization */
	void add_tag(tag tag) {
		tags_.push_back(tag);
		write_ostream(serialize((uint8_t)tag, type_mask::TAG));
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
	void start_bin(uint64_t payload_size) {
		left_ = payload_size;
		payload_bytes_.clear();
		set_init(serialize(payload_size, type_mask::BYTE_STRING));
	}
	void start_string(uint64_t payload_size) {
		left_ = payload_size;
		payload_bytes_.clear();
		set_init(serialize(payload_size, type_mask::TEXT_STRING));
	}

	void add_payload(const void* data, size_t size) {
		if(size == 0) return;
		if(complete()) throw complete_object();
		if(!is_payload_type()) throw add_mismatch();
		if(left_ < size) throw payload_too_big();
		payload_bytes_.insert(payload_bytes_.end(), (uint8_t*)data, ((uint8_t*)data)+size);
		write_ostream(payload_bytes_);
		left_ -= size;
	}
	void add_payload(const std::vector<uint8_t>& payload) {add_payload(payload.data(), payload.size());}
	void add_payload(const std::string& payload) {add_payload(payload.data(), payload.size());}

	// Binary string (aka Binary Large OBject aka BLOB)
	void set_bin(const void* data, size_t size) {
		start_bin(size);
		add_payload(data, size);
	}
	void set_bin(const std::vector<uint8_t>& string) {set_bin(string.data(), string.size());}

	// UTF-8 string
	void set_string(const char* data, size_t size) {
		start_string(size);
		add_payload(data, size);
	}
	void set_string(const std::string& string) {set_string(string.data(), string.size());}
	void set_string(const char* data) {set_string(data, strlen(data));} // Well, null-terminated strings are bad by design, but as general-purpose library we should support them.

	/* Definite sequence types */
	void start_array(uint64_t sequence_size) {
		left_ = sequence_size;
		set_init(serialize(sequence_size, type_mask::ARRAY));
	}

	void start_map(uint64_t sequence_size) {
		left_ = sequence_size*2;
		set_init(serialize(sequence_size, type_mask::MAP));
	}

	object& add_element() {
		if(complete()) throw complete_object();
		if(!is_complex()) throw add_mismatch();

		child_.emplace_back();
		child_.back().set_parent(this);
		if(is_definite_sequence()) left_--;
		return child_.back();
	}

	void add_element(object child) {
		object& child_ref = add_element();
		child_ref = child;
	}

	template<class T>
	void add_element(T element) {
		object child(this);
		child.set(element);
		add_element(child);
	}

	template<class T>
	void add_element(const T* data, size_t size) {
		object child(this);
		child.set(data, size);
		add_element(child);
	}

	template<class T, class N>
	void add_element(std::pair<T, N> element) {
		add_element(element.first);
		add_element(element.second);
	}

	template<class T>
	void set_array(const std::vector<T>& array){
		start_array(array.size());
		for(auto& array_item : array){
			add_element(array_item);
		}
	}
	template<class T, size_t N>
	void set_array(const std::array<T,N>& array){
		start_array(N);
		for(auto& array_item : array){
			add_element(array_item);
		}
	}

	template<class T, class N>
	void set_map(const std::map<T, N>& map){
		start_map(map.size());
		for(auto& map_item : map){
			add_element(map_item);
		}
	}

	// For serialization
	std::string serialize_to_string() {
		if(!complete()) throw incomplete_object();

		std::string result;
		// Writing tags
		for(tag tag1 : tags_){
			auto tag_v = serialize((uint8_t)tag1, type_mask::TAG);
			result.append(tag_v.begin(), tag_v.end());
		}
		// Writing init_bytes
		result.append(init_bytes_.begin(), init_bytes_.end());

		if(is_payload_type()){
			result.append(payload_bytes_.begin(), payload_bytes_.end());
		}else if(is_complex()) {
			for(auto& child : child_){
				result.append(child.serialize_to_string());
			}
			if(is_indefinite_sequence()){
				result.push_back((char)indefinite_marker::BREAK_CODE);
			}
		}
		return result;
	}

	// Unsigned integer (uint8_t, uint16_t, uint32_t, uint64_t)
	template<class Integer>
	void set_uint(Integer integer, pack_method method = pack_method::AUTO) {set_init(serialize(integer, type_mask::UINT));}

	// Negative integer in format std::abs(integer)-1
	void set_neg_int(uint64_t integer) {set_init(serialize(integer, type_mask::NEGATIVE_INT));}

	// Generic integer.
	template<class Integer>
	void set_int(Integer integer, pack_method method = pack_method::AUTO) {
		if(integer >= 0)
			set_uint(static_cast<typename std::make_unsigned<Integer>::type>(integer));
		else
			set_neg_int(static_cast<typename std::make_unsigned<Integer>::type>(std::abs(integer+1)));
	}

	// Simple values
	void set_simple(simple_value value) {
		init_bytes_ = serialize((uint8_t)value, type_mask::FLOAT);
		write_ostream(init_bytes_);
	}

	// Boolean
	void set_bool(bool value) {
		set_simple(value ? simple_value::TRUE_VALUE : simple_value::FALSE_VALUE);
	}

	// Null value
	void set_null() {
		set_simple(simple_value::NULL_VALUE);
	}

	// Undefined value
	void set_undefined() {
		set_simple(simple_value::UNDEFINED);
	}

	// Floating-point number
	// C library doesn't have half-precision format, so we accept pointer to half-precision float variable. Endianess: native.
	void set_half(half value) {
		init_bytes_ = serialize_exact(*(reinterpret_cast<uint16_t*>(&value)), type_mask::FLOAT);
		write_ostream(init_bytes_);
	}

	void set_float(float value) {
		init_bytes_ = serialize_exact(*(reinterpret_cast<uint32_t*>(&value)), type_mask::FLOAT);
		write_ostream(init_bytes_);
	}

	void set_double(double value) {
		init_bytes_ = serialize_exact(*(reinterpret_cast<uint64_t*>(&value)), type_mask::FLOAT);
		write_ostream(init_bytes_);
	}

	template<class FPValue>
	void set_fp(FPValue value) {
		if(value == half(value))
			set_half(half(value));
		else if(value == float(value))
			set_float(float(value));
		else
			set_double(value);
	}

	/* Generic "set" functions */
	void set(const void* data, size_t size){set_bin(data, size);}
	void set(const char* data, size_t size){set_string(data, size);}
	void set(const std::string& data){set_string(data);}
	void set(const char* data){set_string(data);}

	template<class T>
	void set(const std::vector<T>& array){set_array(array);}
	template<class T, class N>
	void set(const std::map<T, N>& map){set_map(map);}

	void set(simple_value value){set_simple(value);}
	void set(bool value){set_bool(value);}
	void set(nullptr_t){set_null();}
	void set(undefined_t){set_undefined();}

	template<class Integer>
	typename std::enable_if<std::is_integral<Integer>::value>::type set(Integer value) {set_int(value);}

	template<class FPValue>
	typename std::enable_if<std::numeric_limits<FPValue>::has_infinity>::type set(FPValue value) {set_fp(value);}

	template<class T>
	object(T value) : object() {set(value);}

	template<class T>
	object(const T* data, size_t size) : object() {set(data, size);}

	/* Parsing */
	void add_bytes(const void* data, size_t size) {
		if(complete()) throw complete_object();
		input_buffer_.insert(input_buffer_.end(), (uint8_t*)data, ((uint8_t*)data)+size);

		while(!input_buffer_.empty()) {
			if (init_bytes_.empty()) {
				// Then it can be only TAG or init_byte
				int more = how_many_more(input_buffer_[0]);
				if (input_buffer_.size() >= 1 + more) {
					std::vector<uint8_t> init_buffer(input_buffer_.begin(), input_buffer_.begin() + 1 + more);
					input_buffer_.erase(input_buffer_.begin(), input_buffer_.begin()+init_buffer.size());

					switch(get_type(input_buffer_[0])) {
						case type_mask::UINT: set_uint(parse_int(init_buffer)); break;
						case type_mask::NEGATIVE_INT: set_neg_int(parse_int(init_buffer)); break;

						case type_mask::BYTE_STRING: {
							if(is_indefinite_sequence(input_buffer_[0]))
								start_bin();
							else
								start_bin(parse_int(init_buffer));
						} break;
						case type_mask::TEXT_STRING: {
							if(is_indefinite_sequence(input_buffer_[0]))
								start_string();
							else
								start_string(parse_int(init_buffer));
						} break;
						case type_mask::ARRAY: {
							if(is_indefinite_sequence(input_buffer_[0]))
								start_array();
							else
								start_array(parse_int(init_buffer));
						} break;
						case type_mask::TAG: add_tag(tag(parse_int(init_buffer))); break;
						case type_mask::FLOAT: {
							if(is_simple(input_buffer_[0]))
								set_simple(simple_value(parse_int(init_buffer)));
							else
								set_fp(parse_float(init_buffer));
						} break;
					}
				}
				continue;
			}else if(is_payload_type()){
				auto ready_to_write = std::min(input_buffer_.size(), left_);
				add_payload(std::vector<uint8_t>(input_buffer_.begin(), input_buffer_.begin()+ready_to_write));
				continue;
			}else if(is_complex()){
				continue;
			}
			break;
		}
	}

	/* Converters */
	uint64_t as_uint64() const {
		return parse_int(init_bytes_);
	}

	/* Getters */
	bool complete() const {
		if(init_bytes_.empty()) return false;
		for(auto& child : child_){
			if(!child.complete()) return false;
		}
		return indefinite_sequence_ == indefinite_marker::BREAK_CODE && left_ == 0;
	}
	bool is_stream_mode() const {return (bool)ostream_buffer_;}

	/* Getters-parsers */
	type_mask get_type() const {return get_type(init_bytes_[0]);}
	bool is_indefinite_sequence() const {return !init_bytes_.empty() && is_indefinite_sequence(init_bytes_[0]);}
	bool is_definite_sequence() const {return !init_bytes_.empty() && is_definite_sequence(init_bytes_[0]);}
	bool is_complex() const {return !init_bytes_.empty() && is_complex(init_bytes_[0]);}
	bool is_payload_type() const {return !init_bytes_.empty() && is_payload_type(init_bytes_[0]);}

private:
	object* parent_;

	std::vector<tag> tags_;
	std::vector<uint8_t> init_bytes_;
	std::vector<uint8_t> payload_bytes_;
	std::vector<object> child_;

	indefinite_marker indefinite_sequence_ = indefinite_marker::BREAK_CODE;
	uint64_t left_ = 0;

	std::ostream* ostream_buffer_;
	std::deque<uint8_t> input_buffer_;

	void set_parent(object* parent) {
		parent_ = parent;
		ostream_buffer_ = parent->ostream_buffer_;
	}

	object(object* parent) : object(parent->ostream_buffer_) {
		set_parent(parent);
	}

	void write_ostream(const std::vector<uint8_t>& serialized_data) {
		if(is_stream_mode()) ostream_buffer_->write((const char*)serialized_data.data(), serialized_data.size());
	}

	/* Private setters */
	void set_init(const std::vector<uint8_t> init_bytes){
		init_bytes_ = init_bytes;
		write_ostream(init_bytes_);
	}

	/* Indefinite sequence start/stop */
	void start_sequence(indefinite_marker sequence_type) {
		indefinite_sequence_ = sequence_type;
		set_init({(uint8_t)sequence_type});
	}
	void stop_sequence(indefinite_marker sequence_type) {
		if(indefinite_sequence_ != sequence_type) throw indefinite_sequence_mismatch();
		indefinite_sequence_ = indefinite_marker::BREAK_CODE;
		write_ostream(std::vector<uint8_t>({(uint8_t)indefinite_marker::BREAK_CODE}));
	}

	/* Serialization routines */
	template<class Integer>
	std::vector<uint8_t> serialize_exact(Integer value, type_mask mask) {
		static_assert(std::is_integral<Integer>::value && !std::is_signed<Integer>::value, "Unsigned integral type expected");
		static_assert(sizeof(Integer) <= sizeof(uint64_t), "Integral type of 1, 2, 4 or 8 bytes expected");

		boost::endian::endian_arithmetic<boost::endian::order::big, Integer, sizeof(Integer)*8, boost::endian::align::yes> value_be = value;
		std::vector<uint8_t> cbor_buffer(1+sizeof(Integer));

		uint8_t size_mask = 0;
		switch(sizeof(Integer)) {
			case 1: size_mask = 24; break;
			case 2: size_mask = 25; break;
			case 4: size_mask = 26; break;
			case 8: size_mask = 27; break;
			default: break;
		}

		cbor_buffer[0] = uint8_t(mask) | size_mask;
		std::copy((uint8_t*)&value_be, (uint8_t*)&value_be+sizeof(Integer), cbor_buffer.data()+1);

		return cbor_buffer;
	}

	std::vector<uint8_t> serialize_compact(uint8_t value, type_mask major_type_mask) {
		value = (uint8_t) major_type_mask | uint8_t(value & 0b00011111);
		return std::vector<uint8_t>({value});
	}

	template<class Integer>
	std::vector<uint8_t> serialize(Integer value, type_mask major_type_mask, pack_method method = pack_method::AUTO) {
		static_assert(std::is_integral<Integer>::value && !std::is_signed<Integer>::value, "Unsigned integral type expected");
		if(method == pack_method::AUTO) {
			if (value <= 23)
				return serialize_compact(value, major_type_mask);
			else if (value <= 0xff)
				return serialize_exact<uint8_t>((uint8_t) value, major_type_mask);
			else if (value <= 0xffff)
				return serialize_exact<uint16_t>((uint16_t) value, major_type_mask);
			else if (value <= 0xffffffff)
				return serialize_exact<uint32_t>((uint32_t) value, major_type_mask);
			else if (value <= 0xffffffffffffffff)
				return serialize_exact<uint64_t>((uint64_t) value, major_type_mask);
		}else if(method == pack_method::EXACT) {
			if(sizeof(Integer) == 1)
				return serialize_exact<uint8_t>((uint8_t) value, major_type_mask);
			else if (sizeof(Integer) == 2)
				return serialize_exact<uint16_t>((uint16_t) value, major_type_mask);
			else if (sizeof(Integer) == 4)
				return serialize_exact<uint32_t>((uint32_t) value, major_type_mask);
			else if (sizeof(Integer) == 8)
				return serialize_exact<uint64_t>((uint64_t) value, major_type_mask);
		} throw error();
	}

	/* Parsing routines */
	type_mask get_type(uint8_t init_byte) const {return type_mask(init_byte & (uint8_t)0b11100000);}
	uint8_t parse_compact(uint8_t init_byte) const {return init_byte & (uint8_t)0b00011111;}

	bool is_simple(uint8_t init_byte) const {
		return get_type(init_byte) == type_mask::FLOAT && parse_compact(init_byte) <= 24;
	}

	bool is_indefinite_sequence(uint8_t init_byte) const {return parse_compact(init_byte) == 31;}
	bool is_definite_sequence(uint8_t init_byte) const {
		type_mask type = get_type(init_byte);
		return !is_indefinite_sequence() && (type == type_mask::ARRAY || type == type_mask::MAP);
	}
	bool is_complex(uint8_t init_byte) const {return is_indefinite_sequence(init_byte) || is_definite_sequence(init_byte);}
	bool is_payload_type(uint8_t init_byte) const {
		type_mask type = get_type(init_byte);
		return !is_complex(init_byte) && (type == type_mask::BYTE_STRING || type == type_mask::TEXT_STRING);
	}

	int how_many_more(uint8_t init_byte) const {
		switch(parse_compact(init_byte)){
			case 24: return 1;
			case 25: return 2;
			case 26: return 4;
			case 27: return 8;
			default: return 0;
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

} /* namespace cborpp */
