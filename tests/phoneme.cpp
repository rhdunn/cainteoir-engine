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

	// copy construction ...

	tts::phoneme q(p);

	assert(q.contains(f::voiceless));
	assert(q.contains(f::bilabial));
	assert(q.contains(f::plosive));

	assert(!q.contains(f::unspecified));
	assert(!q.contains(f::voiced));
	assert(!q.contains(f::vowel));

	// copy assignment ...

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

	// copy construction ...

	tts::phoneme q(p);

	assert(q.contains(f::high));
	assert(q.contains(f::front));
	assert(q.contains(f::unrounded));
	assert(q.contains(f::vowel));

	assert(!q.contains(f::unspecified));
	assert(!q.contains(f::rounded));
	assert(!q.contains(f::voiceless));

	// copy assignment ...

	tts::phoneme a(f::voiced, f::bilabial, f::lateral, f::fricative);
	a = tts::phoneme(f::high, f::front, f::unrounded, f::vowel);

	assert(a.contains(f::high));
	assert(a.contains(f::front));
	assert(a.contains(f::unrounded));
	assert(a.contains(f::vowel));

	assert(!a.contains(f::unspecified));
	assert(!a.contains(f::rounded));
	assert(!a.contains(f::voiceless));

	a = { f::low, f::back, f::rounded, f::vowel };

	assert(a.contains(f::low));
	assert(a.contains(f::back));
	assert(a.contains(f::rounded));
	assert(a.contains(f::vowel));

	assert(!a.contains(f::unspecified));
	assert(!a.contains(f::unrounded));
	assert(!a.contains(f::voiceless));
}

TEST_CASE("phoneme equality")
{
	assert(tts::phoneme(f::unspecified, f::unspecified, f::unspecified, f::unspecified)
	    == tts::phoneme(f::unspecified, f::unspecified, f::unspecified, f::unspecified));

	assert(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel)
	    == tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel));

	assert(!(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel)
	      == tts::phoneme(f::high,      f::back, f::unrounded, f::vowel)));

	assert(!(tts::phoneme(f::semi_high, f::back,  f::unrounded, f::vowel)
	      == tts::phoneme(f::semi_high, f::front, f::unrounded, f::vowel)));

	assert(!(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel)
	      == tts::phoneme(f::semi_high, f::back, f::rounded,   f::vowel)));

	assert(!(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel)
	      == tts::phoneme(f::semi_high, f::back, f::unrounded, f::lateral)));
}

TEST_CASE("phoneme inequality")
{
	assert(!(tts::phoneme(f::unspecified, f::unspecified, f::unspecified, f::unspecified)
	      != tts::phoneme(f::unspecified, f::unspecified, f::unspecified, f::unspecified)));

	assert(!(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel)
	      != tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel)));

	assert(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel)
	    != tts::phoneme(f::high,      f::back, f::unrounded, f::vowel));

	assert(tts::phoneme(f::semi_high, f::back,  f::unrounded, f::vowel)
	    != tts::phoneme(f::semi_high, f::front, f::unrounded, f::vowel));

	assert(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel)
	    != tts::phoneme(f::semi_high, f::back, f::rounded,   f::vowel));

	assert(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel)
	    != tts::phoneme(f::semi_high, f::back, f::unrounded, f::lateral));
}

TEST_CASE("tts::get_feature_id -- invalid abbreviations")
{
	assert(tts::get_feature_id(nullptr) == f::unspecified);
	assert(tts::get_feature_id("xyz") == f::unspecified);
	assert(tts::get_feature_id("aspirated") == f::unspecified);
}

TEST_CASE("tts::get_feature_id -- ascii-ipa abbreviations")
{
	assert(tts::get_feature_id("vcd") == f::voiced);
	assert(tts::get_feature_id("vls") == f::voiceless);

	assert(tts::get_feature_id("blb") == f::bilabial);
	assert(tts::get_feature_id("lbd") == f::labio_dental);
	assert(tts::get_feature_id("dnt") == f::dental);
	assert(tts::get_feature_id("alv") == f::alveolar);
	assert(tts::get_feature_id("rfx") == f::retroflex);
	assert(tts::get_feature_id("pla") == f::palato_alveolar);
	assert(tts::get_feature_id("pal") == f::palatal);
	assert(tts::get_feature_id("vel") == f::velar);
	assert(tts::get_feature_id("lbv") == f::labio_velar);
	assert(tts::get_feature_id("uvl") == f::uvular);
	assert(tts::get_feature_id("phr") == f::pharyngeal);
	assert(tts::get_feature_id("glt") == f::glottal);

	assert(tts::get_feature_id("stp") == f::plosive);
	assert(tts::get_feature_id("frc") == f::fricative);
	assert(tts::get_feature_id("nas") == f::nasal);
	assert(tts::get_feature_id("apr") == f::approximant);
	assert(tts::get_feature_id("vwl") == f::vowel);
	assert(tts::get_feature_id("lat") == f::lateral);
	assert(tts::get_feature_id("trl") == f::trill);
	assert(tts::get_feature_id("flp") == f::flap);
	assert(tts::get_feature_id("clk") == f::click);
	assert(tts::get_feature_id("ejc") == f::ejective);
	assert(tts::get_feature_id("imp") == f::implosive);

	assert(tts::get_feature_id("hgh") == f::high);
	assert(tts::get_feature_id("smh") == f::semi_high);
	assert(tts::get_feature_id("umd") == f::upper_mid);
	assert(tts::get_feature_id("mid") == f::mid);
	assert(tts::get_feature_id("lmd") == f::lower_mid);
	assert(tts::get_feature_id("low") == f::low);

	assert(tts::get_feature_id("fnt") == f::front);
	assert(tts::get_feature_id("cnt") == f::center);
	assert(tts::get_feature_id("bck") == f::back);

	assert(tts::get_feature_id("unr") == f::unrounded);
	assert(tts::get_feature_id("rnd") == f::rounded);

	assert(tts::get_feature_id("asp") == f::aspirated);
	assert(tts::get_feature_id("unx") == f::unexploded);
	assert(tts::get_feature_id("syl") == f::syllabic);
	assert(tts::get_feature_id("mrm") == f::murmured);
	assert(tts::get_feature_id("lng") == f::long_);
	assert(tts::get_feature_id("vzd") == f::velarized);
	assert(tts::get_feature_id("lzd") == f::labialized);
	assert(tts::get_feature_id("pzd") == f::palatalized);
	assert(tts::get_feature_id("rzd") == f::rhoticized);
	assert(tts::get_feature_id("nzd") == f::nasalized);
	assert(tts::get_feature_id("fzd") == f::pharyngealized);

	// These are properties of consonant phonemes that are not needed to
	// specify phonemes in the IPA chart. They are only referenced in
	// Appendix A of the ASCII-IPA document.

	assert(tts::get_feature_id("orl") == f::unspecified);
	assert(tts::get_feature_id("ctl") == f::unspecified);
}
