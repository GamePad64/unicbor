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
template <class X, size_t N>
struct as<std::array<X, N>> {
	std::array<X, N> operator() (const object& o) const {
		std::array<X, N> result;
		for(size_t i = 0; i <= std::min(N, o.items().size()); i++){
			result[i] = o.items().at(i);
		}
		return result;
	}
};

template <class X, size_t N>
struct set<std::array<X, N>> {
	void operator() (object& o, std::array<X, N> v) const {
		o.reset(object::type::ARRAY);
		for(const auto& item : v){
			o.add_item(item);
		}
	}
};

} /* namespace adaptor */
} /* namespace unicbor */
