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

namespace unicbor {

struct undefined_t {};
undefined_t undefined;

using half_float::half;

// RFC 7049 §2.1.  Floating-Point Numbers and Values with No Content
enum class type_mask : uint8_t {
	UINT 		 = 0,
	NEGATIVE_INT = 1 << 5,
	BYTE_STRING	 = 2 << 5,
	TEXT_STRING	 = 3 << 5,
	ARRAY		 = 4 << 5,
	MAP			 = 5 << 5,
	TAG			 = 6 << 5,
	FLOAT		 = 7 << 5
};

// RFC 7049 §2.2.  Floating-Point Numbers and Values with No Content
// It is used with FLOAT major_type
enum class simple_value : uint8_t {
	FALSE_VALUE = 20,
	TRUE_VALUE = 21,
	NULL_VALUE = 22,
	UNDEFINED = 23,
	// 24 will be handled automatically
	HALF = 25,
	SINGLE = 26,
	DOUBLE = 27,

	BREAK_CODE = 31
};

// RFC 7049 §2.2.  Indefinite Lengths for Some Major Types
enum class indefinite_marker : uint8_t {
	INDEFINITE_BYTE_STRING = uint8_t(type_mask::BYTE_STRING) | uint8_t(31),
	INDEFINITE_TEXT_STRING = uint8_t(type_mask::TEXT_STRING) | uint8_t(31),
	INDEFINITE_ARRAY = uint8_t(type_mask::ARRAY) | uint8_t(31),
	INDEFINITE_MAP = uint8_t(type_mask::MAP) | uint8_t(31),
	BREAK_CODE = uint8_t(type_mask::FLOAT) | uint8_t(31)
};

// IANA Registry: https://www.iana.org/assignments/cbor-tags/cbor-tags.xhtml
// (This is not complete, additional tags will be added soon)
enum class tag : uint64_t {
	// Date/time
	DATETIME_STRING = 0,
	DATETIME_NUMERIC = 1,
	TIMESTAMP = DATETIME_NUMERIC,

	// Big numbers
	BIGNUM_POSITIVE = 2,
	BIGNUM_NEGATIVE = 3,

	// Floats
	DECIMAL_FRACTION = 4,
	BIGFLOAT = 5,

	/* 6..20 Unassigned */

	EXPECT_BASE64URL = 21,
	EXPECT_BASE64 = 22,
	EXPECT_BASE16 = 23,
	EXPECT_HEX = EXPECT_BASE16,

	ENCODED_CBOR = 24,

	URI = 32,
	BASE64URL = 33,
	BASE64 = 34,

	REGULAR_EXPRESSION = 35,
	REGEXP = REGULAR_EXPRESSION,

	MIME = 36,

	CBOR_SELF_DESCRIBE = 55799,

	/* stringref extension */
	STRINGREF = 25,
	STRINGREF_NAMESPACE = 256
};

} /* namespace unicbor */
