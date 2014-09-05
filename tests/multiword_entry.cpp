/* Test for the multiword_entry API.
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

#include <cainteoir/dictionary.hpp>
#include <cstdio>

#include "tester.hpp"

namespace tts = cainteoir::tts;

REGISTER_TESTSUITE("multiword_entry");

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

TEST_CASE("hyphenated : 1 word")
{
	tts::multiword_entry entry{ std::make_shared<cainteoir::buffer>("test"),
	                            tts::multiword_entry::hyphenated };
	assert(!entry.is_multiword());

	assert(entry.have_word());
	match((*entry).word, "test", 4);
	assert((*entry).stress == tts::initial_stress::as_transcribed);
	assert(entry.position() == 0);

	++entry;
	assert(!entry.have_word());
	assert(!(*entry).word.get());
	assert((*entry).stress == tts::initial_stress::as_transcribed);
	assert(entry.position() == 1);
}

TEST_CASE("hyphenated : 2 words")
{
	tts::multiword_entry entry{ std::make_shared<cainteoir::buffer>("first-second"),
	                            tts::multiword_entry::hyphenated };
	assert(entry.is_multiword());

	assert(entry.have_word());
	match((*entry).word, "first", 5);
	assert((*entry).stress == tts::initial_stress::as_transcribed);
	assert(entry.position() == 0);

	++entry;
	assert(entry.have_word());
	match((*entry).word, "second", 6);
	assert((*entry).stress == tts::initial_stress::as_transcribed);
	assert(entry.position() == 1);

	++entry;
	assert(!entry.have_word());
	assert(!(*entry).word.get());
	assert((*entry).stress == tts::initial_stress::as_transcribed);
	assert(entry.position() == 2);
}

TEST_CASE("hyphenated : 3 words")
{
	tts::multiword_entry entry{ std::make_shared<cainteoir::buffer>("this-and-that"),
	                            tts::multiword_entry::hyphenated };
	assert(entry.is_multiword());

	assert(entry.have_word());
	match((*entry).word, "this", 4);
	assert((*entry).stress == tts::initial_stress::as_transcribed);
	assert(entry.position() == 0);

	++entry;
	assert(entry.have_word());
	match((*entry).word, "and", 3);
	assert((*entry).stress == tts::initial_stress::as_transcribed);
	assert(entry.position() == 1);

	++entry;
	assert(entry.have_word());
	match((*entry).word, "that", 4);
	assert((*entry).stress == tts::initial_stress::as_transcribed);
	assert(entry.position() == 2);

	++entry;
	assert(!entry.have_word());
	assert(!(*entry).word.get());
	assert((*entry).stress == tts::initial_stress::as_transcribed);
	assert(entry.position() == 3);
}

TEST_CASE("hyphenated : apostrophe")
{
	tts::multiword_entry entry{ std::make_shared<cainteoir::buffer>("they're"),
	                            tts::multiword_entry::hyphenated };
	assert(!entry.is_multiword());

	assert(entry.have_word());
	match((*entry).word, "they're", 7);
	assert((*entry).stress == tts::initial_stress::as_transcribed);
	assert(entry.position() == 0);

	++entry;
	assert(!entry.have_word());
	assert(!(*entry).word.get());
	assert((*entry).stress == tts::initial_stress::as_transcribed);
	assert(entry.position() == 1);
}

TEST_CASE("hyphenated : stress markers")
{
	tts::multiword_entry entry{ std::make_shared<cainteoir::buffer>(".one,two"),
	                            tts::multiword_entry::hyphenated };
	assert(!entry.is_multiword());

	assert(entry.have_word());
	match((*entry).word, ".one,two", 8);
	assert((*entry).stress == tts::initial_stress::as_transcribed);
	assert(entry.position() == 0);

	++entry;
	assert(!entry.have_word());
	assert(!(*entry).word.get());
	assert((*entry).stress == tts::initial_stress::as_transcribed);
	assert(entry.position() == 1);
}

TEST_CASE("stressed : 1 word [no stress]")
{
	tts::multiword_entry entry{ std::make_shared<cainteoir::buffer>("test"),
	                            tts::multiword_entry::stressed };
	assert(!entry.is_multiword());

	assert(entry.have_word());
	match((*entry).word, "test", 4);
	assert((*entry).stress == tts::initial_stress::as_transcribed);
	assert(entry.position() == 0);

	++entry;
	assert(!entry.have_word());
	assert(!(*entry).word.get());
	assert((*entry).stress == tts::initial_stress::as_transcribed);
	assert(entry.position() == 1);
}

TEST_CASE("stressed : 1 word [primary-stressed]")
{
	tts::multiword_entry entry{ std::make_shared<cainteoir::buffer>("'one"),
	                            tts::multiword_entry::stressed };
	assert(!entry.is_multiword());

	assert(entry.have_word());
	match((*entry).word, "one", 3);
	assert((*entry).stress == tts::initial_stress::primary);
	assert(entry.position() == 0);

	++entry;
	assert(!entry.have_word());
	assert(!(*entry).word.get());
	assert((*entry).stress == tts::initial_stress::as_transcribed);
	assert(entry.position() == 1);
}

TEST_CASE("stressed : 1 word [secondary-stressed]")
{
	tts::multiword_entry entry{ std::make_shared<cainteoir::buffer>(",one"),
	                            tts::multiword_entry::stressed };
	assert(!entry.is_multiword());

	assert(entry.have_word());
	match((*entry).word, "one", 3);
	assert((*entry).stress == tts::initial_stress::secondary);
	assert(entry.position() == 0);

	++entry;
	assert(!entry.have_word());
	assert(!(*entry).word.get());
	assert((*entry).stress == tts::initial_stress::as_transcribed);
	assert(entry.position() == 1);
}

TEST_CASE("stressed : 1 word [unstressed]")
{
	tts::multiword_entry entry{ std::make_shared<cainteoir::buffer>(".one"),
	                            tts::multiword_entry::stressed };
	assert(!entry.is_multiword());

	assert(entry.have_word());
	match((*entry).word, "one", 3);
	assert((*entry).stress == tts::initial_stress::unstressed);
	assert(entry.position() == 0);

	++entry;
	assert(!entry.have_word());
	assert(!(*entry).word.get());
	assert((*entry).stress == tts::initial_stress::as_transcribed);
	assert(entry.position() == 1);
}

TEST_CASE("stressed : milti-word [primary-stressed]")
{
	tts::multiword_entry entry{ std::make_shared<cainteoir::buffer>("one'two"),
	                            tts::multiword_entry::stressed };
	assert(entry.is_multiword());

	assert(entry.have_word());
	match((*entry).word, "one", 3);
	assert((*entry).stress == tts::initial_stress::as_transcribed);
	assert(entry.position() == 0);

	++entry;
	assert(entry.have_word());
	match((*entry).word, "two", 3);
	assert((*entry).stress == tts::initial_stress::primary);
	assert(entry.position() == 1);

	++entry;
	assert(!entry.have_word());
	assert(!(*entry).word.get());
	assert((*entry).stress == tts::initial_stress::as_transcribed);
	assert(entry.position() == 2);
}

TEST_CASE("stressed : multi-word [secondary-stressed]")
{
	tts::multiword_entry entry{ std::make_shared<cainteoir::buffer>("one,two"),
	                            tts::multiword_entry::stressed };
	assert(entry.is_multiword());

	assert(entry.have_word());
	match((*entry).word, "one", 3);
	assert((*entry).stress == tts::initial_stress::as_transcribed);
	assert(entry.position() == 0);

	++entry;
	assert(entry.have_word());
	match((*entry).word, "two", 3);
	assert((*entry).stress == tts::initial_stress::secondary);
	assert(entry.position() == 1);

	++entry;
	assert(!entry.have_word());
	assert(!(*entry).word.get());
	assert((*entry).stress == tts::initial_stress::as_transcribed);
	assert(entry.position() == 2);
}

TEST_CASE("stressed : multi-word [unstressed]")
{
	tts::multiword_entry entry{ std::make_shared<cainteoir::buffer>("one.two"),
	                            tts::multiword_entry::stressed };
	assert(entry.is_multiword());

	assert(entry.have_word());
	match((*entry).word, "one", 3);
	assert((*entry).stress == tts::initial_stress::as_transcribed);
	assert(entry.position() == 0);

	++entry;
	assert(entry.have_word());
	match((*entry).word, "two", 3);
	assert((*entry).stress == tts::initial_stress::unstressed);
	assert(entry.position() == 1);

	++entry;
	assert(!entry.have_word());
	assert(!(*entry).word.get());
	assert((*entry).stress == tts::initial_stress::as_transcribed);
	assert(entry.position() == 2);
}
