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
template <>
struct as<uint64_t> {
	uint64_t operator() (const object& o) const {
		return uint64_t(o.get_uint());
	}
};

template <>
struct as<int64_t> {
	int64_t operator() (const object& o) const {
		if(o.get_type() == object::type::UINT)
			return int64_t(o.get_uint());
		else if(o.get_type() == object::type::NEGATIVE_INT)
			return -(int64_t(o.get_uint()))-1;
	}
};

template <>
struct as<uint32_t> {
	uint32_t operator() (const object& o) const {
		return uint32_t(o.get_uint());
	}
};

template <>
struct as<int32_t> {
	int32_t operator() (const object& o) const {
		as<int64_t> getter;
		return getter(o);
	}
};

template <>
struct as<uint16_t> {
	uint16_t operator() (const object& o) const {
		return uint16_t(o.get_uint());
	}
};

template <>
struct as<int16_t> {
	int16_t operator() (const object& o) const {
		as<int64_t> getter;
		return getter(o);
	}
};

template <>
struct as<uint8_t> {
	uint8_t operator() (const object& o) const {
		return uint8_t(o.get_uint());
	}
};

template <>
struct as<int8_t> {
	int8_t operator() (const object& o) const {
		as<int64_t> getter;
		return getter(o);
	}
};

template <>
struct as<unsigned long long> {
	unsigned long long operator() (const object& o) const {
		return (unsigned long long)o.get_uint();
	}
};

template <>
struct as<long long> {
	long long operator() (const object& o) const {
		as<int64_t> getter;
		return getter(o);
	}
};

// Setters
template <>
struct set<uint64_t> {
	void operator() (object& o, uint64_t v) const {
		o.set_uint(v);
	}
};

template <>
struct set<int64_t> {
	void operator() (object& o, int64_t v) const {
		if(v >= 0)
			o.set_uint((uint64_t)v);
		else
			o.set_neg_int(std::abs((v+1)));
	}
};

template <>
struct set<uint32_t> {
	void operator() (object& o, uint32_t v) const {
		o.set_uint(v);
	}
};

template <>
struct set<int32_t> {
	void operator() (object& o, int32_t v) const {
		set<int64_t> setter;
		setter(o, v);
	}
};

template <>
struct set<uint16_t> {
	void operator() (object& o, uint16_t v) const {
		o.set_uint(v);
	}
};

template <>
struct set<int16_t> {
	void operator() (object& o, int16_t v) const {
		set<int64_t> setter;
		setter(o, v);
	}
};

template <>
struct set<uint8_t> {
	void operator() (object& o, uint8_t v) const {
		o.set_uint(v);
	}
};

template <>
struct set<int8_t> {
	void operator() (object& o, int8_t v) const {
		set<int64_t> setter;
		setter(o, v);
	}
};

template <>
struct set<unsigned long long> {
	void operator() (object& o, unsigned long long v) const {
		o.set_uint(v);
	}
};

template <>
struct set<long long> {
	void operator() (object& o, long long v) const {
		set<int64_t> setter;
		setter(o, v);
	}
};

} /* namespace adaptor */
} /* namespace unicbor */
