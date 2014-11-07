/* Test for the phoneme stress patterns.
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

#include <cainteoir/phoneme.hpp>
#include <cstdio>

#include "tester.hpp"

REGISTER_TESTSUITE("phoneme stress");

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;

TEST_CASE("as transcribed [initial stress]")
{
	static const char *testcases[] =
	{
		"'maI_^",
		",maI_^",
		"maI_^",
	};

	auto reader = tts::createPhonemeReader("cxs");
	auto writer = tts::createPhonemeWriter("cxs");

	for (const auto &test : testcases)
	{
		ipa::phonemes phonemes;
		reader->reset(std::make_shared<cainteoir::buffer>(test));
		while (reader->read())
			phonemes.push_back(*reader);

		ipa::phonemes result;
		tts::make_stressed(phonemes, result, tts::initial_stress::as_transcribed);

		cainteoir::memory_file output;
		writer->reset(output);
		for (const auto &phoneme : result)
			writer->write(phoneme);

		assert(output.buffer()->str() == test);
	}
}

TEST_CASE("primary [initial stress]")
{
	static const std::pair<const char *, const char *> testcases[] =
	{
		{ "'maI_^", "'maI_^" },
		{ ",maI_^", "'maI_^" },
		{ "maI_^",  "'maI_^" },
	};

	auto reader = tts::createPhonemeReader("cxs");
	auto writer = tts::createPhonemeWriter("cxs");

	for (const auto &test : testcases)
	{
		ipa::phonemes phonemes;
		reader->reset(std::make_shared<cainteoir::buffer>(test.first));
		while (reader->read())
			phonemes.push_back(*reader);

		ipa::phonemes result;
		tts::make_stressed(phonemes, result, tts::initial_stress::primary);

		cainteoir::memory_file output;
		writer->reset(output);
		for (const auto &phoneme : result)
			writer->write(phoneme);

		assert(output.buffer()->str() == test.second);
	}
}

TEST_CASE("secondary [initial stress]")
{
	static const std::pair<const char *, const char *> testcases[] =
	{
		{ "'maI_^", ",maI_^" },
		{ ",maI_^", ",maI_^" },
		{ "maI_^",  ",maI_^" },
	};

	auto reader = tts::createPhonemeReader("cxs");
	auto writer = tts::createPhonemeWriter("cxs");

	for (const auto &test : testcases)
	{
		ipa::phonemes phonemes;
		reader->reset(std::make_shared<cainteoir::buffer>(test.first));
		while (reader->read())
			phonemes.push_back(*reader);

		ipa::phonemes result;
		tts::make_stressed(phonemes, result, tts::initial_stress::secondary);

		cainteoir::memory_file output;
		writer->reset(output);
		for (const auto &phoneme : result)
			writer->write(phoneme);

		assert(output.buffer()->str() == test.second);
	}
}

TEST_CASE("unstressed [initial stress]")
{
	static const std::pair<const char *, const char *> testcases[] =
	{
		{ "'maI_^", "maI_^" },
		{ ",maI_^", "maI_^" },
		{ "maI_^",  "maI_^" },
	};

	auto reader = tts::createPhonemeReader("cxs");
	auto writer = tts::createPhonemeWriter("cxs");

	for (const auto &test : testcases)
	{
		ipa::phonemes phonemes;
		reader->reset(std::make_shared<cainteoir::buffer>(test.first));
		while (reader->read())
			phonemes.push_back(*reader);

		ipa::phonemes result;
		tts::make_stressed(phonemes, result, tts::initial_stress::unstressed);

		cainteoir::memory_file output;
		writer->reset(output);
		for (const auto &phoneme : result)
			writer->write(phoneme);

		assert(output.buffer()->str() == test.second);
	}
}
