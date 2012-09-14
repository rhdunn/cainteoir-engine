/* Test for the buffer API.
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

#include <cainteoir/buffer.hpp>
#include <cstdio>

#include "tester.hpp"

REGISTER_TESTSUITE("buffer");

void match(const std::shared_ptr<cainteoir::buffer> &got, const char *expected, int len)
{
	assert(len == got->size());
	const char *buffer = got->begin();
	for (int i = 0; i < got->size(); ++i)
		assert(uint8_t(expected[i]) == uint8_t(buffer[i]));
}

TEST_CASE("cainteoir::make_buffer")
{
	match(cainteoir::make_buffer(std::string("test")), "test", 5); // null terminated
	match(cainteoir::make_buffer("test", 4), "test", 4);
	match(cainteoir::make_buffer("test", 2), "te", 2);
}

TEST_CASE("cainteoir::normalize")
{
	match(cainteoir::normalize(cainteoir::make_buffer("test", 4)), "test", 4);

	// whitespace at the start:

	match(cainteoir::normalize(cainteoir::make_buffer("   a test", 9)), "a test", 6); // SPACE
	match(cainteoir::normalize(cainteoir::make_buffer("\t\t\ta test", 9)), "a test", 6); // HORIZONTAL TAB
	match(cainteoir::normalize(cainteoir::make_buffer("\r\r\ra test", 9)), "a test", 6); // CARRIDGE RETURN
	match(cainteoir::normalize(cainteoir::make_buffer("\n\n\na test", 9)), "a test", 6); // LINE FEED
	match(cainteoir::normalize(cainteoir::make_buffer("\xC2\xA0\xC2\xA0 a test", 11)), "a test", 6); // NBSP

	// whitespace at the end:

	match(cainteoir::normalize(cainteoir::make_buffer("a test   ", 9)), "a test", 6); // SPACE
	match(cainteoir::normalize(cainteoir::make_buffer("a test\t\t\t", 9)), "a test", 6); // HORIZONTAL TAB
	match(cainteoir::normalize(cainteoir::make_buffer("a test\r\r\r", 9)), "a test", 6); // CARRIDGE RETURN
	match(cainteoir::normalize(cainteoir::make_buffer("a test\n\n\n", 9)), "a test", 6); // LINE FEED
	match(cainteoir::normalize(cainteoir::make_buffer("a test\xC2\xA0\xC2\xA0", 10)), "a test", 6); // NBSP

	// whitespace in the middle:

	match(cainteoir::normalize(cainteoir::make_buffer("a test", 6)), "a test", 6); // SPACE
	match(cainteoir::normalize(cainteoir::make_buffer("a\ttest", 6)), "a test", 6); // HORIZONTAL TAB
	match(cainteoir::normalize(cainteoir::make_buffer("a\rtest", 6)), "a test", 6); // CARRIDGE RETURN
	match(cainteoir::normalize(cainteoir::make_buffer("a\ntest", 6)), "a test", 6); // LINE FEED
	match(cainteoir::normalize(cainteoir::make_buffer("a\xC2\xA0test", 6)), "a test", 6); // NBSP

	// consecutive whitespace in the middle:

	match(cainteoir::normalize(cainteoir::make_buffer("a   test", 8)), "a test", 6); // SPACE
	match(cainteoir::normalize(cainteoir::make_buffer("a\t\t\ttest", 8)), "a test", 6); // HORIZONTAL TAB
	match(cainteoir::normalize(cainteoir::make_buffer("a\r\r\rtest", 8)), "a test", 6); // CARRIDGE RETURN
	match(cainteoir::normalize(cainteoir::make_buffer("a\n\n\ntest", 8)), "a test", 6); // LINE FEED
	match(cainteoir::normalize(cainteoir::make_buffer("a\xC2\xA0\xC2\xA0test", 9)), "a test", 6); // NBSP
}
