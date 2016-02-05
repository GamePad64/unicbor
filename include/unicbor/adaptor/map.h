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
template <class X, class Y>
struct as<std::map<X, Y>> {
	std::map<X, Y> operator() (const object& o) const {
		std::map<X, Y> result;
		for(auto it = o.items().cbegin(); it < o.items().cend(); it++){
			result.insert({it->as<X>(), (++it)->as<Y>()});
		}
		return result;
	}
};

template <class X, class Y>
struct set<std::map<X, Y>> {
	void operator() (object& o, std::map<X, Y> v) const {
		o.reset(object::type::MAP);
		for(auto& item_pair : v){
			o.add_item(item_pair.first);
			o.add_item(item_pair.second);
		}
	}
};

} /* namespace adaptor */
} /* namespace unicbor */
