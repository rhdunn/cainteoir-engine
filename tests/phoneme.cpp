/* Test for the phoneme model.
 *
 * Copyright (C) 2013 Reece H. Dunn
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

REGISTER_TESTSUITE("phoneme");

namespace tts = cainteoir::tts;

typedef tts::feature f;

TEST_CASE("phoneme object size")
{
	assert(sizeof(tts::phoneme) == 4);
}

TEST_CASE("3 feature phoneme")
{
	tts::phoneme p(f::voiceless, f::bilabial, f::plosive);

	assert(p.contains(f::voiceless));
	assert(p.contains(f::bilabial));
	assert(p.contains(f::plosive));

	assert(!p.contains(f::unspecified));
	assert(!p.contains(f::voiced));
	assert(!p.contains(f::vowel));
}

TEST_CASE("4 feature phoneme")
{
	tts::phoneme p(f::high, f::front, f::unrounded, f::vowel);

	assert(p.contains(f::high));
	assert(p.contains(f::front));
	assert(p.contains(f::unrounded));
	assert(p.contains(f::vowel));

	assert(!p.contains(f::unspecified));
	assert(!p.contains(f::rounded));
	assert(!p.contains(f::voiceless));
}

TEST_CASE("copy construction")
{
	tts::phoneme a(f::voiced, f::dental, f::fricative);
	tts::phoneme b(a);

	assert(b.contains(f::voiced));
	assert(b.contains(f::dental));
	assert(b.contains(f::fricative));

	assert(!b.contains(f::unspecified));
	assert(!b.contains(f::voiceless));
	assert(!b.contains(f::vowel));

	tts::phoneme c(f::low, f::back, f::rounded, f::vowel);
	tts::phoneme d(c);

	assert(d.contains(f::low));
	assert(d.contains(f::back));
	assert(d.contains(f::rounded));
	assert(d.contains(f::vowel));

	assert(!d.contains(f::unspecified));
	assert(!d.contains(f::unrounded));
	assert(!d.contains(f::voiceless));
}

TEST_CASE("copy assignment")
{
	tts::phoneme a(f::voiced, f::dental, f::fricative);
	a = tts::phoneme(f::voiceless, f::bilabial, f::plosive);

	assert(a.contains(f::voiceless));
	assert(a.contains(f::bilabial));
	assert(a.contains(f::plosive));

	assert(!a.contains(f::unspecified));
	assert(!a.contains(f::voiced));
	assert(!a.contains(f::vowel));

	a = { f::voiced, f::dental, f::fricative };

	assert(a.contains(f::voiced));
	assert(a.contains(f::dental));
	assert(a.contains(f::fricative));

	assert(!a.contains(f::unspecified));
	assert(!a.contains(f::voiceless));
	assert(!a.contains(f::vowel));

	tts::phoneme b(f::low, f::back, f::rounded, f::vowel);
	b = tts::phoneme(f::high, f::front, f::unrounded, f::vowel);

	assert(b.contains(f::high));
	assert(b.contains(f::front));
	assert(b.contains(f::unrounded));
	assert(b.contains(f::vowel));

	assert(!b.contains(f::unspecified));
	assert(!b.contains(f::rounded));
	assert(!b.contains(f::voiceless));

	b = { f::low, f::back, f::rounded, f::vowel };

	assert(b.contains(f::low));
	assert(b.contains(f::back));
	assert(b.contains(f::rounded));
	assert(b.contains(f::vowel));

	assert(!b.contains(f::unspecified));
	assert(!b.contains(f::unrounded));
	assert(!b.contains(f::voiceless));
}
