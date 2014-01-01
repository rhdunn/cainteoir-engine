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

TEST_CASE("vowel stress")
{
	static const std::pair<const char *, const char *> testcases[] =
	{
		// vowel - primary
		{ "p'Ed", "p'Ed" }, // on a vowel -- no change
		{ "'pEd", "p'Ed" }, // on a consonant -- move to next vowel
		{ "'strEd", "str'Ed" }, // on a consonant -- move to next vowel
		// vowel - secondary
		{ "p,Ed", "p,Ed" }, // on a vowel -- no change
		{ ",pEd", "p,Ed" }, // on a consonant -- move to next vowel
		{ ",strEd", "str,Ed" }, // on a consonant -- move to next vowel
		// diphthong (rising) - primary
		{ "p'eI_^d", "p'eI_^d" }, // on a vowel -- no change
		{ "'peI_^d", "p'eI_^d" }, // on a consonant -- move to next vowel
		{ "'streI_^d", "str'eI_^d" }, // on a consonant -- move to next vowel
		// diphthong (rising) - secondary
		{ "p,eI_^d", "p,eI_^d" }, // on a vowel -- no change
		{ ",peI_^d", "p,eI_^d" }, // on a consonant -- move to next vowel
		{ ",streI_^d", "str,eI_^d" }, // on a consonant -- move to next vowel
		// diphthong (falling) - primary
		{ "p'U@_^d", "p'U@_^d" }, // on a vowel -- no change
		{ "'pU@_^d", "p'U@_^d" }, // on a consonant -- move to next vowel
		{ "'strU@_^d", "str'U@_^d" }, // on a consonant -- move to next vowel
		// diphthong (falling) - secondary
		{ "p,U@_^d", "p,U@_^d" }, // on a vowel -- no change
		{ ",pU@_^d", "p,U@_^d" }, // on a consonant -- move to next vowel
		{ ",strU@_^d", "str,U@_^d" }, // on a consonant -- move to next vowel
	};

	auto reader = tts::createPhonemeReader("cxs");
	auto writer = tts::createPhonemeWriter("cxs");

	for (const auto &test : testcases)
	{
		std::list<tts::phoneme> phonemes;
		reader->reset(std::make_shared<cainteoir::buffer>(test.first));
		while (reader->read())
			phonemes.push_back(*reader);

		tts::make_vowel_stressed(phonemes);

		cainteoir::memory_file output;
		writer->reset(output);
		for (const auto &phoneme : phonemes)
			writer->write(phoneme);

		assert(output.buffer()->str() == test.second);
	}
}
