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

struct error : public std::runtime_error {
	error() : std::runtime_error("UniCBOR error") {}
	error(const char* what) : std::runtime_error(what) {}
};
struct add_mismatch : public error {
	add_mismatch() : error("Items can be added only to sequence types") {}
};
struct complete_object : public error {
	complete_object() : error("Object is complete. You cannot add new elements to it") {}
};
struct indefinite_sequence_mismatch : public error {
	indefinite_sequence_mismatch() : error("Indefinite sequence started as one type, ended as another") {}
};
struct payload_too_big : public error {
	payload_too_big() : error("Added payload is bigger, than payload type, holding it") {}
};
struct multiple_payloads_started : public error {
	multiple_payloads_started() : error("Only one payload allowed") {}
};

} /* namespace unicbor */
