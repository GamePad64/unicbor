/* Copyright (C) 2015 Alexander Shishenko <alex@shishenko.com>
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
#include "../constants.h"
#include "adaptor_base.h"
#include "../object.h"

namespace unicbor {

namespace adaptor {

// Converters
template <class X>
struct as<std::vector<X>> {
	std::vector<X> operator() (const object& o) const {
		std::vector<X> result;
		for(const auto& item : o.items()){
			result.push_back(item.as<X>());
		}
		return result;
	}
};

template <class X>
struct set<std::vector<X>> {
	void operator() (object& o, std::vector<X> v) const {
		o.reset(object::type::ARRAY);
		for(const auto& item : v){
			o.add_item(item);
		}
	}
};

template <>
struct as<std::vector<uint8_t>> {
	std::vector<uint8_t> operator() (const object& o) const {
		std::vector<uint8_t> result;
		result.assign(o.payload().begin(), o.payload().end());
		return result;
	}
};

template <>
struct set<std::vector<uint8_t>> {
	void operator() (object& o, std::vector<uint8_t> v) const {
		o.reset(object::type::BYTE_STRING);
		o.set(v.data(), v.size());
	}
};

} /* namespace adaptor */
} /* namespace unicbor */
