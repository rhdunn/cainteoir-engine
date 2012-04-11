/* Test for UTF-8 manipulation.
 *
 * Copyright (C) 2012 Reece H. Dunn
 *
 * This file is part of cainteoir-engine.
 *
 * cainteoir-engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cainteoir-engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cainteoir-engine.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cainteoir/unicode.hpp>
#include <stdexcept>
#include <cstdio>

#include "tester.hpp"

namespace utf8 = cainteoir::utf8;

REGISTER_TESTSUITE("UTF-8");

void encode(uint32_t c, const char *encoded)
{
	char buffer[50];

	const char *end = utf8::write(buffer, c);
	assert(strlen(encoded) == end-buffer);
	for (int i = 0; i < end-buffer; ++i)
		assert(uint8_t(encoded[i]) == uint8_t(buffer[i]));
}

TEST_CASE("encoding a 1-byte UTF-8 sequence")
{
	encode(0x0043, "\x43");
	encode(0x0065, "\x65");
	encode(0x007F, "\x7F");
}

TEST_CASE("encoding a 2-byte UTF-8 sequence")
{
	encode(0x0080, "\xC2\x80");
	encode(0x00A0, "\xC2\xA0");
	encode(0x00BF, "\xC2\xBF");
	encode(0x00C0, "\xC3\x80");
	encode(0x00FF, "\xC3\xBF");
	encode(0x0100, "\xC4\x80");
	encode(0x07FF, "\xDF\xBF");
}

TEST_CASE("encoding a 3-byte UTF-8 sequence")
{
	encode(0x0800, "\xE0\xA0\x80");
	encode(0x083F, "\xE0\xA0\xBF");
	encode(0x0840, "\xE0\xA1\x80");
	encode(0x087F, "\xE0\xA1\xBF");
	encode(0x0880, "\xE0\xA2\x80");
	encode(0x093B, "\xE0\xA4\xBB");
	encode(0x0FFF, "\xE0\xBF\xBF");
	encode(0x1000, "\xE1\x80\x80");
	encode(0xFFFF, "\xEF\xBF\xBF");
}

TEST_CASE("encoding a 4-byte UTF-8 sequence")
{
	// NOTE: Strictly speaking, only values up to and including 0x10FFFF are valid unicode characters.

	encode(0x010000, "\xF0\x90\x80\x80");
	encode(0x01003F, "\xF0\x90\x80\xBF");
	encode(0x010040, "\xF0\x90\x81\x80");
	encode(0x010190, "\xF0\x90\x86\x90");
	encode(0x013018, "\xF0\x93\x80\x98");
	encode(0x1FFFFF, "\xF7\xBF\xBF\xBF");
}

TEST_CASE("encoding an out-of-range UTF-8 sequence")
{
	// NOTE: 5-byte and 6-byte UTF-8 is encodable as UTF-8, but are not valid unicode.

	encode(0x00200000, "");
	encode(0x0197BF4D, "");
	encode(0xFFFFFFFF, "");
}
