/* Test for the Letter-to-Phoneme finite state transducer (matching) algorithm.
 *
 * Copyright (C) 2014 Reece H. Dunn
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

#include <cainteoir/language.hpp>
#include <cstdio>

#include "tester.hpp"

REGISTER_TESTSUITE("l2p_transducer");

namespace tts = cainteoir::tts;

void test_(const char *pattern,
           const char *s,
           int offset,
           int match_offset,
           bool matches,
           const char *file, int line)
{
	tts::l2p_transducer rule(pattern);
	const char *current = s + offset;

	assert_location(rule.match(s, current, s + strlen(s)) == matches, file, line);
	assert_location(current == s + match_offset, file, line);
}

#define test(...) test_(__VA_ARGS__, __FILE__, __LINE__)

TEST_CASE("consume : single character")
{
	test("a", "a", 0, 1, true);
	test("a", "b", 0, 0, false);
	test("b", "b", 0, 1, true);
}

TEST_CASE("consume : multiple characters")
{
	test("aa", "aa", 0, 2, true);
	test("ab", "aa", 0, 0, false);
	test("ba", "aa", 0, 0, false);
}

TEST_CASE("consume : utf-8 characters")
{
	// NOTE: Matching UTF-8 characters works the same way as matching
	// multiple characters ...

	test("\xCE\xB1", "\xCE\xB1", 0, 2, true);  // 'α', 'α'
	test("\xCE\xB1", "\xCE\xB2", 0, 0, false); // 'α', 'β'
}
