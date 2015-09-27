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

std::string int1("\0", 1);
std::string int2("\x01", 1);
std::string int3("\x0A", 1);
std::string int4("\x17", 1);
std::string int5("\x18\x18", 2);
std::string int6("\x18\x19", 2);
std::string int7("\x18\x64", 2);
std::string int8("\x19\x03\xe8", 3);
std::string int9("\x1a\0\x0f\x42\x40", 5);
std::string int10("\x1b\0\0\0\xe8\xd4\xa5\x10\0", 9);
std::string int11("\x1b\xff\xff\xff\xff\xff\xff\xff\xff", 9);

std::string bigint1("\xc2\x49\x01\0\0\0\0\0\0\0\0", 11);
std::string neg_bigint1("\x3b\xff\xff\xff\xff\xff\xff\xff\xff", 9);
std::string neg_bigint2("\xc3\x49\x01\0\0\0\0\0\0\0\0", 11);

std::string neg_int1("\x20", 1);
std::string neg_int2("\x29", 1);
std::string neg_int3("\x38\x63", 2);
std::string neg_int4("\x39\x03\xe7", 3);
std::string float1("\xf9\0\0", 3);
std::string float2("\xf9\x80\0", 3);
std::string float3("\xf9\x3c\0", 3);
std::string float4("\xfb\x3f\xf1\x99\x99\x99\x99\x99\x9a", 9);
std::string float5("\xf9\x3e\0", 3);
std::string float6("\xf9\x7b\xff", 3);
std::string float7("\xfa\x47\xc3\x50\0", 5);
std::string float8("\xfa\x7f\x7f\xff\xff", 5);
std::string float9("\xfb\x7e\x37\xe4\x3c\x88\0\x75\x9c", 9);
std::string float10("\xf9\0\x01", 3);
std::string float11("\xf9\x04\0", 3);
std::string float12("\xf9\xc4\0", 3);
std::string float13("\xfb\xc0\x10\x66\x66\x66\x66\x66\x66", 9);

std::string half_limits1("\xf9\x7c\0", 3);
std::string half_limits2("\xf9\x7e\0", 3);
std::string half_limits3("\xf9\xfc\0", 3);
std::string float_limits1("\xfa\x7f\x80\0\0", 5);
std::string float_limits2("\xfa\x7f\xc0\0\0", 5);
std::string float_limits3("\xfa\xff\x80\0\0", 5);
std::string double_limits1("\xfb\x7f\xf0\0\0\0\0\0\0", 9);
std::string double_limits2("\xfb\x7f\xf8\0\0\0\0\0\0", 9);
std::string double_limits3("\xfb\xff\xf0\0\0\0\0\0\0", 9);

std::string simple1("\xf4", 1);
std::string simple2("\xf5", 1);
std::string simple3("\xf6", 1);
std::string simple4("\xf7", 1);
std::string simple5("\xf0", 1);
std::string simple6("\xf8\x18", 2);
std::string simple7("\xf8\xff", 2);

std::string tag1("\xc0\x74\x32\x30\x31\x33\x2d\x30\x33\x2d\x32\x31\x54\x32\x30\x3a\x30\x34\x3a\x30\x30\x5a", 22);
std::string tag2("\xc1\x1a\x51\x4b\x67\xb0", 6);
std::string tag3("\xc1\xfb\x41\xd4\x52\xd9\xec\x20\0\0 ", 10);
std::string tag4("\xd7\x44\x01\x02\x03\x04", 6);
std::string tag5("\xd8\x18\x45\x64\x49\x45\x54\x46", 8);
std::string tag6("\xd8\x20\x76\x68\x74\x74\x70\x3a\x2f\x2f\x77\x77\x77\x2e\x65\x78\x61\x6d\x70\x6c\x65\x2e\x63\x6f\x6d", 25);

std::string bin_string1("\x40", 1);
std::string bin_string2("\x44\x01\x02\x03\x04", 5);

std::string string1("\x60", 1);
std::string string2("\x61\x61", 2);
std::string string3("\x64\x49\x45\x54\x46", 5);
std::string string4("\x62\x22\x5c", 3);
std::string string5("\x62\xc3\xbc", 3);
std::string string6("\x63\xe6\xb0\xb4", 4);
std::string string7("\x64\xf0\x90\x85\x91", 5);

std::string array1("\x80", 1);
std::string array2("\x83\x01\x02\x03", 4);
std::string array3("\x83\x01\x82\x02\x03\x82\x04\x05", 8);
std::string array4("\x98\x19\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x18\x18\x19", 29);

std::string map1("\xa0", 1);
std::string map2("\xa2\x01\x02\x03\x04", 5);
std::string map3("\xa2\x61\x61\x01\x61\x62\x82\x02\x03", 9);
std::string map4("\x82\x61\x61\xa1\x61\x62\x61\x63", 8);
std::string map5("\xa5\x61\x61\x61\x41\x61\x62\x61\x42\x61\x63\x61\x43\x61\x64\x61\x44\x61\x65\x61\x45", 21);

std::string indef1("\x5f\x42\x01\x02\x43\x03\x04\x05\xff", 9);
std::string indef2("\x7f\x65\x73\x74\x72\x65\x61\x64\x6d\x69\x6e\x67\xff", 13);
std::string indef3("\x9f\xff", 2);
std::string indef4("\x9f\x01\x82\x02\x03\x9f\x04\x05\xff\xff", 10);
std::string indef5("\x9f\x01\x82\x02\x03\x82\x04\x05\xff", 9);
std::string indef6("\x83\x01\x82\x02\x03\x9f\x04\x05\xff", 9);
std::string indef7("\x83\x01\x9f\x02\x03\xff\x82\x04\x05", 9);
std::string indef8("\x9f\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x18\x18\x19\xff", 29);
std::string indef9("\xbf\x61\x61\x01\x61\x62\x9f\x02\x03\xff\xff", 11);
std::string indef10("\x82\x61\x61\xbf\x61\x62\x61\x63\xff", 9);
std::string indef11("\xbf\x63\x46\x75\x6e\xf5\x63\x41\x6d\x74\x21\xff", 12);
