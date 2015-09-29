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

namespace cborpp {

class parser_base {
protected:
	enum class current_container : uint8_t {
		BYTE_STRING = (uint8_t)type_mask::BYTE_STRING,
		TEXT_STRING = (uint8_t)type_mask::TEXT_STRING,
		ARRAY = (uint8_t)type_mask::ARRAY,
		MAP = (uint8_t)type_mask::MAP,

		INDEFINITE_BYTE_STRING = (uint8_t)indefinite_marker::INDEFINITE_BYTE_STRING,
		INDEFINITE_TEXT_STRING = (uint8_t)indefinite_marker::INDEFINITE_TEXT_STRING,
		INDEFINITE_ARRAY = (uint8_t)indefinite_marker::INDEFINITE_ARRAY,
		INDEFINITE_MAP = (uint8_t)indefinite_marker::INDEFINITE_MAP
	};

	std::stack<std::pair<current_container, uint64_t>> current_container_;

	bool is_indefinite_sequence(current_container container){return ((uint8_t)container & (uint8_t)0b00011111) == 31;}
	bool is_definite_sequence(current_container container){return container == current_container::ARRAY || container == current_container::MAP;}
	bool is_sequence(current_container container){return is_indefinite_sequence(container) || is_definite_sequence(container);}
	bool is_payload_type(current_container container){return !is_sequence(container) && (container == current_container::BYTE_STRING || container == current_container::TEXT_STRING);}

	bool is_indefinite_sequence(){return is_indefinite_sequence(current_container_.top().first);}
	bool is_definite_sequence(){return is_definite_sequence(current_container_.top().first);}
	bool is_sequence(){return is_sequence(current_container_.top().first);}
	bool is_payload_type(){return is_payload_type(current_container_.top().first);}
};

} /* namespace cborpp */
