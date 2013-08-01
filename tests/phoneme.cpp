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
	assert(sizeof(tts::phoneme) == 8);
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
	assert(!p.contains(f::rhoticized));
}

TEST_CASE("5 feature phoneme")
{
	tts::phoneme p(f::high, f::front, f::unrounded, f::vowel, f::rhoticized);

	assert(p.contains(f::high));
	assert(p.contains(f::front));
	assert(p.contains(f::unrounded));
	assert(p.contains(f::vowel));
	assert(p.contains(f::rhoticized));

	assert(!p.contains(f::unspecified));
	assert(!p.contains(f::rounded));
	assert(!p.contains(f::voiceless));
	assert(!p.contains(f::long_));
}

TEST_CASE("6 feature phoneme")
{
	tts::phoneme p(f::high, f::front, f::unrounded, f::vowel, f::rhoticized, f::primary_stress);

	assert(p.contains(f::high));
	assert(p.contains(f::front));
	assert(p.contains(f::unrounded));
	assert(p.contains(f::vowel));
	assert(p.contains(f::rhoticized));
	assert(p.contains(f::primary_stress));

	assert(!p.contains(f::unspecified));
	assert(!p.contains(f::rounded));
	assert(!p.contains(f::voiceless));
	assert(!p.contains(f::long_));
	assert(!p.contains(f::secondary_stress));
}

TEST_CASE("7 feature phoneme")
{
	tts::phoneme p(f::high, f::front, f::unrounded, f::vowel, f::long_, f::rhoticized, f::primary_stress);

	assert(p.contains(f::high));
	assert(p.contains(f::front));
	assert(p.contains(f::unrounded));
	assert(p.contains(f::vowel));
	assert(p.contains(f::rhoticized));
	assert(p.contains(f::primary_stress));
	assert(p.contains(f::long_));

	assert(!p.contains(f::unspecified));
	assert(!p.contains(f::rounded));
	assert(!p.contains(f::voiceless));
	assert(!p.contains(f::secondary_stress));
}

TEST_CASE("8 feature phoneme")
{
	tts::phoneme p(f::high, f::front, f::unrounded, f::vowel, f::long_, f::rhoticized, f::unreleased, f::primary_stress);

	assert(p.contains(f::high));
	assert(p.contains(f::front));
	assert(p.contains(f::unrounded));
	assert(p.contains(f::vowel));
	assert(p.contains(f::rhoticized));
	assert(p.contains(f::primary_stress));
	assert(p.contains(f::long_));
	assert(p.contains(f::unreleased));

	assert(!p.contains(f::unspecified));
	assert(!p.contains(f::rounded));
	assert(!p.contains(f::voiceless));
	assert(!p.contains(f::secondary_stress));
}

TEST_CASE("copy construction")
{
	tts::phoneme p(f::high, f::front, f::unrounded, f::vowel, f::long_, f::rhoticized, f::unreleased, f::primary_stress);
	tts::phoneme q(p);

	assert(p.contains(f::high));
	assert(p.contains(f::front));
	assert(p.contains(f::unrounded));
	assert(p.contains(f::vowel));
	assert(p.contains(f::rhoticized));
	assert(p.contains(f::primary_stress));
	assert(p.contains(f::long_));
	assert(p.contains(f::unreleased));

	assert(!p.contains(f::unspecified));
	assert(!p.contains(f::rounded));
	assert(!p.contains(f::voiceless));
	assert(!p.contains(f::secondary_stress));
}

TEST_CASE("copy assignment")
{
	tts::phoneme p(f::voiced, f::dental, f::fricative);
	p = tts::phoneme(f::high, f::front, f::unrounded, f::vowel, f::long_, f::rhoticized, f::unreleased, f::primary_stress);

	assert(p.contains(f::high));
	assert(p.contains(f::front));
	assert(p.contains(f::unrounded));
	assert(p.contains(f::vowel));
	assert(p.contains(f::rhoticized));
	assert(p.contains(f::primary_stress));
	assert(p.contains(f::long_));
	assert(p.contains(f::unreleased));

	assert(!p.contains(f::unspecified));
	assert(!p.contains(f::rounded));
	assert(!p.contains(f::voiceless));
	assert(!p.contains(f::secondary_stress));

	tts::phoneme q(f::voiced, f::dental, f::fricative);
	q = { f::high, f::front, f::unrounded, f::vowel, f::long_, f::rhoticized, f::unreleased, f::primary_stress };

	assert(q.contains(f::high));
	assert(q.contains(f::front));
	assert(q.contains(f::unrounded));
	assert(q.contains(f::vowel));
	assert(q.contains(f::rhoticized));
	assert(q.contains(f::primary_stress));
	assert(q.contains(f::long_));
	assert(q.contains(f::unreleased));

	assert(!q.contains(f::unspecified));
	assert(!q.contains(f::rounded));
	assert(!q.contains(f::voiceless));
	assert(!q.contains(f::secondary_stress));
}

TEST_CASE("phoneme equality")
{
	// same position, same feature ...

	assert(tts::phoneme(f::unspecified, f::unspecified, f::unspecified, f::unspecified, f::unspecified)
	    == tts::phoneme(f::unspecified, f::unspecified, f::unspecified, f::unspecified, f::unspecified));

	assert(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	    == tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized));

	// same position, different feature ...

	assert(!(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	      == tts::phoneme(f::high,      f::back, f::unrounded, f::vowel, f::rhoticized)));

	assert(!(tts::phoneme(f::semi_high, f::back,  f::unrounded, f::vowel, f::rhoticized)
	      == tts::phoneme(f::semi_high, f::front, f::unrounded, f::vowel, f::rhoticized)));

	assert(!(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	      == tts::phoneme(f::semi_high, f::back, f::rounded,   f::vowel, f::rhoticized)));

	assert(!(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel,   f::rhoticized)
	      == tts::phoneme(f::semi_high, f::back, f::unrounded, f::lateral, f::rhoticized)));

	assert(!(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	      == tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::long_)));

	// different number of features ...

	assert(!(tts::phoneme(f::voiceless, f::alveolar, f::fricative, f::unspecified, f::unspecified)
	      == tts::phoneme(f::voiceless, f::alveolar, f::lateral,   f::fricative,   f::unspecified)));

	assert(!(tts::phoneme(f::voiceless, f::alveolar, f::lateral,   f::fricative,   f::unspecified)
	      == tts::phoneme(f::voiceless, f::alveolar, f::fricative, f::unspecified, f::unspecified)));

	assert(!(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	      == tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::unspecified)));

	// all unspecified except one feature, different position ...
	//
	// NOTE: The unspecified feature is a special feature that should be ignored in any
	// comparison, which is what is being tested here.

	for (int i = 0; i < 5; ++i)
	{
		tts::feature a[5] = {
			f::unspecified, f::unspecified, f::unspecified, f::unspecified, f::unspecified
		};
		a[i] = f::glottal;

		for (int j = 0; j < 5; ++j)
		{
			if (i == j) continue;

			tts::feature b[5] = {
				f::unspecified, f::unspecified, f::unspecified, f::unspecified, f::unspecified
			};
			b[j] = f::glottal;

			assert(tts::phoneme(a[0], a[1], a[2], a[3], a[4])
			    == tts::phoneme(b[0], b[1], b[2], b[3], b[4]));
		}
	}

	// different position, same feature ...

	assert(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	    == tts::phoneme(f::rhoticized, f::semi_high, f::back, f::unrounded, f::vowel));

	assert(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	    == tts::phoneme(f::vowel, f::rhoticized, f::semi_high, f::back, f::unrounded));

	assert(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	    == tts::phoneme(f::unrounded, f::vowel, f::rhoticized, f::semi_high, f::back));

	assert(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	    == tts::phoneme(f::back, f::unrounded, f::vowel, f::rhoticized, f::semi_high));
}

TEST_CASE("phoneme inequality")
{
	// same position, same feature ...

	assert(!(tts::phoneme(f::unspecified, f::unspecified, f::unspecified, f::unspecified, f::unspecified)
	      != tts::phoneme(f::unspecified, f::unspecified, f::unspecified, f::unspecified, f::unspecified)));

	assert(!(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	      != tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)));

	// same position, different feature ...

	assert(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	    != tts::phoneme(f::high,      f::back, f::unrounded, f::vowel, f::rhoticized));

	assert(tts::phoneme(f::semi_high, f::back,  f::unrounded, f::vowel, f::rhoticized)
	    != tts::phoneme(f::semi_high, f::front, f::unrounded, f::vowel, f::rhoticized));

	assert(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	    != tts::phoneme(f::semi_high, f::back, f::rounded,   f::vowel, f::rhoticized));

	assert(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel,   f::rhoticized)
	    != tts::phoneme(f::semi_high, f::back, f::unrounded, f::lateral, f::rhoticized));

	assert(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	    != tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::long_));

	// different number of features ...

	assert(tts::phoneme(f::voiceless, f::alveolar, f::fricative, f::unspecified, f::unspecified)
	    != tts::phoneme(f::voiceless, f::alveolar, f::lateral,   f::fricative,   f::unspecified));

	assert(tts::phoneme(f::voiceless, f::alveolar, f::lateral,   f::fricative,   f::unspecified)
	    != tts::phoneme(f::voiceless, f::alveolar, f::fricative, f::unspecified, f::unspecified));

	assert(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	    != tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::unspecified));

	// all unspecified except one feature, different position ...
	//
	// NOTE: The unspecified feature is a special feature that should be ignored in any
	// comparison, which is what is being tested here.

	for (int i = 0; i < 5; ++i)
	{
		tts::feature a[5] = {
			f::unspecified, f::unspecified, f::unspecified, f::unspecified, f::unspecified
		};
		a[i] = f::glottal;

		for (int j = 0; j < 5; ++j)
		{
			if (i == j) continue;

			tts::feature b[5] = {
				f::unspecified, f::unspecified, f::unspecified, f::unspecified, f::unspecified
			};
			b[j] = f::glottal;

			assert(!(tts::phoneme(a[0], a[1], a[2], a[3], a[4])
			      != tts::phoneme(b[0], b[1], b[2], b[3], b[4])));
		}
	}

	// different position, same feature ...

	assert(!(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	      != tts::phoneme(f::rhoticized, f::semi_high, f::back, f::unrounded, f::vowel)));

	assert(!(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	      != tts::phoneme(f::vowel, f::rhoticized, f::semi_high, f::back, f::unrounded)));

	assert(!(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	      != tts::phoneme(f::unrounded, f::vowel, f::rhoticized, f::semi_high, f::back)));

	assert(!(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	      != tts::phoneme(f::back, f::unrounded, f::vowel, f::rhoticized, f::semi_high)));
}

TEST_CASE("remove feature")
{
	tts::phoneme p(f::voiceless, f::bilabial, f::plosive, f::nasalized);

	assert(!p.remove(f::unspecified));

	assert(p.contains(f::voiceless));
	assert(p.contains(f::bilabial));
	assert(p.contains(f::plosive));
	assert(p.contains(f::nasalized));

	assert(!p.remove(f::voiced));

	assert(p.contains(f::voiceless));
	assert(p.contains(f::bilabial));
	assert(p.contains(f::plosive));
	assert(p.contains(f::nasalized));

	assert(p.remove(f::nasalized));

	assert(p.contains(f::voiceless));
	assert(p.contains(f::bilabial));
	assert(p.contains(f::plosive));
	assert(!p.contains(f::nasalized));
}

TEST_CASE("add feature")
{
	tts::phoneme p;
	assert(!p.contains(f::voiceless));
	assert(!p.contains(f::voiced));

	assert(!p.add(f::unspecified));
	assert(!p.contains(f::voiceless));
	assert(!p.contains(f::voiced));

	assert(p.add(f::voiceless));
	assert(p.contains(f::voiceless));
	assert(!p.contains(f::voiced));

	assert(!p.add(f::voiceless));
	assert(p.contains(f::voiceless));
	assert(!p.contains(f::voiced));
}

TEST_CASE("tts::get_feature_id -- invalid abbreviations")
{
	auto _ = [](const tts::feature x) -> std::pair<bool, tts::feature>
	{
		return { false, x };
	};

	assert(tts::get_feature_id(nullptr) == _(f::unspecified));
	assert(tts::get_feature_id("xyz") == _(f::unspecified));
	assert(tts::get_feature_id("aspirated") == _(f::unspecified));
}

TEST_CASE("tts::get_feature_id -- ascii-ipa abbreviations")
{
	auto _ = [](const tts::feature x) -> std::pair<bool, tts::feature>
	{
		return { true, x };
	};

	assert(tts::get_feature_id("vcd") == _(f::voiced));
	assert(tts::get_feature_id("vls") == _(f::voiceless));

	assert(tts::get_feature_id("blb") == _(f::bilabial));
	assert(tts::get_feature_id("lbd") == _(f::labio_dental));
	assert(tts::get_feature_id("dnt") == _(f::dental));
	assert(tts::get_feature_id("alv") == _(f::alveolar));
	assert(tts::get_feature_id("rfx") == _(f::retroflex));
	assert(tts::get_feature_id("pla") == _(f::palato_alveolar));
	assert(tts::get_feature_id("pal") == _(f::palatal));
	assert(tts::get_feature_id("vel") == _(f::velar));
	assert(tts::get_feature_id("uvl") == _(f::uvular));
	assert(tts::get_feature_id("phr") == _(f::pharyngeal));
	assert(tts::get_feature_id("glt") == _(f::glottal));

	assert(tts::get_feature_id("stp") == _(f::plosive));
	assert(tts::get_feature_id("frc") == _(f::fricative));
	assert(tts::get_feature_id("nas") == _(f::nasal));
	assert(tts::get_feature_id("apr") == _(f::approximant));
	assert(tts::get_feature_id("vwl") == _(f::vowel));
	assert(tts::get_feature_id("lat") == _(f::lateral));
	assert(tts::get_feature_id("trl") == _(f::trill));
	assert(tts::get_feature_id("flp") == _(f::flap));
	assert(tts::get_feature_id("clk") == _(f::click));
	assert(tts::get_feature_id("ejc") == _(f::ejective));
	assert(tts::get_feature_id("imp") == _(f::implosive));

	assert(tts::get_feature_id("hgh") == _(f::high));
	assert(tts::get_feature_id("smh") == _(f::semi_high));
	assert(tts::get_feature_id("umd") == _(f::upper_mid));
	assert(tts::get_feature_id("mid") == _(f::mid));
	assert(tts::get_feature_id("lmd") == _(f::lower_mid));
	assert(tts::get_feature_id("low") == _(f::low));

	assert(tts::get_feature_id("fnt") == _(f::front));
	assert(tts::get_feature_id("cnt") == _(f::center));
	assert(tts::get_feature_id("bck") == _(f::back));

	assert(tts::get_feature_id("unr") == _(f::unrounded));
	assert(tts::get_feature_id("rnd") == _(f::rounded));

	assert(tts::get_feature_id("asp") == _(f::aspirated));
	assert(tts::get_feature_id("unx") == _(f::unreleased));
	assert(tts::get_feature_id("syl") == _(f::syllabic));
	assert(tts::get_feature_id("mrm") == _(f::murmured));
	assert(tts::get_feature_id("lng") == _(f::long_));
	assert(tts::get_feature_id("vzd") == _(f::velarized));
	assert(tts::get_feature_id("lzd") == _(f::labialized));
	assert(tts::get_feature_id("pzd") == _(f::palatalized));
	assert(tts::get_feature_id("rzd") == _(f::rhoticized));
	assert(tts::get_feature_id("nzd") == _(f::nasalized));
	assert(tts::get_feature_id("fzd") == _(f::pharyngealized));

	// These are properties of consonant phonemes that are not needed to
	// specify phonemes in the IPA chart. They are only referenced in
	// Appendix A of the ASCII-IPA document.

	assert(tts::get_feature_id("orl") == _(f::unspecified));
	assert(tts::get_feature_id("ctl") == _(f::unspecified));
}

TEST_CASE("tts::get_feature_id -- extended abbreviations")
{
	auto _ = [](const tts::feature x) -> std::pair<bool, tts::feature>
	{
		return { true, x };
	};

	assert(tts::get_feature_id("sml") == _(f::semi_low));
	assert(tts::get_feature_id("sib") == _(f::sibilant));
	assert(tts::get_feature_id("afr") == _(f::affricate));
	assert(tts::get_feature_id("epg") == _(f::epiglottal));

	assert(tts::get_feature_id("dpf") == _(f::falling_diphthong));
	assert(tts::get_feature_id("dpr") == _(f::rising_diphthong));

	assert(tts::get_feature_id("st1") == _(f::primary_stress));
	assert(tts::get_feature_id("st2") == _(f::secondary_stress));

	assert(tts::get_feature_id("sbr") == _(f::syllable_break));
	assert(tts::get_feature_id("pau") == _(f::silent_pause));

	assert(tts::get_feature_id("est") == _(f::extra_short));
	assert(tts::get_feature_id("hlg") == _(f::half_long));
}

TEST_CASE("tts::get_feature_abbreviation")
{
	assert(!tts::get_feature_abbreviation(f::unspecified));

	tts::feature out_of_range = (tts::feature)((int)f::pharyngealized + 1);
	assert(!tts::get_feature_abbreviation(out_of_range));

	assert(tts::get_feature_abbreviation(f::voiced)    == "vcd");
	assert(tts::get_feature_abbreviation(f::voiceless) == "vls");

	assert(tts::get_feature_abbreviation(f::bilabial)        == "blb");
	assert(tts::get_feature_abbreviation(f::labio_dental)    == "lbd");
	assert(tts::get_feature_abbreviation(f::dental)          == "dnt");
	assert(tts::get_feature_abbreviation(f::alveolar)        == "alv");
	assert(tts::get_feature_abbreviation(f::retroflex)       == "rfx");
	assert(tts::get_feature_abbreviation(f::palato_alveolar) == "pla");
	assert(tts::get_feature_abbreviation(f::palatal)         == "pal");
	assert(tts::get_feature_abbreviation(f::velar)           == "vel");
	assert(tts::get_feature_abbreviation(f::uvular)          == "uvl");
	assert(tts::get_feature_abbreviation(f::pharyngeal)      == "phr");
	assert(tts::get_feature_abbreviation(f::epiglottal)      == "epg");
	assert(tts::get_feature_abbreviation(f::glottal)         == "glt");

	assert(tts::get_feature_abbreviation(f::vowel)             == "vwl");
	assert(tts::get_feature_abbreviation(f::falling_diphthong) == "dpf");
	assert(tts::get_feature_abbreviation(f::rising_diphthong)  == "dpr");
	assert(tts::get_feature_abbreviation(f::lateral)           == "lat");

	assert(tts::get_feature_abbreviation(f::plosive)     == "stp");
	assert(tts::get_feature_abbreviation(f::fricative)   == "frc");
	assert(tts::get_feature_abbreviation(f::affricate)   == "afr");
	assert(tts::get_feature_abbreviation(f::nasal)       == "nas");
	assert(tts::get_feature_abbreviation(f::approximant) == "apr");
	assert(tts::get_feature_abbreviation(f::trill)       == "trl");
	assert(tts::get_feature_abbreviation(f::flap)        == "flp");
	assert(tts::get_feature_abbreviation(f::click)       == "clk");
	assert(tts::get_feature_abbreviation(f::ejective)    == "ejc");
	assert(tts::get_feature_abbreviation(f::implosive)   == "imp");

	assert(tts::get_feature_abbreviation(f::high)      == "hgh");
	assert(tts::get_feature_abbreviation(f::semi_high) == "smh");
	assert(tts::get_feature_abbreviation(f::upper_mid) == "umd");
	assert(tts::get_feature_abbreviation(f::mid)       == "mid");
	assert(tts::get_feature_abbreviation(f::lower_mid) == "lmd");
	assert(tts::get_feature_abbreviation(f::semi_low)  == "sml");
	assert(tts::get_feature_abbreviation(f::low)       == "low");

	assert(tts::get_feature_abbreviation(f::front)  == "fnt");
	assert(tts::get_feature_abbreviation(f::center) == "cnt");
	assert(tts::get_feature_abbreviation(f::back)   == "bck");

	assert(tts::get_feature_abbreviation(f::unrounded) == "unr");
	assert(tts::get_feature_abbreviation(f::rounded)   == "rnd");

	assert(tts::get_feature_abbreviation(f::primary_stress)   == "st1");
	assert(tts::get_feature_abbreviation(f::secondary_stress) == "st2");

	assert(tts::get_feature_abbreviation(f::syllable_break) == "sbr");
	assert(tts::get_feature_abbreviation(f::silent_pause)   == "pau");

	assert(tts::get_feature_abbreviation(f::extra_short) == "est");
	assert(tts::get_feature_abbreviation(f::half_long)   == "hlg");
	assert(tts::get_feature_abbreviation(f::long_)       == "lng");

	assert(tts::get_feature_abbreviation(f::sibilant)       == "sib");
	assert(tts::get_feature_abbreviation(f::aspirated)      == "asp");
	assert(tts::get_feature_abbreviation(f::unreleased)     == "unx");
	assert(tts::get_feature_abbreviation(f::syllabic)       == "syl");
	assert(tts::get_feature_abbreviation(f::murmured)       == "mrm");
	assert(tts::get_feature_abbreviation(f::velarized)      == "vzd");
	assert(tts::get_feature_abbreviation(f::labialized)     == "lzd");
	assert(tts::get_feature_abbreviation(f::palatalized)    == "pzd");
	assert(tts::get_feature_abbreviation(f::rhoticized)     == "rzd");
	assert(tts::get_feature_abbreviation(f::nasalized)      == "nzd");
	assert(tts::get_feature_abbreviation(f::pharyngealized) == "fzd");
}

TEST_CASE("tts::get_feature_name")
{
	assert(!tts::get_feature_name(f::unspecified));

	tts::feature out_of_range = (tts::feature)((int)f::pharyngealized + 1);
	assert(!tts::get_feature_name(out_of_range));

	const char *locale = "en_GB.utf8";
	if (!setlocale(LC_MESSAGES, locale) && !setenv("LANGUAGE", locale, 1) && !setenv("LANG", locale, 1))
	{
		printf("... ... unable to set locale to '%s'\n", locale);
	}

	assert(tts::get_feature_name(f::voiced)    == "voiced");
	assert(tts::get_feature_name(f::voiceless) == "voiceless");

	assert(tts::get_feature_name(f::bilabial)        == "bilabial");
	assert(tts::get_feature_name(f::labio_dental)    == "labio-dental");
	assert(tts::get_feature_name(f::dental)          == "dental");
	assert(tts::get_feature_name(f::alveolar)        == "alveolar");
	assert(tts::get_feature_name(f::retroflex)       == "retroflex");
	assert(tts::get_feature_name(f::palato_alveolar) == "palato-alveolar");
	assert(tts::get_feature_name(f::palatal)         == "palatal");
	assert(tts::get_feature_name(f::velar)           == "velar");
	assert(tts::get_feature_name(f::uvular)          == "uvular");
	assert(tts::get_feature_name(f::pharyngeal)      == "pharyngeal");
	assert(tts::get_feature_name(f::epiglottal)      == "epiglottal");
	assert(tts::get_feature_name(f::glottal)         == "glottal");

	assert(tts::get_feature_name(f::vowel)             == "vowel");
	assert(tts::get_feature_name(f::falling_diphthong) == "falling diphthong");
	assert(tts::get_feature_name(f::rising_diphthong)  == "rising diphthong");
	assert(tts::get_feature_name(f::lateral)           == "lateral");

	assert(tts::get_feature_name(f::plosive)     == "plosive");
	assert(tts::get_feature_name(f::fricative)   == "fricative");
	assert(tts::get_feature_name(f::affricate)   == "affricate");
	assert(tts::get_feature_name(f::nasal)       == "nasal");
	assert(tts::get_feature_name(f::approximant) == "approximant");
	assert(tts::get_feature_name(f::trill)       == "trill");
	assert(tts::get_feature_name(f::flap)        == "flap");
	assert(tts::get_feature_name(f::click)       == "click");
	assert(tts::get_feature_name(f::ejective)    == "ejective");
	assert(tts::get_feature_name(f::implosive)   == "implosive");

	assert(tts::get_feature_name(f::high)      == "high");
	assert(tts::get_feature_name(f::semi_high) == "semi-high");
	assert(tts::get_feature_name(f::upper_mid) == "upper-mid");
	assert(tts::get_feature_name(f::mid)       == "mid");
	assert(tts::get_feature_name(f::lower_mid) == "lower-mid");
	assert(tts::get_feature_name(f::semi_low)  == "semi-low");
	assert(tts::get_feature_name(f::low)       == "low");

	assert(tts::get_feature_name(f::front)  == "front");
	assert(tts::get_feature_name(f::center) == "center");
	assert(tts::get_feature_name(f::back)   == "back");

	assert(tts::get_feature_name(f::unrounded) == "unrounded");
	assert(tts::get_feature_name(f::rounded)   == "rounded");

	assert(tts::get_feature_name(f::primary_stress)   == "primary stress");
	assert(tts::get_feature_name(f::secondary_stress) == "secondary stress");

	assert(tts::get_feature_name(f::syllable_break) == "syllable break");
	assert(tts::get_feature_name(f::silent_pause)   == "pause");

	assert(tts::get_feature_name(f::extra_short) == "extra short");
	assert(tts::get_feature_name(f::half_long)   == "half long");
	assert(tts::get_feature_name(f::long_)       == "long");

	assert(tts::get_feature_name(f::sibilant)       == "sibilant");
	assert(tts::get_feature_name(f::aspirated)      == "aspirated");
	assert(tts::get_feature_name(f::unreleased)     == "unreleased");
	assert(tts::get_feature_name(f::syllabic)       == "syllabic");
	assert(tts::get_feature_name(f::murmured)       == "murmured");
	assert(tts::get_feature_name(f::velarized)      == "velarized");
	assert(tts::get_feature_name(f::labialized)     == "labialized");
	assert(tts::get_feature_name(f::palatalized)    == "palatalized");
	assert(tts::get_feature_name(f::rhoticized)     == "rhoticized");
	assert(tts::get_feature_name(f::nasalized)      == "nasalized");
	assert(tts::get_feature_name(f::pharyngealized) == "pharyngealized");
}

TEST_CASE("explicit feature reader -- no input")
{
	std::shared_ptr<tts::phoneme_reader> reader = tts::createExplicitFeaturePhonemeReader();
	assert(reader.get());
	assert(*reader == tts::phoneme(f::unspecified, f::unspecified, f::unspecified));

	assert(!reader->read());
	assert(*reader == tts::phoneme(f::unspecified, f::unspecified, f::unspecified));

	reader->reset(std::shared_ptr<cainteoir::buffer>());

	assert(!reader->read());
	assert(*reader == tts::phoneme(f::unspecified, f::unspecified, f::unspecified));

	auto test = std::make_shared<cainteoir::buffer>(nullptr, nullptr);
	reader->reset(test);

	assert(!reader->read());
	assert(*reader == tts::phoneme(f::unspecified, f::unspecified, f::unspecified));
}

TEST_CASE("explicit feature reader -- single phoneme")
{
	// Test data to cover all feature abbreviations:
	static const std::initializer_list<std::pair<
		const char *,
		const tts::phoneme
	>> phonemes = {
		// prosody ...
		{ "{pau}",                 { f::silent_pause, f::unspecified, f::unspecified } },
		{ "{pau,est}",             { f::silent_pause, f::extra_short, f::unspecified } },
		// consonants ...
		{ "{vcd,blb,stp,asp}",     { f::voiced, f::bilabial, f::plosive, f::aspirated } },
		{ "{vls,lbd,frc,unx}",     { f::voiceless, f::labio_dental, f::fricative, f::unreleased } },
		{ "{vls,lbd,afr,unx}",     { f::voiceless, f::labio_dental, f::affricate, f::unreleased } },
		{ "{vcd,dnt,nas,syl}",     { f::voiced, f::dental, f::nasal, f::syllabic } },
		{ "{vls,alv,apr,mrm}",     { f::voiceless, f::alveolar, f::approximant, f::murmured } },
		{ "{vcd,lbd,lat,frc}",     { f::voiced, f::labio_dental, f::lateral, f::fricative } },
		{ "{vls,rfx,trl,vzd}",     { f::voiceless, f::retroflex, f::trill, f::velarized } },
		{ "{vcd,pla,flp,lzd}",     { f::voiced, f::palato_alveolar, f::flap, f::labialized } },
		{ "{vls,pal,clk}",         { f::voiceless, f::palatal, f::click } },
		{ "{vcd,vel,ejc}",         { f::voiced, f::velar, f::ejective } },
		{ "{vls,lbv,imp}",         { f::voiceless, f::velar, f::labialized, f::implosive } },
		{ "{vcd,uvl,stp,pzd}",     { f::voiced, f::uvular, f::plosive, f::palatalized } },
		{ "{vls,phr,frc,nzd}",     { f::voiceless, f::pharyngeal, f::fricative, f::nasalized } },
		{ "{vcd,epg,frc}",         { f::voiced, f::epiglottal, f::fricative } },
		{ "{vcd,glt,nas,fzd}",     { f::voiced, f::glottal, f::nasal, f::pharyngealized } },
		// ignored phoneme features ...
		{ "{vcd,blb,orl,stp}",     { f::voiced, f::bilabial, f::plosive } },
		{ "{vcd,blb,ctl,stp}",     { f::voiced, f::bilabial, f::plosive } },
		// vowels ...
		{ "{hgh,fnt,unr,vwl,lng}", { f::high, f::front, f::unrounded, f::vowel, f::long_ } },
		{ "{smh,fnt,rnd,vwl,st1}", { f::semi_high, f::front, f::rounded, f::vowel, f::primary_stress } },
		{ "{umd,cnt,unr,vwl,st2}", { f::upper_mid, f::center, f::unrounded, f::vowel, f::secondary_stress } },
		{ "{mid,cnt,rnd,vwl,rzd}", { f::mid, f::center, f::rounded, f::vowel, f::rhoticized } },
		{ "{lmd,bck,unr,vwl}",     { f::lower_mid, f::back, f::unrounded, f::vowel } },
		{ "{sml,fnt,unr,vwl}",     { f::semi_low, f::front, f::unrounded, f::vowel } },
		{ "{low,bck,rnd,vwl}",     { f::low, f::back, f::rounded, f::vowel } },
	};

	std::shared_ptr<tts::phoneme_reader> reader = tts::createExplicitFeaturePhonemeReader();

	for (const auto &test : phonemes)
	{
		fprintf(stdout, "... ... testing phoneme %s\n", test.first);
		reader->reset(std::make_shared<cainteoir::buffer>(test.first));

		assert(reader->read());
		assert(*reader == test.second);

		assert(!reader->read());
		assert(*reader == tts::phoneme(f::unspecified, f::unspecified, f::unspecified));
	}
}

TEST_CASE("explicit feature reader -- multiple phonemes")
{
	std::shared_ptr<tts::phoneme_reader> reader = tts::createExplicitFeaturePhonemeReader();

	const char *test = "{vls,alv,stp}{low,fnt,unr,vwl}{vcd,vel,stp}"; // = /t&g/
	reader->reset(std::make_shared<cainteoir::buffer>(test));

	assert(reader->read());
	assert(*reader == tts::phoneme(f::voiceless, f::alveolar, f::plosive));

	assert(reader->read());
	assert(*reader == tts::phoneme(f::low, f::front, f::unrounded, f::vowel));

	assert(reader->read());
	assert(*reader == tts::phoneme(f::voiced, f::velar, f::plosive));

	assert(!reader->read());
	assert(*reader == tts::phoneme(f::unspecified, f::unspecified, f::unspecified));
}

TEST_CASE("explicit feature reader -- multiple phonemes with whitespace")
{
	std::shared_ptr<tts::phoneme_reader> reader = tts::createExplicitFeaturePhonemeReader();

	const char *test = "\r\t{vls,alv,stp}\n {low,fnt,unr,vwl}\r\n{vcd,vel,stp}"; // = /t&g/
	reader->reset(std::make_shared<cainteoir::buffer>(test));

	assert(reader->read());
	assert(*reader == tts::phoneme(f::voiceless, f::alveolar, f::plosive));

	assert(reader->read());
	assert(*reader == tts::phoneme(f::low, f::front, f::unrounded, f::vowel));

	assert(reader->read());
	assert(*reader == tts::phoneme(f::voiced, f::velar, f::plosive));

	assert(!reader->read());
	assert(*reader == tts::phoneme(f::unspecified, f::unspecified, f::unspecified));
}

TEST_CASE("explicit feature reader -- phoneme errors")
{
	static const std::initializer_list<std::pair<
		const char *,
		const std::string
	>> phonemes = {
		// missing '}' at the end of the phoneme ...
		{ "{vcd,alv,frc",              "unexpected end of phoneme (expecting '}')" },
		{ "{vcd,alv,",                 "unexpected end of phoneme (expecting '}')" },
		{ "{",                         "unexpected end of phoneme (expecting '}')" },
		// no '{' to start the phoneme ...
		{ "}",                         "unexpected start of phoneme (expecting '{')" },
		{ "vcd,alv,frc",               "unexpected start of phoneme (expecting '{')" },
		{ ",alv,frc",                  "unexpected start of phoneme (expecting '{')" },
		{ "@",                         "unexpected start of phoneme (expecting '{')" },
		{ "5",                         "unexpected start of phoneme (expecting '{')" },
		// no phoneme feature specified ...
		{ "{}",                        "missing phoneme feature" },
		{ "{vcd,}",                    "missing phoneme feature" },
		{ "{,lbd}",                    "missing phoneme feature" },
		// more than 8 features ...
		{ "{vcd,dnt,lat,frc,mrm,asp,lzd,pzd,nzd}", "a phoneme must specify no more than 8 features" },
		// features shorter/longer than 3 characters ...
		{ "{s}",                       "a phoneme feature must be 3 characters long" },
		{ "{5}",                       "a phoneme feature must be 3 characters long" },
		{ "{%}",                       "a phoneme feature must be 3 characters long" },
		{ "{st}",                      "a phoneme feature must be 3 characters long" },
		{ "{stop}",                    "a phoneme feature must be 3 characters long" },
		// unknown feature ...
		{ "{aaa}",                     "unknown phoneme feature 'aaa'" },
		{ "{xyz}",                     "unknown phoneme feature 'xyz'" },
		{ "{STP}",                     "unknown phoneme feature 'STP'" },
		{ "{stP}",                     "unknown phoneme feature 'stP'" },
		{ "{st5}",                     "unknown phoneme feature 'st5'" },
		{ "{st%}",                     "unknown phoneme feature 'st%'" },
	};

	std::shared_ptr<tts::phoneme_reader> reader = tts::createExplicitFeaturePhonemeReader();

	for (const auto &test : phonemes)
	{
		fprintf(stdout, "... ... testing phoneme %s\n", test.first);
		reader->reset(std::make_shared<cainteoir::buffer>(test.first));

		try
		{
			reader->read();
			assert(false); // reading the phoneme should have thrown an exception
		}
		catch (const tts::phoneme_error &e)
		{
			assert(e.what() == test.second);
		}
		catch (...)
		{
			assert(false); // should be a tts::phoneme_error exception
		}
		assert(*reader == tts::phoneme(f::unspecified, f::unspecified, f::unspecified));
	}
}

TEST_CASE("explicit feature writer -- no input")
{
	std::shared_ptr<tts::phoneme_writer> writer = tts::createExplicitFeaturePhonemeWriter();
	assert(writer.get());

	assert(!writer->write({ f::voiceless, f::glottal, f::plosive }));

	cainteoir::memory_file output;
	writer->reset(output);

	auto data = output.buffer();
	assert(data->empty());

	writer->reset(nullptr);
	assert(!writer->write({ f::voiceless, f::glottal, f::plosive }));
}

TEST_CASE("explicit feature writer -- single phoneme")
{
	// Test data to cover all feature abbreviations:
	static const std::initializer_list<std::pair<
		const std::string,
		const tts::phoneme
	>> phonemes = {
		// prosody ...
		{ "{pau}",                 { f::silent_pause, f::unspecified, f::unspecified } },
		{ "{pau,est}",             { f::silent_pause, f::extra_short, f::unspecified } },
		// consonants ...
		{ "{vcd,blb,stp,asp}",     { f::voiced, f::bilabial, f::plosive, f::aspirated } },
		{ "{vls,lbd,frc,unx}",     { f::voiceless, f::labio_dental, f::fricative, f::unreleased } },
		{ "{vcd,dnt,nas,syl}",     { f::voiced, f::dental, f::nasal, f::syllabic } },
		{ "{vls,alv,apr,mrm}",     { f::voiceless, f::alveolar, f::approximant, f::murmured } },
		{ "{vcd,lbd,lat,frc}",     { f::voiced, f::labio_dental, f::lateral, f::fricative } },
		{ "{vcd,lbd,lat,afr}",     { f::voiced, f::labio_dental, f::lateral, f::affricate } },
		{ "{vls,rfx,trl,vzd}",     { f::voiceless, f::retroflex, f::trill, f::velarized } },
		{ "{vcd,pla,flp,lzd}",     { f::voiced, f::palato_alveolar, f::flap, f::labialized } },
		{ "{vls,pal,clk}",         { f::voiceless, f::palatal, f::click } },
		{ "{vcd,vel,ejc}",         { f::voiced, f::velar, f::ejective } },
		{ "{vls,vel,lzd,imp}",     { f::voiceless, f::velar, f::labialized, f::implosive } },
		{ "{vcd,uvl,stp,pzd}",     { f::voiced, f::uvular, f::plosive, f::palatalized } },
		{ "{vls,phr,frc,nzd}",     { f::voiceless, f::pharyngeal, f::fricative, f::nasalized } },
		{ "{vcd,epg,frc}",         { f::voiced, f::epiglottal, f::fricative } },
		{ "{vcd,glt,nas,fzd}",     { f::voiced, f::glottal, f::nasal, f::pharyngealized } },
		// vowels ...
		{ "{hgh,fnt,unr,vwl,lng}", { f::high, f::front, f::unrounded, f::vowel, f::long_ } },
		{ "{smh,fnt,rnd,vwl,st1}", { f::semi_high, f::front, f::rounded, f::vowel, f::primary_stress } },
		{ "{umd,cnt,unr,vwl,st2}", { f::upper_mid, f::center, f::unrounded, f::vowel, f::secondary_stress } },
		{ "{mid,cnt,rnd,vwl,rzd}", { f::mid, f::center, f::rounded, f::vowel, f::rhoticized } },
		{ "{lmd,bck,unr,vwl}",     { f::lower_mid, f::back, f::unrounded, f::vowel } },
		{ "{sml,fnt,unr,vwl}",     { f::semi_low, f::front, f::unrounded, f::vowel } },
		{ "{low,bck,rnd,vwl}",     { f::low, f::back, f::rounded, f::vowel } },
	};

	std::shared_ptr<tts::phoneme_writer> writer = tts::createExplicitFeaturePhonemeWriter();

	for (const auto &test : phonemes)
	{
		fprintf(stdout, "... ... testing phoneme %s\n", test.first.c_str());

		cainteoir::memory_file output;
		writer->reset(output);

		assert(writer->write(test.second));
		assert(output.buffer()->str() == test.first);
	}
}

TEST_CASE("explicit feature writer -- multiple phonemes")
{
	std::shared_ptr<tts::phoneme_writer> writer = tts::createExplicitFeaturePhonemeWriter();

	cainteoir::memory_file output;
	writer->reset(output);

	assert(writer->write(tts::phoneme(f::voiceless, f::alveolar, f::plosive)));
	assert(writer->write(tts::phoneme(f::low, f::front, f::unrounded, f::vowel)));
	assert(writer->write(tts::phoneme(f::voiced, f::velar, f::plosive)));

	assert(output.buffer()->str() == "{vls,alv,stp}{low,fnt,unr,vwl}{vcd,vel,stp}"); // output == /t&g/
}
