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

namespace cborpp {

template<class Buffer>
class unpacker {
public:
	unpacker(Buffer& buffer) : buffer_(buffer) {}
	virtual ~unpacker() {}

	object unpack_init() {
		uint8_t c = buffer_.get();
		switch(c){
			case (uint8_t)indefinite_marker::INDEFINITE_BYTE_STRING:
			case (uint8_t)indefinite_marker::INDEFINITE_TEXT_STRING:
			case (uint8_t)indefinite_marker::INDEFINITE_ARRAY:
			case (uint8_t)indefinite_marker::INDEFINITE_MAP:
			case (uint8_t)indefinite_marker::BREAK_CODE:
				return object(c);
			default: break;
		}
		uint8_t additional = c & 0b00011111;
		if(additional <= 23)
			return object(c, (uint64_t)additional);
		else if(additional == 24){
			return object(c, (uint64_t)buffer_.get());
		}else if(additional == 25) {
			uint16_t int_payload;
			half fp_payload;
			union {
				boost::endian::big_int16_at big_value;
				std::array<uint8_t, 2> buf;
			};

			buf[0] = buffer_.get();
			buf[1] = buffer_.get();

			int_payload = big_value;
			return object(c, (uint64_t)int_payload);
		}else if(additional == 26) {
			union {
				uint32_t int_payload;
				float fp_payload;
			};
			union {
				boost::endian::big_int32_at big_value;
				std::array<uint8_t, 4> buf;
			};

			buf[0] = buffer_.get();
			buf[1] = buffer_.get();
			buf[2] = buffer_.get();
			buf[3] = buffer_.get();

			int_payload = big_value;
			return object(c, (uint64_t) int_payload);
		}else if(additional == 27) {
			union {
				uint64_t int_payload;
				double fp_payload;
			};
			union {
				boost::endian::big_int64_at big_value;
				std::array<uint8_t, 8> buf;
			};

			buf[0] = buffer_.get();
			buf[1] = buffer_.get();
			buf[2] = buffer_.get();
			buf[3] = buffer_.get();
			buf[4] = buffer_.get();
			buf[5] = buffer_.get();
			buf[6] = buffer_.get();
			buf[7] = buffer_.get();

			int_payload = big_value;
			return object(c, (uint64_t)int_payload);
		}
	}

	void copy_bytes(char* buffer, size_t size) {
		for(;buffer > 0; --size){
			*buffer = buffer_.get();
			buffer++;
		}
	}

	std::string copy_bytes(size_t size) {
		std::string s;
		for(int i = 0; i < size; i++){
			s.push_back(buffer_.get());
		}
		return s;
	}

private:
	Buffer& buffer_;

	std::stack<boost::bimap<std::string, uint64_t>> strref_ns;
};

} /* namespace cborpp */
