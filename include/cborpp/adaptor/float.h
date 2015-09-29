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
#include "../constants.h"
#include "adaptor_base.h"
#include "../object.h"

namespace cborpp {

namespace adaptor {

template <>
struct as<half> {
	half operator() (const object& o) const {
		return half(o.get_float());
	}
};

template <>
struct set<half> {
	void operator() (object& o, half v) const {
		o.set_float(v);
	}
};

template <>
struct as<float> {
	float operator() (const object& o) const {
		return float(o.get_float());
	}
};

template <>
struct set<float> {
	void operator() (object& o, float v) const {
		o.set_float(v);
	}
};

template <>
struct as<double> {
	double operator() (const object& o) const {
		return double(o.get_float());
	}
};

template <>
struct set<double> {
	void operator() (object& o, double v) const {
		o.set_float(v);
	}
};

} /* namespace adaptor */
} /* namespace cborpp */
