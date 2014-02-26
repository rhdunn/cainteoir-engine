/* Test for the phoneme model.
 *
 * Copyright (C) 2013-2014 Reece H. Dunn
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
#include <stdexcept>
#include <iomanip>
#include <cstdio>

#include "tester.hpp"

REGISTER_TESTSUITE("phoneme");

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;

typedef tts::feature f;

namespace cainteoir { namespace ipa
{
	std::ostream &operator<<(std::ostream &os, const phoneme &value)
	{
		auto fill = os.fill();
		return os << "0x"
		          << std::hex << std::setw(16) << std::setfill('0') << std::uppercase
		          << value.get(UINT16_C(0xFFFFFFFFFFFFFFFF))
		          << std::dec << std::setw(0) << std::setfill(fill) << std::nouppercase;
	}
}}

PRINTABLE(ipa::phoneme);

TEST_CASE("ipa::phoneme -- object size")
{
	assert(sizeof(ipa::phoneme) == 8);
}

TEST_CASE("ipa::phoneme -- construction")
{
	const ipa::phoneme::value_type zero = UINT16_C(0x0000000000000000);
	const ipa::phoneme::value_type val1 = UINT16_C(0x1234567812345678);
	const ipa::phoneme::value_type val2 = UINT16_C(0x8765432187654321);

	assert(ipa::phoneme() == ipa::phoneme(zero));
	assert(ipa::phoneme(val1) == ipa::phoneme(val1));
	assert(ipa::phoneme(val1) != ipa::phoneme(val2));

	ipa::phoneme a(val1);
	ipa::phoneme b(val2);

	assert(ipa::phoneme(a) == ipa::phoneme(val1));
	assert(ipa::phoneme(b) != ipa::phoneme(val1));
}

TEST_CASE("ipa::phoneme -- assignment")
{
	const ipa::phoneme::value_type zero = UINT16_C(0x0000000000000000);
	const ipa::phoneme::value_type fill = UINT16_C(0xFFFFFFFFFFFFFFFF);
	const ipa::phoneme::value_type val1 = UINT16_C(0x1234567812345678);

	ipa::phoneme p;

	p = ipa::phoneme(val1);
	assert(p == ipa::phoneme(val1));

	p = ipa::phoneme(fill);
	assert(p == ipa::phoneme(fill));

	p = ipa::phoneme(zero);
	assert(p == ipa::phoneme(zero));
}

TEST_CASE("ipa::phoneme -- get")
{
	const ipa::phoneme::value_type zero = UINT16_C(0x0000000000000000);
	const ipa::phoneme::value_type fill = UINT16_C(0xFFFFFFFFFFFFFFFF);
	const ipa::phoneme::value_type val1 = UINT16_C(0x1234567812345678);
	const ipa::phoneme::value_type mask = UINT16_C(0x1200000000000000);

	assert(ipa::phoneme(fill).get(fill) == fill);
	assert(ipa::phoneme(fill).get(val1) == val1);
	assert(ipa::phoneme(fill).get(zero) == zero);

	assert(ipa::phoneme(val1).get(fill) == val1);
	assert(ipa::phoneme(val1).get(mask) == mask);
	assert(ipa::phoneme(val1).get(zero) == zero);

	// calling get does not change the phoneme ...

	ipa::phoneme a(fill);
	a.get(zero);
	assert(a == ipa::phoneme(fill));
}

TEST_CASE("ipa::phoneme -- set")
{
	const ipa::phoneme::value_type zero = UINT16_C(0x0000000000000000);
	const ipa::phoneme::value_type fill = UINT16_C(0xFFFFFFFFFFFFFFFF);
	const ipa::phoneme::value_type mask = UINT16_C(0xFF00000000000000);

	assert(ipa::phoneme(zero).set(fill) == ipa::phoneme(fill));
	assert(ipa::phoneme(zero).set(mask) == ipa::phoneme(mask));
	assert(ipa::phoneme(zero).set(zero) == ipa::phoneme(zero));

	assert(ipa::phoneme(fill).set(fill) == ipa::phoneme(fill));
	assert(ipa::phoneme(fill).set(mask) == ipa::phoneme(fill));
	assert(ipa::phoneme(fill).set(zero) == ipa::phoneme(fill));
}

TEST_CASE("ipa::phoneme -- set with bit mask")
{
	const ipa::phoneme::value_type zero = UINT16_C(0x0000000000000000);
	const ipa::phoneme::value_type fill = UINT16_C(0xFFFFFFFFFFFFFFFF);
	const ipa::phoneme::value_type mask = UINT16_C(0xFF00000000000000);
	const ipa::phoneme::value_type invm = UINT16_C(0x00FFFFFFFFFFFFFF);

	assert(ipa::phoneme(zero).set(fill, fill) == ipa::phoneme(fill));
	assert(ipa::phoneme(zero).set(fill, mask) == ipa::phoneme(mask));
	assert(ipa::phoneme(zero).set(fill, zero) == ipa::phoneme(zero));

	assert(ipa::phoneme(fill).set(zero, fill) == ipa::phoneme(zero));
	assert(ipa::phoneme(fill).set(zero, mask) == ipa::phoneme(invm));
	assert(ipa::phoneme(fill).set(zero, zero) == ipa::phoneme(fill));
}

TEST_CASE("ipa::phoneme -- clear")
{
	const ipa::phoneme::value_type zero = UINT16_C(0x0000000000000000);
	const ipa::phoneme::value_type fill = UINT16_C(0xFFFFFFFFFFFFFFFF);
	const ipa::phoneme::value_type mask = UINT16_C(0xFF00000000000000);
	const ipa::phoneme::value_type invm = UINT16_C(0x00FFFFFFFFFFFFFF);

	assert(ipa::phoneme(zero).clear(fill) == ipa::phoneme(zero));
	assert(ipa::phoneme(zero).clear(mask) == ipa::phoneme(zero));
	assert(ipa::phoneme(zero).clear(zero) == ipa::phoneme(zero));

	assert(ipa::phoneme(fill).clear(fill) == ipa::phoneme(zero));
	assert(ipa::phoneme(fill).clear(mask) == ipa::phoneme(invm));
	assert(ipa::phoneme(fill).clear(zero) == ipa::phoneme(fill));
}

TEST_CASE("kirshenbaum -- invalid")
{
	using tts::phoneme_error;
	assert_throws(ipa::phoneme().set(nullptr), phoneme_error, "unknown phoneme feature '(null)'");
	assert_throws(ipa::phoneme().set("aaa"), phoneme_error, "unknown phoneme feature 'aaa'");
	assert_throws(ipa::phoneme().set("xyz"), phoneme_error, "unknown phoneme feature 'xyz'");
	assert_throws(ipa::phoneme().set("STP"), phoneme_error, "unknown phoneme feature 'STP'");
	assert_throws(ipa::phoneme().set("stP"), phoneme_error, "unknown phoneme feature 'stP'");
	assert_throws(ipa::phoneme().set("st5"), phoneme_error, "unknown phoneme feature 'st5'");
	assert_throws(ipa::phoneme().set("st%"), phoneme_error, "unknown phoneme feature 'st%'");
	assert_throws(ipa::phoneme().set("aspirated"), phoneme_error, "unknown phoneme feature 'aspirated'");
}


#define KIRSHENBAUM_BIN1(abbr, label, value) /* Enable Binary Feature */ \
	TEST_CASE("kirshenbaum -- " abbr " = " label) \
	{ \
		const ipa::phoneme::value_type zero = UINT16_C(0x0000000000000000); \
		const ipa::phoneme::value_type fill = UINT16_C(0xFFFFFFFFFFFFFFFF); \
		assert(ipa::phoneme(zero).set(abbr) == ipa::phoneme(value)); \
		assert(ipa::phoneme(fill).set(abbr) == ipa::phoneme(fill)); \
	}

#define KIRSHENBAUM_BIN0(abbr, label, value) /* Disable Binary Feature */ \
	TEST_CASE("kirshenbaum -- " abbr " = " label) \
	{ \
		const ipa::phoneme::value_type zero = UINT16_C(0x0000000000000000); \
		const ipa::phoneme::value_type fill = UINT16_C(0xFFFFFFFFFFFFFFFF); \
		assert(ipa::phoneme(zero).set(abbr) == ipa::phoneme(zero)); \
		assert(ipa::phoneme(fill).set(abbr) == ipa::phoneme(fill & ~value)); \
	} \

#define KIRSHENBAUM_RNGD(abbr, label, value, mask) /* Ranged (Value + Mask) Feature */ \
	TEST_CASE("kirshenbaum -- " abbr " = " label) \
	{ \
		const ipa::phoneme::value_type zero = UINT16_C(0x0000000000000000); \
		const ipa::phoneme::value_type fill = UINT16_C(0xFFFFFFFFFFFFFFFF); \
		assert(ipa::phoneme(zero).set(abbr) == ipa::phoneme(value)); \
		assert(ipa::phoneme(fill).set(abbr) == ipa::phoneme((fill & ~mask) | value)); \
	} \

KIRSHENBAUM_BIN1("vcd", "voiced", ipa::voiced)
KIRSHENBAUM_BIN0("vls", "voiceless", ipa::voiced)
KIRSHENBAUM_RNGD("blb", "bilabial", ipa::bilabial, ipa::place_of_articulation)
KIRSHENBAUM_RNGD("lbd", "labio-dental", ipa::labio_dental, ipa::place_of_articulation)
KIRSHENBAUM_RNGD("dnt", "dental", ipa::dental, ipa::place_of_articulation)
KIRSHENBAUM_RNGD("alv", "alveolar", ipa::alveolar, ipa::place_of_articulation)
KIRSHENBAUM_RNGD("pla", "palato-alveolar", ipa::palato_alveolar, ipa::place_of_articulation)
KIRSHENBAUM_RNGD("rfx", "retroflex", ipa::retroflex, ipa::place_of_articulation)
KIRSHENBAUM_RNGD("alp", "alveolo-palatal [extension]", ipa::alveolo_palatal, ipa::place_of_articulation)
KIRSHENBAUM_RNGD("pal", "palatal", ipa::palatal, ipa::place_of_articulation)
KIRSHENBAUM_RNGD("lbp", "labio-palatal [extension]", ipa::labio_palatal, ipa::place_of_articulation)
KIRSHENBAUM_RNGD("vel", "velar", ipa::velar, ipa::place_of_articulation)
KIRSHENBAUM_RNGD("lbv", "labio-velar", ipa::labio_velar, ipa::place_of_articulation)
KIRSHENBAUM_RNGD("uvl", "uvular", ipa::uvular, ipa::place_of_articulation)
KIRSHENBAUM_RNGD("phr", "pharyngeal", ipa::pharyngeal, ipa::place_of_articulation)
KIRSHENBAUM_RNGD("epg", "epiglottal [extension]", ipa::epiglottal, ipa::place_of_articulation)
KIRSHENBAUM_RNGD("glt", "glottal", ipa::glottal, ipa::place_of_articulation)
KIRSHENBAUM_RNGD("stp", "stop", ipa::plosive, ipa::manner_of_articulation)
KIRSHENBAUM_RNGD("frc", "fricative", ipa::fricative, ipa::manner_of_articulation)
KIRSHENBAUM_RNGD("nas", "nasal", ipa::nasal, ipa::manner_of_articulation)
KIRSHENBAUM_BIN1("orl", "oral", 0) // Not Supported -- Kirshenbaum does not use {orl,stp}/{nas,stp}
KIRSHENBAUM_RNGD("apr", "approximant", ipa::approximant, ipa::manner_of_articulation)
KIRSHENBAUM_RNGD("vwl", "vowel", ipa::vowel, ipa::phoneme_type)
KIRSHENBAUM_BIN1("lat", "lateral", ipa::lateral)
KIRSHENBAUM_BIN0("ctl", "central", ipa::lateral)
KIRSHENBAUM_RNGD("trl", "trill", ipa::trill, ipa::manner_of_articulation)
KIRSHENBAUM_RNGD("flp", "flap", ipa::flap, ipa::manner_of_articulation)
KIRSHENBAUM_RNGD("clk", "click", ipa::click, ipa::manner_of_articulation)
KIRSHENBAUM_RNGD("ejc", "ejective", ipa::ejective, ipa::manner_of_articulation)
KIRSHENBAUM_RNGD("imp", "implosive", ipa::implosive, ipa::manner_of_articulation)
KIRSHENBAUM_RNGD("hgh", "high", ipa::high, ipa::vowel_height)
KIRSHENBAUM_RNGD("smh", "semi-high", ipa::semi_high, ipa::vowel_height)
KIRSHENBAUM_RNGD("sml", "semi-low [extension]", ipa::semi_low, ipa::vowel_height)
KIRSHENBAUM_RNGD("umd", "upper-mid", ipa::upper_mid, ipa::vowel_height)
KIRSHENBAUM_RNGD("mid", "mid", ipa::mid, ipa::vowel_height)
KIRSHENBAUM_RNGD("lmd", "lower-mid", ipa::lower_mid, ipa::vowel_height)
KIRSHENBAUM_RNGD("low", "low", ipa::low, ipa::vowel_height)
KIRSHENBAUM_RNGD("fnt", "front", ipa::front, ipa::vowel_backness)
KIRSHENBAUM_RNGD("cnt", "center", ipa::center, ipa::vowel_backness)
KIRSHENBAUM_RNGD("bck", "back", ipa::back, ipa::vowel_backness)
KIRSHENBAUM_BIN1("rnd", "rounded", ipa::rounded)
KIRSHENBAUM_BIN0("unr", "unrounded", ipa::rounded)
KIRSHENBAUM_RNGD("st1", "primary stress [extension]", ipa::primary_stress, ipa::stress)
KIRSHENBAUM_RNGD("st2", "secondary stress [extension]", ipa::secondary_stress, ipa::stress)
KIRSHENBAUM_RNGD("st3", "extra stress [extension]", ipa::extra_stress, ipa::stress)
KIRSHENBAUM_RNGD("est", "extra-short [extension]", ipa::extra_short, ipa::length)
KIRSHENBAUM_RNGD("hlg", "half-long [extension]", ipa::half_long, ipa::length)
KIRSHENBAUM_RNGD("lng", "long", ipa::long_, ipa::length)
KIRSHENBAUM_RNGD("pau", "pause (silence) [extension]", ipa::pause, ipa::phoneme_type)
KIRSHENBAUM_RNGD("sbr", "syllable break [extension]", ipa::syllable_break, ipa::phoneme_type)
KIRSHENBAUM_RNGD("lnk", "linking (no break) [extension]", ipa::linking, ipa::phoneme_type)
KIRSHENBAUM_RNGD("fbr", "foot (minor) break [extension]", ipa::foot_break, ipa::phoneme_type)
KIRSHENBAUM_RNGD("ibr", "intonation (major) break [extension]", ipa::intonation_break, ipa::phoneme_type)
KIRSHENBAUM_BIN1("syl", "syllabic", ipa::syllabic)
KIRSHENBAUM_BIN0("nsy", "non-syllabic [extension]", ipa::syllabic)
KIRSHENBAUM_RNGD("asp", "aspirated", ipa::aspirated, ipa::release)
KIRSHENBAUM_RNGD("unx", "unexploded", ipa::unexploded, ipa::release)
KIRSHENBAUM_RNGD("nzr", "nasal release [extension]", ipa::nasal_release, ipa::release)
KIRSHENBAUM_RNGD("ltr", "lateral release [extension]", ipa::lateral_release, ipa::release)
KIRSHENBAUM_BIN1("mrm", "murmured", ipa::murmured)
KIRSHENBAUM_BIN1("vzd", "velarized", ipa::velarized)
KIRSHENBAUM_BIN1("lzd", "labialized", ipa::labialized)
KIRSHENBAUM_BIN1("pzd", "palatalized", ipa::palatalized)
KIRSHENBAUM_BIN1("rzd", "rhoticized", ipa::rhoticized)
KIRSHENBAUM_BIN1("nzd", "nasalized", ipa::nasalized)
KIRSHENBAUM_BIN1("fzd", "pharyngealized", ipa::pharyngealized)

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
	assert(tts::get_feature_id("epg") == _(f::epiglottal));

	assert(tts::get_feature_id("nsy") == _(f::non_syllabic));

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
	assert(tts::get_feature_abbreviation(f::lateral)           == "lat");

	assert(tts::get_feature_abbreviation(f::plosive)     == "stp");
	assert(tts::get_feature_abbreviation(f::fricative)   == "frc");
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
	assert(tts::get_feature_abbreviation(f::non_syllabic)   == "nsy");
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

	assert(tts::get_feature_name(f::vowel)   == "vowel");
	assert(tts::get_feature_name(f::lateral) == "lateral");

	assert(tts::get_feature_name(f::plosive)     == "plosive");
	assert(tts::get_feature_name(f::fricative)   == "fricative");
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
	assert(tts::get_feature_name(f::non_syllabic)   == "non-syllabic");
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
		{ "{lmd,bck,unr,vwl,nsy}", { f::lower_mid, f::back, f::unrounded, f::vowel, f::non_syllabic } },
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
		{ "{lmd,bck,unr,vwl,nsy}", { f::lower_mid, f::back, f::unrounded, f::vowel, f::non_syllabic } },
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
