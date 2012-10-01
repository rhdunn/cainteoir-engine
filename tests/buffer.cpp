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

void match_(const std::shared_ptr<cainteoir::buffer> &got, const char *expected, int len, const char *file, int line)
{
	assert_location(len == got->size(), file, line);
	const char *buffer = got->begin();
	for (int i = 0; i < got->size(); ++i)
		assert_location(uint8_t(expected[i]) == uint8_t(buffer[i]), file, line);
}

#define match(got, expected, len) match_(got, expected, len, __FILE__, __LINE__)

TEST_CASE("cainteoir::make_buffer")
{
	match(cainteoir::make_buffer(std::string("test")), "test", 5); // null terminated
	match(cainteoir::make_buffer("test", 4), "test", 4);
	match(cainteoir::make_buffer("test", 2), "te", 2);
}

TEST_CASE("cainteoir::normalize -- no space characters")
{
	match(cainteoir::normalize(cainteoir::make_buffer("test", 4)), "test", 4);
}

TEST_CASE("cainteoir::normalize -- U+0009 (CHARACTER TABULATION)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\x09\x09\x09test", 7)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\x09\x09\x09", 7)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\x09String", 11)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\x09\x09\x09String", 13)),
	      "Test String", 11);
}

TEST_CASE("cainteoir::normalize -- U+000A (LINE FEED)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\x0A\x0A\x0Atest", 7)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\x0A\x0A\x0A", 7)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\x0AString", 11)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\x0A\x0A\x0AString", 13)),
	      "Test String", 11);
}

TEST_CASE("cainteoir::normalize -- U+000B (LINE TABULATION)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\x0B\x0B\x0Btest", 7)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\x0B\x0B\x0B", 7)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\x0BString", 11)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\x0B\x0B\x0BString", 13)),
	      "Test String", 11);
}

TEST_CASE("cainteoir::normalize -- U+000C (FORM FEED)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\x0C\x0C\x0Ctest", 7)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\x0C\x0C\x0C", 7)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\x0CString", 11)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\x0C\x0C\x0CString", 13)),
	      "Test String", 11);
}

TEST_CASE("cainteoir::normalize -- U+000D (CARRIAGE RETURN)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\x0D\x0D\x0Dtest", 7)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\x0D\x0D\x0D", 7)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\x0DString", 11)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\x0D\x0D\x0DString", 13)),
	      "Test String", 11);
}

TEST_CASE("cainteoir::normalize -- U+0020 (SPACE)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\x20\x20\x20test", 7)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\x20\x20\x20", 7)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\x20String", 11)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\x20\x20\x20String", 13)),
	      "Test String", 11);
}

TEST_CASE("cainteoir::normalize -- U+0085 (NEXT LINE)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\xC2\x85\xC2\x85\xC2\x85test", 10)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\xC2\x85\xC2\x85\xC2\x85", 10)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xC2\x85String", 12)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xC2\x85\xC2\x85\xC2\x85String", 16)),
	      "Test String", 11);
}

TEST_CASE("cainteoir::normalize -- U+00A0 (NO-BREAK SPACE)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\xC2\xA0\xC2\xA0\xC2\xA0test", 10)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\xC2\xA0\xC2\xA0\xC2\xA0", 10)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xC2\xA0String", 12)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xC2\xA0\xC2\xA0\xC2\xA0String", 16)),
	      "Test String", 11);
}

TEST_CASE("cainteoir::normalize -- U+1680 (OGHAM SPACE MARK)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\xE1\x9A\x80\xE1\x9A\x80\xE1\x9A\x80test", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\xE1\x9A\x80\xE1\x9A\x80\xE1\x9A\x80", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE1\x9A\x80String", 13)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE1\x9A\x80\xE1\x9A\x80\xE1\x9A\x80String", 19)),
	      "Test String", 11);
}

TEST_CASE("cainteoir::normalize -- U+180E (MONGOLIAN VOWEL SEPARATOR)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\xE1\xA0\x8E\xE1\xA0\x8E\xE1\xA0\x8Etest", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\xE1\xA0\x8E\xE1\xA0\x8E\xE1\xA0\x8E", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE1\xA0\x8EString", 13)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE1\xA0\x8E\xE1\xA0\x8E\xE1\xA0\x8EString", 19)),
	      "Test String", 11);
}

TEST_CASE("cainteoir::normalize -- U+2000 (EN QUAD)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\xE2\x80\x80\xE2\x80\x80\xE2\x80\x80test", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\xE2\x80\x80\xE2\x80\x80\xE2\x80\x80", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\x80String", 13)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\x80\xE2\x80\x80\xE2\x80\x80String", 19)),
	      "Test String", 11);
}

TEST_CASE("cainteoir::normalize -- U+2001 (EM QUAD)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\xE2\x80\x81\xE2\x80\x81\xE2\x80\x81test", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\xE2\x80\x81\xE2\x80\x81\xE2\x80\x81", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\x81String", 13)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\x81\xE2\x80\x81\xE2\x80\x81String", 19)),
	      "Test String", 11);
}

TEST_CASE("cainteoir::normalize -- U+2002 (EN SPACE)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\xE2\x80\x82\xE2\x80\x82\xE2\x80\x82test", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\xE2\x80\x82\xE2\x80\x82\xE2\x80\x82", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\x82String", 13)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\x82\xE2\x80\x82\xE2\x80\x82String", 19)),
	      "Test String", 11);
}

TEST_CASE("cainteoir::normalize -- U+2003 (EM SPACE)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\xE2\x80\x82\xE2\x80\x82\xE2\x80\x82test", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\xE2\x80\x82\xE2\x80\x82\xE2\x80\x82", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\x82String", 13)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\x82\xE2\x80\x82\xE2\x80\x82String", 19)),
	      "Test String", 11);
}

TEST_CASE("cainteoir::normalize -- U+2004 (THREE-PER-EM SPACE)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\xE2\x80\x84\xE2\x80\x84\xE2\x80\x84test", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\xE2\x80\x84\xE2\x80\x84\xE2\x80\x84", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\x84String", 13)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\x84\xE2\x80\x84\xE2\x80\x84String", 19)),
	      "Test String", 11);
}

TEST_CASE("cainteoir::normalize -- U+2005 (FOUR-PER-EM SPACE)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\xE2\x80\x85\xE2\x80\x85\xE2\x80\x85test", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\xE2\x80\x85\xE2\x80\x85\xE2\x80\x85", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\x85String", 13)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\x85\xE2\x80\x85\xE2\x80\x85String", 19)),
	      "Test String", 11);
}

TEST_CASE("cainteoir::normalize -- U+2006 (SIX-PER-EM SPACE)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\xE2\x80\x86\xE2\x80\x86\xE2\x80\x86test", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\xE2\x80\x86\xE2\x80\x86\xE2\x80\x86", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\x86String", 13)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\x86\xE2\x80\x86\xE2\x80\x86String", 19)),
	      "Test String", 11);
}

TEST_CASE("cainteoir::normalize -- U+2007 (FIGURE SPACE)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\xE2\x80\x87\xE2\x80\x87\xE2\x80\x87test", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\xE2\x80\x87\xE2\x80\x87\xE2\x80\x87", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\x87String", 13)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\x87\xE2\x80\x87\xE2\x80\x87String", 19)),
	      "Test String", 11);
}

TEST_CASE("cainteoir::normalize -- U+2008 (PUNCTUATION SPACE)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\xE2\x80\x88\xE2\x80\x88\xE2\x80\x88test", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\xE2\x80\x88\xE2\x80\x88\xE2\x80\x88", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\x88String", 13)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\x88\xE2\x80\x88\xE2\x80\x88String", 19)),
	      "Test String", 11);
}

TEST_CASE("cainteoir::normalize -- U+2009 (THIN SPACE)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\xE2\x80\x89\xE2\x80\x89\xE2\x80\x89test", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\xE2\x80\x89\xE2\x80\x89\xE2\x80\x89", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\x89String", 13)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\x89\xE2\x80\x89\xE2\x80\x89String", 19)),
	      "Test String", 11);
}

TEST_CASE("cainteoir::normalize -- U+200A (HAIR SPACE)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\xE2\x80\x8A\xE2\x80\x8A\xE2\x80\x8Atest", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\xE2\x80\x8A\xE2\x80\x8A\xE2\x80\x8A", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\x8AString", 13)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\x8A\xE2\x80\x8A\xE2\x80\x8AString", 19)),
	      "Test String", 11);
}

TEST_CASE("cainteoir::normalize -- U+2028 (LINE SEPARATOR)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\xE2\x80\xA8\xE2\x80\xA8\xE2\x80\xA8test", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\xE2\x80\xA8\xE2\x80\xA8\xE2\x80\xA8", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\xA8String", 13)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\xA8\xE2\x80\xA8\xE2\x80\xA8String", 19)),
	      "Test String", 11);
}

TEST_CASE("cainteoir::normalize -- U+2029 (PARAGRAPH SEPARATOR)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\xE2\x80\xA9\xE2\x80\xA9\xE2\x80\xA9test", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\xE2\x80\xA9\xE2\x80\xA9\xE2\x80\xA9", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\xA9String", 13)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\xA9\xE2\x80\xA9\xE2\x80\xA9String", 19)),
	      "Test String", 11);
}

TEST_CASE("cainteoir::normalize -- U+202F (NARROW NO-BREAK SPACE)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\xE2\x80\xAF\xE2\x80\xAF\xE2\x80\xAFtest", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\xE2\x80\xAF\xE2\x80\xAF\xE2\x80\xAF", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\xAFString", 13)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x80\xAF\xE2\x80\xAF\xE2\x80\xAFString", 19)),
	      "Test String", 11);
}

TEST_CASE("cainteoir::normalize -- U+205F (MEDIUM MATHEMATICAL SPACE)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\xE2\x81\x9F\xE2\x81\x9F\xE2\x81\x9Ftest", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\xE2\x81\x9F\xE2\x81\x9F\xE2\x81\x9F", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x81\x9FString", 13)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE2\x81\x9F\xE2\x81\x9F\xE2\x81\x9FString", 19)),
	      "Test String", 11);
}

TEST_CASE("cainteoir::normalize -- U+3000 (IDEOGRAPHIC SPACE)")
{
	match(cainteoir::normalize(cainteoir::make_buffer("\xE3\x80\x80\xE3\x80\x80\xE3\x80\x80test", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("test\xE3\x80\x80\xE3\x80\x80\xE3\x80\x80", 13)),
	      "test", 4);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE3\x80\x80String", 13)),
	      "Test String", 11);
	match(cainteoir::normalize(cainteoir::make_buffer("Test\xE3\x80\x80\xE3\x80\x80\xE3\x80\x80String", 19)),
	      "Test String", 11);
}
