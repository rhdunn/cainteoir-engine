/* Test for the buffer API.
 *
 * Copyright (C) 2012-2014 Reece H. Dunn
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

void match_(const cainteoir::buffer &got, const char *expected, int len, const char *file, int line)
{
	bool matched = assert_location(len == got.size(), file, line);
	const char *buffer = got.begin();
	for (int i = 0; i < got.size(); ++i)
		matched &= assert_location(uint8_t(expected[i]) == uint8_t(buffer[i]), file, line);
	if (!matched)
	{
		fprintf(stdout, "==> failed with test string \"");
		for (auto c : cainteoir::buffer(expected, expected + len))
		{
			if (c > 0x20 && c < 0x80)
				fprintf(stdout, "%c", c);
			else
				fprintf(stdout, "\\x%02X", (uint8_t)c);
		}
		fprintf(stdout, "\"\n");
	}
}

void match_(const std::shared_ptr<cainteoir::buffer> &got, const char *expected, int len, const char *file, int line)
{
	assert_location(got.get(), file, len);
	match_(*got, expected, len, file, line);
}

#define match(got, expected, len) match_(got, expected, len, __FILE__, __LINE__)

TEST_CASE("cainteoir::buffer construction")
{
	cainteoir::buffer a(nullptr);
	assert(a.size() == 0);
	assert(a.empty());
	assert(a.begin() == a.end());

	const char *value = "hello there";
	match(cainteoir::buffer(value), "hello there", 11);
	match(cainteoir::buffer(value, value+5), "hello", 5);
}

TEST_CASE("cainteoir::make_buffer")
{
	match(cainteoir::make_buffer(std::string("test")), "test", 5); // null terminated
	match(cainteoir::make_buffer("test", 4), "test", 4);
	match(cainteoir::make_buffer("test", 2), "te", 2);
}

struct normalized_testcase
{
	std::shared_ptr<cainteoir::buffer> preserved;
	std::shared_ptr<cainteoir::buffer> collapsed;
	std::shared_ptr<cainteoir::buffer> removed;

	normalized_testcase(const char *aPreserved, const char *aCollapsed)
		: preserved(std::make_shared<cainteoir::buffer>(aPreserved))
		, collapsed(std::make_shared<cainteoir::buffer>(aCollapsed))
	{
	}

	normalized_testcase(const char *aPreserved, const char *aCollapsed, const char *aRemoved)
		: preserved(std::make_shared<cainteoir::buffer>(aPreserved))
		, collapsed(std::make_shared<cainteoir::buffer>(aCollapsed))
		, removed(std::make_shared<cainteoir::buffer>(aRemoved))
	{
	}
};

static const std::initializer_list<normalized_testcase> left_whitespace =
{
	// U+0009 : CHARACTER TABULATION
	{ "\x09test", " test", "test" },
	{ "\x09\x09\x09test", " test", "test" },
	// U+000A : LINE FEED
	{ "\x0Atest", " test", "test" },
	{ "\x0A\x0A\x0Atest", " test", "test" },
	// U+000B : LINE TABULATION
	{ "\x0Btest", " test", "test" },
	{ "\x0B\x0B\x0Btest", " test", "test" },
	// U+000C : FORM FEED
	{ "\x0Ctest", " test", "test" },
	{ "\x0C\x0C\x0Ctest", " test", "test" },
	// U+000D : CARRIAGE RETURN
	{ "\x0Dtest", " test", "test" },
	{ "\x0D\x0D\x0Dtest", " test", "test" },
	// U+0020 : SPACE
	{ "\x20test", " test", "test" },
	{ "\x20\x20\x20test", " test", "test" },
	// U+0085 : NEXT LINE
	{ "\xC2\x85test", " test", "test" },
	{ "\xC2\x85\xC2\x85\xC2\x85test", " test", "test" },
	// U+00A0 : NO-BREAK SPACE
	{ "\xC2\xA0test", " test", "test" },
	{ "\xC2\xA0\xC2\xA0\xC2\xA0test", " test", "test" },
	// U+1680 : OGHAM SPACE MARK
	{ "\xE1\x9A\x80test", " test", "test" },
	{ "\xE1\x9A\x80\xE1\x9A\x80\xE1\x9A\x80test", " test", "test" },
	// U+2000 : EN QUAD
	{ "\xE2\x80\x80test", " test", "test" },
	{ "\xE2\x80\x80\xE2\x80\x80\xE2\x80\x80test", " test", "test" },
	// U+2001 : EM QUAD
	{ "\xE2\x80\x81test", " test", "test" },
	{ "\xE2\x80\x81\xE2\x80\x81\xE2\x80\x81test", " test", "test" },
	// U+2002 : EN SPACE
	{ "\xE2\x80\x82test", " test", "test" },
	{ "\xE2\x80\x82\xE2\x80\x82\xE2\x80\x82test", " test", "test" },
	// U+2003 : EM SPACE
	{ "\xE2\x80\x83test", " test", "test" },
	{ "\xE2\x80\x83\xE2\x80\x83\xE2\x80\x83test", " test", "test" },
	// U+2004 : THREE-PER-EM SPACE
	{ "\xE2\x80\x84test", " test", "test" },
	{ "\xE2\x80\x84\xE2\x80\x84\xE2\x80\x84test", " test", "test" },
	// U+2005 : FOUR-PER-EM SPACE
	{ "\xE2\x80\x85test", " test", "test" },
	{ "\xE2\x80\x85\xE2\x80\x85\xE2\x80\x85test", " test", "test" },
	// U+2006 : SIX-PER-EM SPACE
	{ "\xE2\x80\x86test", " test", "test" },
	{ "\xE2\x80\x86\xE2\x80\x86\xE2\x80\x86test", " test", "test" },
	// U+2007 : FIGURE SPACE
	{ "\xE2\x80\x87test", " test", "test" },
	{ "\xE2\x80\x87\xE2\x80\x87\xE2\x80\x87test", " test", "test" },
	// U+2008 : PUNCTUATION SPACE
	{ "\xE2\x80\x88test", " test", "test" },
	{ "\xE2\x80\x88\xE2\x80\x88\xE2\x80\x88test", " test", "test" },
	// U+2009 : THIN SPACE
	{ "\xE2\x80\x89test", " test", "test" },
	{ "\xE2\x80\x89\xE2\x80\x89\xE2\x80\x89test", " test", "test" },
	// U+200A : HAIR SPACE
	{ "\xE2\x80\x8Atest", " test", "test" },
	{ "\xE2\x80\x8A\xE2\x80\x8A\xE2\x80\x8Atest", " test", "test" },
	// U+2028 : LINE SEPARATOR
	{ "\xE2\x80\xA8test", " test", "test" },
	{ "\xE2\x80\xA8\xE2\x80\xA8\xE2\x80\xA8test", " test", "test" },
	// U+2029 : PARAGRAPH SEPARATOR
	{ "\xE2\x80\xA9test", " test", "test" },
	{ "\xE2\x80\xA9\xE2\x80\xA9\xE2\x80\xA9test", " test", "test" },
	// U+202F : NARROW NO-BREAK SPACE
	{ "\xE2\x80\xAFtest", " test", "test" },
	{ "\xE2\x80\xAF\xE2\x80\xAF\xE2\x80\xAFtest", " test", "test" },
	// U+205F : MEDIUM MATHEMATICAL SPACE
	{ "\xE2\x81\x9Ftest", " test", "test" },
	{ "\xE2\x81\x9F\xE2\x81\x9F\xE2\x81\x9Ftest", " test", "test" },
	// U+3000 : IDEOGRAPHIC SPACE
	{ "\xE3\x80\x80test", " test", "test" },
	{ "\xE3\x80\x80\xE3\x80\x80\xE3\x80\x80test", " test", "test" },
};

static const std::initializer_list<normalized_testcase> right_whitespace =
{
	// U+0009 : CHARACTER TABULATION
	{ "test\x09", "test ", "test" },
	{ "test\x09\x09\x09", "test ", "test" },
	// U+000A : LINE FEED
	{ "test\x0A", "test ", "test" },
	{ "test\x0A\x0A\x0A", "test ", "test" },
	// U+000B : LINE TABULATION
	{ "test\x0B", "test ", "test" },
	{ "test\x0B\x0B\x0B", "test ", "test" },
	// U+000C : FORM FEED
	{ "test\x0C", "test ", "test" },
	{ "test\x0C\x0C\x0C", "test ", "test" },
	// U+000D : CARRIAGE RETURN
	{ "test\x0D", "test ", "test" },
	{ "test\x0D\x0D\x0D", "test ", "test" },
	// U+0020 : SPACE
	{ "test\x20", "test ", "test" },
	{ "test\x20\x20\x20", "test ", "test" },
	// U+0085 : NEXT LINE
	{ "test\xC2\x85", "test ", "test" },
	{ "test\xC2\x85\xC2\x85\xC2\x85", "test ", "test" },
	// U+00A0 : NO-BREAK SPACE
	{ "test\xC2\xA0", "test ", "test" },
	{ "test\xC2\xA0\xC2\xA0\xC2\xA0", "test ", "test" },
	// U+1680 : OGHAM SPACE MARK
	{ "test\xE1\x9A\x80", "test ", "test" },
	{ "test\xE1\x9A\x80\xE1\x9A\x80\xE1\x9A\x80", "test ", "test" },
	// U+2000 : EN QUAD
	{ "test\xE2\x80\x80", "test ", "test" },
	{ "test\xE2\x80\x80\xE2\x80\x80\xE2\x80\x80", "test ", "test" },
	// U+2001 : EM QUAD
	{ "test\xE2\x80\x81", "test ", "test" },
	{ "test\xE2\x80\x81\xE2\x80\x81\xE2\x80\x81", "test ", "test" },
	// U+2002 : EN SPACE
	{ "test\xE2\x80\x82", "test ", "test" },
	{ "test\xE2\x80\x82\xE2\x80\x82\xE2\x80\x82", "test ", "test" },
	// U+2003 : EM SPACE
	{ "test\xE2\x80\x83", "test ", "test" },
	{ "test\xE2\x80\x83\xE2\x80\x83\xE2\x80\x83", "test ", "test" },
	// U+2004 : THREE-PER-EM SPACE
	{ "test\xE2\x80\x84", "test ", "test" },
	{ "test\xE2\x80\x84\xE2\x80\x84\xE2\x80\x84", "test ", "test" },
	// U+2005 : FOUR-PER-EM SPACE
	{ "test\xE2\x80\x85", "test ", "test" },
	{ "test\xE2\x80\x85\xE2\x80\x85\xE2\x80\x85", "test ", "test" },
	// U+2006 : SIX-PER-EM SPACE
	{ "test\xE2\x80\x86", "test ", "test" },
	{ "test\xE2\x80\x86\xE2\x80\x86\xE2\x80\x86", "test ", "test" },
	// U+2007 : FIGURE SPACE
	{ "test\xE2\x80\x87", "test ", "test" },
	{ "test\xE2\x80\x87\xE2\x80\x87\xE2\x80\x87", "test ", "test" },
	// U+2008 : PUNCTUATION SPACE
	{ "test\xE2\x80\x88", "test ", "test" },
	{ "test\xE2\x80\x88\xE2\x80\x88\xE2\x80\x88", "test ", "test" },
	// U+2009 : THIN SPACE
	{ "test\xE2\x80\x89", "test ", "test" },
	{ "test\xE2\x80\x89\xE2\x80\x89\xE2\x80\x89", "test ", "test" },
	// U+200A : HAIR SPACE
	{ "test\xE2\x80\x8A", "test ", "test" },
	{ "test\xE2\x80\x8A\xE2\x80\x8A\xE2\x80\x8A", "test ", "test" },
	// U+2028 : LINE SEPARATOR
	{ "test\xE2\x80\xA8", "test ", "test" },
	{ "test\xE2\x80\xA8\xE2\x80\xA8\xE2\x80\xA8", "test ", "test" },
	// U+2029 : PARAGRAPH SEPARATOR
	{ "test\xE2\x80\xA9", "test ", "test" },
	{ "test\xE2\x80\xA9\xE2\x80\xA9\xE2\x80\xA9", "test ", "test" },
	// U+202F : NARROW NO-BREAK SPACE
	{ "test\xE2\x80\xAF", "test ", "test" },
	{ "test\xE2\x80\xAF\xE2\x80\xAF\xE2\x80\xAF", "test ", "test" },
	// U+205F : MEDIUM MATHEMATICAL SPACE
	{ "test\xE2\x81\x9F", "test ", "test" },
	{ "test\xE2\x81\x9F\xE2\x81\x9F\xE2\x81\x9F", "test ", "test" },
	// U+3000 : IDEOGRAPHIC SPACE
	{ "test\xE3\x80\x80", "test ", "test" },
	{ "test\xE3\x80\x80\xE3\x80\x80\xE3\x80\x80", "test ", "test" },
};

static const std::initializer_list<normalized_testcase> middle_whitespace =
{
	// no space characters
	{ "test", "test" },
	// U+0009 : CHARACTER TABULATION
	{ "test\x09string", "test string" },
	{ "test\x09\x09\x09string", "test string" },
	// U+000A : LINE FEED
	{ "test\x0Astring", "test string" },
	{ "test\x0A\x0A\x0Astring", "test string" },
	// U+000B : LINE TABULATION
	{ "test\x0Bstring", "test string" },
	{ "test\x0B\x0B\x0Bstring", "test string" },
	// U+000C : FORM FEED
	{ "test\x0Cstring", "test string" },
	{ "test\x0C\x0C\x0Cstring", "test string" },
	// U+000D : CARRIAGE RETURN
	{ "test\x0Dstring", "test string" },
	{ "test\x0D\x0D\x0Dstring", "test string" },
	// U+0020 : SPACE
	{ "test\x20string", "test string" },
	{ "test\x20\x20\x20string", "test string" },
	// U+0085 : NEXT LINE
	{ "test\xC2\x85string", "test string" },
	{ "test\xC2\x85\xC2\x85\xC2\x85string", "test string" },
	// U+00A0 : NO-BREAK SPACE
	{ "test\xC2\xA0string", "test string" },
	{ "test\xC2\xA0\xC2\xA0\xC2\xA0string", "test string" },
	// U+1680 : OGHAM SPACE MARK
	{ "test\xE1\x9A\x80string", "test string" },
	{ "test\xE1\x9A\x80\xE1\x9A\x80\xE1\x9A\x80string", "test string" },
	// U+2000 : EN QUAD
	{ "test\xE2\x80\x80string", "test string" },
	{ "test\xE2\x80\x80\xE2\x80\x80\xE2\x80\x80string", "test string" },
	// U+2001 : EM QUAD
	{ "test\xE2\x80\x81string", "test string" },
	{ "test\xE2\x80\x81\xE2\x80\x81\xE2\x80\x81string", "test string" },
	// U+2002 : EN SPACE
	{ "test\xE2\x80\x82string", "test string" },
	{ "test\xE2\x80\x82\xE2\x80\x82\xE2\x80\x82string", "test string" },
	// U+2003 : EM SPACE
	{ "test\xE2\x80\x83string", "test string" },
	{ "test\xE2\x80\x83\xE2\x80\x83\xE2\x80\x83string", "test string" },
	// U+2004 : THREE-PER-EM SPACE
	{ "test\xE2\x80\x84string", "test string" },
	{ "test\xE2\x80\x84\xE2\x80\x84\xE2\x80\x84string", "test string" },
	// U+2005 : FOUR-PER-EM SPACE
	{ "test\xE2\x80\x85string", "test string" },
	{ "test\xE2\x80\x85\xE2\x80\x85\xE2\x80\x85string", "test string" },
	// U+2006 : SIX-PER-EM SPACE
	{ "test\xE2\x80\x86string", "test string" },
	{ "test\xE2\x80\x86\xE2\x80\x86\xE2\x80\x86string", "test string" },
	// U+2007 : FIGURE SPACE
	{ "test\xE2\x80\x87string", "test string" },
	{ "test\xE2\x80\x87\xE2\x80\x87\xE2\x80\x87string", "test string" },
	// U+2008 : PUNCTUATION SPACE
	{ "test\xE2\x80\x88string", "test string" },
	{ "test\xE2\x80\x88\xE2\x80\x88\xE2\x80\x88string", "test string" },
	// U+2009 : THIN SPACE
	{ "test\xE2\x80\x89string", "test string" },
	{ "test\xE2\x80\x89\xE2\x80\x89\xE2\x80\x89string", "test string" },
	// U+200A : HAIR SPACE
	{ "test\xE2\x80\x8Astring", "test string" },
	{ "test\xE2\x80\x8A\xE2\x80\x8A\xE2\x80\x8Astring", "test string" },
	// U+2028 : LINE SEPARATOR
	{ "test\xE2\x80\xA8string", "test string" },
	{ "test\xE2\x80\xA8\xE2\x80\xA8\xE2\x80\xA8string", "test string" },
	// U+2029 : PARAGRAPH SEPARATOR
	{ "test\xE2\x80\xA9string", "test string" },
	{ "test\xE2\x80\xA9\xE2\x80\xA9\xE2\x80\xA9string", "test string" },
	// U+202F : NARROW NO-BREAK SPACE
	{ "test\xE2\x80\xAFstring", "test string" },
	{ "test\xE2\x80\xAF\xE2\x80\xAF\xE2\x80\xAFstring", "test string" },
	// U+205F : MEDIUM MATHEMATICAL SPACE
	{ "test\xE2\x81\x9Fstring", "test string" },
	{ "test\xE2\x81\x9F\xE2\x81\x9F\xE2\x81\x9Fstring", "test string" },
	// U+3000 : IDEOGRAPHIC SPACE
	{ "test\xE3\x80\x80string", "test string" },
	{ "test\xE3\x80\x80\xE3\x80\x80\xE3\x80\x80string", "test string" },
};

static const std::initializer_list<normalized_testcase> whitespace_only =
{
	// U+0009 : CHARACTER TABULATION
	{ "\x09", " ", "" },
	{ "\x09\x09\x09", " ", "" },
	// U+000A : LINE FEED
	{ "\x0A", " ", "" },
	{ "\x0A\x0A\x0A", " ", "" },
	// U+000B : LINE TABULATION
	{ "\x0B", " ", "" },
	{ "\x0B\x0B\x0B", " ", "" },
	// U+000C : FORM FEED
	{ "\x0C", " ", "" },
	{ "\x0C\x0C\x0C", " ", "" },
	// U+000D : CARRIAGE RETURN
	{ "\x0D", " ", "" },
	{ "\x0D\x0D\x0D", " ", "" },
	// U+0020 : SPACE
	{ "\x20", " ", "" },
	{ "\x20\x20\x20", " ", "" },
	// U+0085 : NEXT LINE
	{ "\xC2\x85", " ", "" },
	{ "\xC2\x85\xC2\x85\xC2\x85", " ", "" },
	// U+00A0 : NO-BREAK SPACE
	{ "\xC2\xA0", " ", "" },
	{ "\xC2\xA0\xC2\xA0\xC2\xA0", " ", "" },
	// U+1680 : OGHAM SPACE MARK
	{ "\xE1\x9A\x80", " ", "" },
	{ "\xE1\x9A\x80\xE1\x9A\x80\xE1\x9A\x80", " ", "" },
	// U+2000 : EN QUAD
	{ "\xE2\x80\x80", " ", "" },
	{ "\xE2\x80\x80\xE2\x80\x80\xE2\x80\x80", " ", "" },
	// U+2001 : EM QUAD
	{ "\xE2\x80\x81", " ", "" },
	{ "\xE2\x80\x81\xE2\x80\x81\xE2\x80\x81", " ", "" },
	// U+2002 : EN SPACE
	{ "\xE2\x80\x82", " ", "" },
	{ "\xE2\x80\x82\xE2\x80\x82\xE2\x80\x82", " ", "" },
	// U+2003 : EM SPACE
	{ "\xE2\x80\x83", " ", "" },
	{ "\xE2\x80\x83\xE2\x80\x83\xE2\x80\x83", " ", "" },
	// U+2004 : THREE-PER-EM SPACE
	{ "\xE2\x80\x84", " ", "" },
	{ "\xE2\x80\x84\xE2\x80\x84\xE2\x80\x84", " ", "" },
	// U+2005 : FOUR-PER-EM SPACE
	{ "\xE2\x80\x85", " ", "" },
	{ "\xE2\x80\x85\xE2\x80\x85\xE2\x80\x85", " ", "" },
	// U+2006 : SIX-PER-EM SPACE
	{ "\xE2\x80\x86", " ", "" },
	{ "\xE2\x80\x86\xE2\x80\x86\xE2\x80\x86", " ", "" },
	// U+2007 : FIGURE SPACE
	{ "\xE2\x80\x87", " ", "" },
	{ "\xE2\x80\x87\xE2\x80\x87\xE2\x80\x87", " ", "" },
	// U+2008 : PUNCTUATION SPACE
	{ "\xE2\x80\x88", " ", "" },
	{ "\xE2\x80\x88\xE2\x80\x88\xE2\x80\x88", " ", "" },
	// U+2009 : THIN SPACE
	{ "\xE2\x80\x89", " ", "" },
	{ "\xE2\x80\x89\xE2\x80\x89\xE2\x80\x89", " ", "" },
	// U+200A : HAIR SPACE
	{ "\xE2\x80\x8A", " ", "" },
	{ "\xE2\x80\x8A\xE2\x80\x8A\xE2\x80\x8A", " ", "" },
	// U+2028 : LINE SEPARATOR
	{ "\xE2\x80\xA8", " ", "" },
	{ "\xE2\x80\xA8\xE2\x80\xA8\xE2\x80\xA8", " ", "" },
	// U+2029 : PARAGRAPH SEPARATOR
	{ "\xE2\x80\xA9", " ", "" },
	{ "\xE2\x80\xA9\xE2\x80\xA9\xE2\x80\xA9", " ", "" },
	// U+202F : NARROW NO-BREAK SPACE
	{ "\xE2\x80\xAF", " ", "" },
	{ "\xE2\x80\xAF\xE2\x80\xAF\xE2\x80\xAF", " ", "" },
	// U+205F : MEDIUM MATHEMATICAL SPACE
	{ "\xE2\x81\x9F", " ", "" },
	{ "\xE2\x81\x9F\xE2\x81\x9F\xE2\x81\x9F", " ", "" },
	// U+3000 : IDEOGRAPHIC SPACE
	{ "\xE3\x80\x80", " ", "" },
	{ "\xE3\x80\x80\xE3\x80\x80\xE3\x80\x80", " ", "" },
};

TEST_CASE("cainteoir::normalize")
{
	for (const auto &test : left_whitespace)
		match(cainteoir::normalize(test.preserved),
		      test.removed->begin(), test.removed->size());

	for (const auto &test : right_whitespace)
		match(cainteoir::normalize(test.preserved),
		      test.removed->begin(), test.removed->size());

	for (const auto &test : middle_whitespace)
		match(cainteoir::normalize(test.preserved),
		      test.collapsed->begin(), test.collapsed->size());

	for (const auto &test : whitespace_only)
		match(cainteoir::normalize(test.preserved),
		      test.removed->begin(), test.removed->size());
}

TEST_CASE("cainteoir::normalize -- left=collapse, right=collapse, space=normal")
{
	auto ws    = cainteoir::whitespace::collapse;
	auto nl    = cainteoir::whitespace::collapse;
	auto left  = cainteoir::whitespace::collapse;
	auto right = cainteoir::whitespace::collapse;

	for (const auto &test : left_whitespace)
		match(cainteoir::normalize(test.preserved, ws, nl, left, right),
		      test.removed->begin(), test.removed->size());

	for (const auto &test : right_whitespace)
		match(cainteoir::normalize(test.preserved, ws, nl, left, right),
		      test.removed->begin(), test.removed->size());

	for (const auto &test : middle_whitespace)
		match(cainteoir::normalize(test.preserved, ws, nl, left, right),
		      test.collapsed->begin(), test.collapsed->size());

	for (const auto &test : whitespace_only)
		match(cainteoir::normalize(test.preserved, ws, nl, left, right),
		      test.removed->begin(), test.removed->size());
}

TEST_CASE("cainteoir::normalize -- left=collapse, right=preserve, space=normal")
{
	auto ws    = cainteoir::whitespace::collapse;
	auto nl    = cainteoir::whitespace::collapse;
	auto left  = cainteoir::whitespace::collapse;
	auto right = cainteoir::whitespace::preserve;

	for (const auto &test : left_whitespace)
		match(cainteoir::normalize(test.preserved, ws, nl, left, right),
		      test.removed->begin(), test.removed->size());

	for (const auto &test : right_whitespace)
		match(cainteoir::normalize(test.preserved, ws, nl, left, right),
		      test.collapsed->begin(), test.collapsed->size());

	for (const auto &test : middle_whitespace)
		match(cainteoir::normalize(test.preserved, ws, nl, left, right),
		      test.collapsed->begin(), test.collapsed->size());

	for (const auto &test : whitespace_only)
		match(cainteoir::normalize(test.preserved, ws, nl, left, right),
		      test.removed->begin(), test.removed->size());
}

TEST_CASE("cainteoir::normalize -- left=preserve, right=collapse, space=pre")
{
	auto ws    = cainteoir::whitespace::preserve;
	auto nl    = cainteoir::whitespace::preserve;
	auto left  = cainteoir::whitespace::preserve;
	auto right = cainteoir::whitespace::collapse;

	for (const auto &test : left_whitespace)
		match(cainteoir::normalize(test.preserved, ws, nl, left, right),
		      test.preserved->begin(), test.preserved->size());

	for (const auto &test : right_whitespace)
		match(cainteoir::normalize(test.preserved, ws, nl, left, right),
		      test.removed->begin(), test.removed->size());

	for (const auto &test : middle_whitespace)
		match(cainteoir::normalize(test.preserved, ws, nl, left, right),
		      test.preserved->begin(), test.preserved->size());

	for (const auto &test : whitespace_only)
		match(cainteoir::normalize(test.preserved, ws, nl, left, right),
		      test.removed->begin(), test.removed->size());
}
