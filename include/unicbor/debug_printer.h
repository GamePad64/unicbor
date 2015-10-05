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
#include "packer.h"
#include "unpacker.h"

namespace unicbor {

std::string debug_string(object& o) {
	std::ostringstream result;
	if(o.is_sequence()){
		switch(o.get_type()){
			case object::type::ARRAY:
				result << "[";
				break;
			case object::type::MAP:
				result << "{";
				break;
		}
	}
	switch(o.get_type()){
		case object::type::UINT:
			result << o.as<uint64_t>();
			break;
		case object::type::NEGATIVE_INT:
			result << o.as<int64_t>();
			break;
		case object::type::BOOL:
			result << o.as<bool>();
			break;
		case object::type::FLOAT:
			result << o.as<double>();
			break;
		case object::type::SIMPLE:
			result << o.as<uint64_t>();
			break;
		case object::type::TEXT_STRING:
			result << "\"" << o.as<std::string>() << "\"";
			break;
		case object::type::BYTE_STRING:
			result << "h\'";
			for(uint8_t b : o.as<std::vector<uint8_t>>()) {
				boost::io::ios_flags_saver ifs(result);
				result << std::hex << std::setfill('0') << std::setw(2) << b;
			}
			result << "\'";
			break;

		default:
			for(auto& child_object : o.items()) {
				result << debug_string(child_object) << " ";
			}
	}
	if(o.is_sequence()){
		switch(o.get_type()){
			case object::type::ARRAY:
				result << "]";
				break;
			case object::type::MAP:
				result << "}";
				break;
		}
	}
	return result.str();
}

} /* namespace unicbor */
