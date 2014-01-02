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
		// syllable breaks
		{ "p'End.IN",    "p'EndIN" },
		{ "sw'Im.INl,i", "sw'ImINl,i" },
		{ "d@.naI_^.IN", "d@naI_^IN" },
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

TEST_CASE("syllable stress")
{
	static const std::pair<const char *, const char *> testcases[] =
	{
		// one syllable -- 'CVC
		// vowel          | rising diphthong        | falling diphthong
		{ "'hIm", "'hIm" }, { "'g@U_^t", "'g@U_^t" }, { "'rI@_^d", "'rI@_^d" },
		{ "h'Im", "'hIm" }, { "g'@U_^t", "'g@U_^t" }, { "r'I@_^d", "'rI@_^d" },
		{ "hIm",  "hIm"  }, { "g@U_^t",  "g@U_^t"  }, { "rI@_^d",  "rI@_^d"  },
		{ ",hIm", ",hIm" }, { ",g@U_^t", ",g@U_^t" }, { ",rI@_^d", ",rI@_^d" },
		{ "h,Im", ",hIm" }, { "g,@U_^t", ",g@U_^t" }, { "r,I@_^d", ",rI@_^d" },
		// one syllable -- 'CCCVC
		// vowel              | rising diphthong            | falling diphthong
		{ "'kghIm", "'kghIm" }, { "'brg@U_^t", "'brg@U_^t" }, { "'strI@_^d", "'strI@_^d" },
		{ "kgh'Im", "'kghIm" }, { "brg'@U_^t", "'brg@U_^t" }, { "str'I@_^d", "'strI@_^d" },
		{ "kghIm",  "kghIm"  }, { "brg@U_^t",  "brg@U_^t"  }, { "strI@_^d",  "strI@_^d"  },
		{ ",kghIm", ",kghIm" }, { ",brg@U_^t", ",brg@U_^t" }, { ",strI@_^d", ",strI@_^d" },
		{ "kgh,Im", ",kghIm" }, { "brg,@U_^t", ",brg@U_^t" }, { "str,I@_^d", ",strI@_^d" },
		// two syllables -- 'CV.CVC
		{ "t'at3:d", "'tat3:d" },
		{ "t,at3:d", ",tat3:d" },
		{ "tat'3:d", "ta't3:d" },
		{ "tat,3:d", "ta,t3:d" },
		{ "t'a.t3:d", "'ta.t3:d" },
		{ "t,a.t3:d", ",ta.t3:d" },
		// two syllables -- 'CVC.CVC
		{ "s'adl3:z", "'sadl3:z" },
		{ "s,adl3:z", ",sadl3:z" },
		{ "sadl'3:z", "sad'l3:z" },
		{ "sadl,3:z", "sad,l3:z" },
		{ "s'ad.l3:z", "'sad.l3:z" },
		{ "s,ad.l3:z", ",sad.l3:z" },
		// syllabic consonant -- 'CVC.C,VC
		{ "b'Vtn=,IN", "'bVtn=,IN" },
		{ "b,Vtn='IN", ",bVtn='IN" },
		{ "b'Vt.n=,IN", "'bVt.n=,IN" },
		{ "b,Vt.n='IN", ",bVt.n='IN" },
	};

	auto reader = tts::createPhonemeReader("cxs");
	auto writer = tts::createPhonemeWriter("cxs");

	for (const auto &test : testcases)
	{
		std::list<tts::phoneme> phonemes;
		reader->reset(std::make_shared<cainteoir::buffer>(test.first));
		while (reader->read())
			phonemes.push_back(*reader);

		tts::make_syllable_stressed(phonemes);

		cainteoir::memory_file output;
		writer->reset(output);
		for (const auto &phoneme : phonemes)
			writer->write(phoneme);

		assert(output.buffer()->str() == test.second);
	}
}
