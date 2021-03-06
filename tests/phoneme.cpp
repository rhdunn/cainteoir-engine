/* Test for the phoneme model.
 *
 * Copyright (C) 2013-2015 Reece H. Dunn
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

namespace cainteoir { namespace ipa
{
	std::ostream &operator<<(std::ostream &os, const phoneme &value)
	{
		auto fill = os.fill();
		return os << "0x"
		          << std::hex << std::setw(16) << std::setfill('0') << std::uppercase
		          << value.get(FEATURE_C(0xFFFFFFFFFFFFFFFF))
		          << std::dec << std::setw(0) << std::setfill(fill) << std::nouppercase;
	}

	std::ostream &operator<<(std::ostream &os, const tts::phoneme_reader &reader)
	{
		return os << (const ipa::phoneme &)reader;
	}
}}

PRINTABLE(ipa::phoneme);
PRINTABLE(tts::phoneme_reader);

TEST_CASE("ipa::phoneme -- object size")
{
	assert(sizeof(ipa::phoneme) == 8);
}

TEST_CASE("ipa::phoneme -- construction")
{
	const ipa::phoneme::value_type zero = FEATURE_C(0x0000000000000000);
	const ipa::phoneme::value_type val1 = FEATURE_C(0x1234567812345678);
	const ipa::phoneme::value_type val2 = FEATURE_C(0x8765432187654321);

	assert(ipa::phoneme() == ipa::phoneme(zero));
	assert(ipa::phoneme(val1) == ipa::phoneme(val1));
	assert(ipa::phoneme(val1) != ipa::phoneme(val2));

	ipa::phoneme a(val1);
	ipa::phoneme b(val2);

	assert(ipa::phoneme(a) == ipa::phoneme(val1));
	assert(ipa::phoneme(b) != ipa::phoneme(val1));
}

TEST_CASE("ipa::phoneme -- comparison")
{
	const ipa::phoneme::value_type zero = FEATURE_C(0x0000000000000000);
	const ipa::phoneme::value_type val1 = FEATURE_C(0x1234567812345678);
	const ipa::phoneme::value_type val2 = FEATURE_C(0x8765432187654321);

	ipa::phoneme a(val1);
	ipa::phoneme b(val2);

	assert(a == ipa::phoneme(val1));
	assert_false(b == ipa::phoneme(val1));

	assert_false(a != ipa::phoneme(val1));
	assert(b != ipa::phoneme(val1));

	assert(a == val1);
	assert_false(b == val1);

	assert_false(a != val1);
	assert(b != val1);

	assert(val1 == a);
	assert_false(val1 == b);

	assert_false(val1 != a);
	assert(val1 != b);
}

TEST_CASE("ipa::phoneme -- assignment")
{
	const ipa::phoneme::value_type zero = FEATURE_C(0x0000000000000000);
	const ipa::phoneme::value_type fill = FEATURE_C(0xFFFFFFFFFFFFFFFF);
	const ipa::phoneme::value_type val1 = FEATURE_C(0x1234567812345678);

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
	const ipa::phoneme::value_type zero = FEATURE_C(0x0000000000000000);
	const ipa::phoneme::value_type fill = FEATURE_C(0xFFFFFFFFFFFFFFFF);
	const ipa::phoneme::value_type val1 = FEATURE_C(0x1234567812345678);
	const ipa::phoneme::value_type mask = FEATURE_C(0x1200000000000000);

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
	const ipa::phoneme::value_type zero = FEATURE_C(0x0000000000000000);
	const ipa::phoneme::value_type fill = FEATURE_C(0xFFFFFFFFFFFFFFFF);
	const ipa::phoneme::value_type mask = FEATURE_C(0xFF00000000000000);

	assert(ipa::phoneme(zero).set(fill) == ipa::phoneme(fill));
	assert(ipa::phoneme(zero).set(mask) == ipa::phoneme(mask));
	assert(ipa::phoneme(zero).set(zero) == ipa::phoneme(zero));

	assert(ipa::phoneme(fill).set(fill) == ipa::phoneme(fill));
	assert(ipa::phoneme(fill).set(mask) == ipa::phoneme(fill));
	assert(ipa::phoneme(fill).set(zero) == ipa::phoneme(fill));
}

TEST_CASE("ipa::phoneme -- set with bit mask")
{
	const ipa::phoneme::value_type zero = FEATURE_C(0x0000000000000000);
	const ipa::phoneme::value_type fill = FEATURE_C(0xFFFFFFFFFFFFFFFF);
	const ipa::phoneme::value_type mask = FEATURE_C(0xFF00000000000000);
	const ipa::phoneme::value_type invm = FEATURE_C(0x00FFFFFFFFFFFFFF);

	assert(ipa::phoneme(zero).set(fill, fill) == ipa::phoneme(fill));
	assert(ipa::phoneme(zero).set(fill, mask) == ipa::phoneme(mask));
	assert(ipa::phoneme(zero).set(fill, zero) == ipa::phoneme(zero));

	assert(ipa::phoneme(fill).set(zero, fill) == ipa::phoneme(zero));
	assert(ipa::phoneme(fill).set(zero, mask) == ipa::phoneme(invm));
	assert(ipa::phoneme(fill).set(zero, zero) == ipa::phoneme(fill));
}

TEST_CASE("ipa::phoneme -- clear")
{
	const ipa::phoneme::value_type zero = FEATURE_C(0x0000000000000000);
	const ipa::phoneme::value_type fill = FEATURE_C(0xFFFFFFFFFFFFFFFF);
	const ipa::phoneme::value_type mask = FEATURE_C(0xFF00000000000000);
	const ipa::phoneme::value_type invm = FEATURE_C(0x00FFFFFFFFFFFFFF);

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
		const ipa::phoneme::value_type zero = FEATURE_C(0x0000000000000000); \
		const ipa::phoneme::value_type fill = FEATURE_C(0xFFFFFFFFFFFFFFFF); \
		assert(!ipa::phoneme(zero).get(abbr)); \
		assert(ipa::phoneme(fill).get(abbr)); \
		assert(ipa::phoneme(zero).set(abbr) == ipa::phoneme(value)); \
		assert(ipa::phoneme(fill).set(abbr) == ipa::phoneme(fill)); \
	}

#define KIRSHENBAUM_BIN0(abbr, label, value) /* Disable Binary Feature */ \
	TEST_CASE("kirshenbaum -- " abbr " = " label) \
	{ \
		const ipa::phoneme::value_type zero = FEATURE_C(0x0000000000000000); \
		const ipa::phoneme::value_type fill = FEATURE_C(0xFFFFFFFFFFFFFFFF); \
		assert(ipa::phoneme(zero).get(abbr)); \
		assert(!ipa::phoneme(fill).get(abbr)); \
		assert(ipa::phoneme(zero).set(abbr) == ipa::phoneme(zero)); \
		assert(ipa::phoneme(fill).set(abbr) == ipa::phoneme(fill & ~value)); \
	} \

#define KIRSHENBAUM_RNGD(abbr, label, value, mask) /* Ranged (Value + Mask) Feature */ \
	TEST_CASE("kirshenbaum -- " abbr " = " label) \
	{ \
		const ipa::phoneme::value_type zero = FEATURE_C(0x0000000000000000); \
		const ipa::phoneme::value_type fill = FEATURE_C(0xFFFFFFFFFFFFFFFF); \
		if (value) { \
			assert(!ipa::phoneme(zero).get(abbr)); \
			assert(ipa::phoneme((fill & ~mask) | value).get(abbr)); \
		} else { \
			assert(ipa::phoneme(zero).get(abbr)); \
			assert(!ipa::phoneme(fill).get(abbr)); \
		} \
		assert(ipa::phoneme(zero).set(abbr) == ipa::phoneme(value)); \
		assert(ipa::phoneme(fill).set(abbr) == ipa::phoneme((fill & ~mask) | value)); \
	} \

#define KIRSHENBAUM_NULL(abbr, label, value, mask) /* Unsupported Feature */ \
	TEST_CASE("kirshenbaum -- " abbr " = " label) \
	{ \
		const ipa::phoneme::value_type zero = FEATURE_C(0x0000000000000000); \
		const ipa::phoneme::value_type fill = FEATURE_C(0xFFFFFFFFFFFFFFFF); \
		assert(!ipa::phoneme(zero).get(abbr)); \
		assert(!ipa::phoneme(fill).get(abbr)); \
		assert(ipa::phoneme(zero).set(abbr) == ipa::phoneme(value)); \
		assert(ipa::phoneme(fill).set(abbr) == ipa::phoneme(fill)); \
	} \

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
KIRSHENBAUM_NULL("orl", "oral", 0, 0) // Not Supported -- Kirshenbaum does not use {orl,stp}/{nas,stp}
KIRSHENBAUM_RNGD("apr", "approximant", ipa::approximant, ipa::manner_of_articulation)
KIRSHENBAUM_RNGD("trl", "trill", ipa::trill, ipa::manner_of_articulation)
KIRSHENBAUM_RNGD("flp", "flap", ipa::flap, ipa::manner_of_articulation)
KIRSHENBAUM_RNGD("clk", "click", ipa::click, ipa::manner_of_articulation)
KIRSHENBAUM_BIN1("ejc", "ejective", ipa::ejective)
KIRSHENBAUM_RNGD("imp", "implosive", ipa::implosive, ipa::manner_of_articulation)
KIRSHENBAUM_BIN1("sib", "sibilant [extension]", ipa::sibilant)
KIRSHENBAUM_BIN1("lat", "lateral", ipa::lateral)
KIRSHENBAUM_BIN0("ctl", "central", ipa::lateral)
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
KIRSHENBAUM_RNGD("con", "consonant [extension]", 0, ipa::phoneme_type)
KIRSHENBAUM_RNGD("vwl", "vowel", ipa::vowel, ipa::phoneme_type)
KIRSHENBAUM_RNGD("pau", "pause (silence) [extension]", ipa::separator, ipa::phoneme_type)
KIRSHENBAUM_RNGD("sbr", "syllable break [extension]", ipa::syllable_break, ipa::phoneme_type)
KIRSHENBAUM_RNGD("lnk", "linking (no break) [extension]", ipa::linking, ipa::phoneme_type)
KIRSHENBAUM_RNGD("fbr", "foot (minor) break [extension]", ipa::foot_break, ipa::phoneme_type)
KIRSHENBAUM_RNGD("ibr", "intonation (major) break [extension]", ipa::intonation_break, ipa::phoneme_type)
KIRSHENBAUM_RNGD("glr", "global rise [extension]", ipa::global_rise, ipa::phoneme_type)
KIRSHENBAUM_RNGD("glf", "global fall [extension]", ipa::global_fall, ipa::phoneme_type)
KIRSHENBAUM_RNGD("ust", "upstep [extension]", ipa::upstep, ipa::phoneme_type)
KIRSHENBAUM_RNGD("dst", "downstep [extension]", ipa::downstep, ipa::phoneme_type)
KIRSHENBAUM_RNGD("ts5", "extra-high (top) start tone [extension]", ipa::tone_start_top, ipa::tone_start)
KIRSHENBAUM_RNGD("ts4", "high start tone [extension]", ipa::tone_start_high, ipa::tone_start)
KIRSHENBAUM_RNGD("ts3", "mid start tone [extension]", ipa::tone_start_mid, ipa::tone_start)
KIRSHENBAUM_RNGD("ts2", "low start tone [extension]", ipa::tone_start_low, ipa::tone_start)
KIRSHENBAUM_RNGD("ts1", "extra-low (bottom) start tone [extension]", ipa::tone_start_bottom, ipa::tone_start)
KIRSHENBAUM_RNGD("tm5", "extra-high (top) middle tone [extension]", ipa::tone_middle_top, ipa::tone_middle)
KIRSHENBAUM_RNGD("tm4", "high middle tone [extension]", ipa::tone_middle_high, ipa::tone_middle)
KIRSHENBAUM_RNGD("tm3", "mid middle tone [extension]", ipa::tone_middle_mid, ipa::tone_middle)
KIRSHENBAUM_RNGD("tm2", "low middle tone [extension]", ipa::tone_middle_low, ipa::tone_middle)
KIRSHENBAUM_RNGD("tm1", "extra-low (bottom) middle tone [extension]", ipa::tone_middle_bottom, ipa::tone_middle)
KIRSHENBAUM_RNGD("te5", "extra-high (top) end tone [extension]", ipa::tone_end_top, ipa::tone_end)
KIRSHENBAUM_RNGD("te4", "high end tone [extension]", ipa::tone_end_high, ipa::tone_end)
KIRSHENBAUM_RNGD("te3", "mid end tone [extension]", ipa::tone_end_mid, ipa::tone_end)
KIRSHENBAUM_RNGD("te2", "low end tone [extension]", ipa::tone_end_low, ipa::tone_end)
KIRSHENBAUM_RNGD("te1", "extra-low (bottom) end tone [extension]", ipa::tone_end_bottom, ipa::tone_end)
KIRSHENBAUM_BIN1("tie", "tied (affricate/diphthong/...) [extension]", ipa::joined_to_next_phoneme)
KIRSHENBAUM_RNGD("syl", "syllabic", ipa::syllabic, ipa::syllabicity)
KIRSHENBAUM_RNGD("nsy", "non-syllabic [extension]", ipa::non_syllabic, ipa::syllabicity)
KIRSHENBAUM_BIN1("unx", "unexploded", ipa::unexploded)
KIRSHENBAUM_RNGD("vls", "voiceless", ipa::voiceless, ipa::voicing)
KIRSHENBAUM_RNGD("mrm", "murmured", ipa::murmured, ipa::voicing)
KIRSHENBAUM_RNGD("brv", "breathy voice [extension]", ipa::breathy_voice, ipa::phonation)
KIRSHENBAUM_RNGD("slv", "slack voice [extension]", ipa::slack_voice, ipa::phonation)
KIRSHENBAUM_RNGD("vcd", "voiced", ipa::voiced, ipa::voicing)
KIRSHENBAUM_RNGD("stv", "stiff voice [extension]", ipa::stiff_voice, ipa::phonation)
KIRSHENBAUM_RNGD("crv", "creaky voice [extension]", ipa::creaky_voice, ipa::phonation)
KIRSHENBAUM_RNGD("dzd", "dentalized (dental diacritic) [extension]", ipa::dentalized, ipa::articulation)
KIRSHENBAUM_RNGD("lgl", "linguolabial [extension]", ipa::linguolabial, ipa::articulation)
KIRSHENBAUM_RNGD("apc", "apical [extension]", ipa::apical, ipa::articulation)
KIRSHENBAUM_RNGD("lmn", "laminal [extension]", ipa::laminal, ipa::articulation)
KIRSHENBAUM_RNGD("adv", "advanced [extension]", ipa::advanced, ipa::articulation)
KIRSHENBAUM_RNGD("ret", "retracted [extension]", ipa::retracted, ipa::articulation)
KIRSHENBAUM_RNGD("czd", "centralized [extension]", ipa::centralized, ipa::articulation)
KIRSHENBAUM_RNGD("mcz", "mid-centralized [extension]", ipa::mid_centralized, ipa::articulation)
KIRSHENBAUM_RNGD("rsd", "raised [extension]", ipa::raised, ipa::articulation)
KIRSHENBAUM_RNGD("lwr", "lowered [extension]", ipa::lowered, ipa::articulation)
KIRSHENBAUM_RNGD("mrd", "more rounded [extension]", ipa::more_rounded, ipa::rounding)
KIRSHENBAUM_RNGD("lrd", "less rounded [extension]", ipa::less_rounded, ipa::rounding)
KIRSHENBAUM_RNGD("vfz", "velarized or pharyngealized [extension]", ipa::velarized_or_pharyngealized, ipa::coarticulation)
KIRSHENBAUM_RNGD("nzd", "nasalized", ipa::nasalized, ipa::coarticulation)
KIRSHENBAUM_BIN1("rzd", "rhoticized", ipa::rhoticized)
KIRSHENBAUM_RNGD("atr", "advanced tongue root [extension]", ipa::advanced_tongue_root, ipa::tongue_root)
KIRSHENBAUM_RNGD("rtr", "retracted tongue root [extension]", ipa::retracted_tongue_root, ipa::tongue_root)
KIRSHENBAUM_BIN1("dcz", "diacritized [extension]", ipa::diacritized)

TEST_CASE("explicit feature reader -- no input")
{
	std::shared_ptr<tts::phoneme_reader> reader = tts::createPhonemeReader("features");
	assert(reader.get());
	assert(*reader == ipa::unspecified);

	assert(!reader->read());
	assert(*reader == ipa::phoneme());

	reader->reset(std::shared_ptr<cainteoir::buffer>());

	assert(!reader->read());
	assert(*reader == ipa::phoneme());

	auto test = std::make_shared<cainteoir::buffer>(nullptr, nullptr);
	reader->reset(test);

	assert(!reader->read());
	assert(*reader == ipa::phoneme());
}

TEST_CASE("explicit feature parser -- no input")
{
	const char *current = nullptr;
	const char *end = nullptr;
	const char *test = "{vcd,alv,stp}";
	ipa::phoneme p;

	std::shared_ptr<tts::phoneme_parser> parser = tts::createPhonemeParser("features");
	assert(parser.get());

	p = ipa::bilabial | ipa::flap;
	assert(!parser->parse(current, end, p));
	assert(current == nullptr);
	assert(end == nullptr);
	assert(p == ipa::phoneme());

	current = end = test;
	p = ipa::bilabial | ipa::flap;
	assert(!parser->parse(current, end, p));
	assert(current == test);
	assert(end == test);
	assert(p == ipa::phoneme());
}

static const std::initializer_list<const char *> phoneme_features = {
	"adv", "alp", "alv", "apc", "apr", "atr",
	"bck", "blb", "brv",
	"clk", "cnt", "con", "crv", "ctl", "czd",
	"dcz", "dnt", "dst", "dzd",
	"ejc", "epg", "est",
	"fbr", "flp", "fnt", "frc",
	"glf", "glr", "glt",
	"hgh", "hlg",
	"ibr", "imp",
	"lat", "lbd", "lbp", "lbv", "lgl", "lmd", "lmn", "lng", "lnk", "low", "lrd", "lwr",
	"mcz", "mid", "mrd", "mrm",
	"nas", "nsy", "nzd",
	"orl",
	"pal", "pau", "phr", "pla",
	"ret", "rfx", "rnd", "rsd", "rtr", "rzd",
	"sbr", "sib", "slv", "smh", "sml", "st1", "st2", "st3", "stp", "stv", "syl",
	"te1", "te2", "te3", "te4", "te5",
	"tie",
	"tm1", "tm2", "tm3", "tm4", "tm5", "trl",
	"ts1", "ts2", "ts3", "ts4", "ts5",
	"umd", "unr", "unx", "ust", "uvl",
	"vcd", "vel", "vfz", "vls", "vwl",
};

TEST_CASE("explicit feature reader -- single feature")
{
	std::shared_ptr<tts::phoneme_reader> reader = tts::createPhonemeReader("features");
	for (const auto &test : phoneme_features)
	{
		char phoneme[] = { '{', test[0], test[1], test[2], '}', 0 };

		fprintf(stdout, "... ... testing feature %s\n", phoneme);
		reader->reset(std::make_shared<cainteoir::buffer>(phoneme));

		assert(reader->read());
		assert(*reader == ipa::phoneme().set(test));

		assert(!reader->read());
		assert(*reader == ipa::phoneme());
	}
}

TEST_CASE("explicit feature parser -- single feature")
{
	std::shared_ptr<tts::phoneme_parser> parser = tts::createPhonemeParser("features");
	for (const auto &test : phoneme_features)
	{
		char phoneme[] = { '{', test[0], test[1], test[2], '}', 0 };
		const char *current = phoneme;
		const char *end = phoneme + 5;
		ipa::phoneme p = ipa::unit;

		fprintf(stdout, "... ... testing feature %s\n", phoneme);
		assert(parser->parse(current, end, p));
		assert(current == end);
		assert(p == ipa::phoneme().set(test));
	}
}

TEST_CASE("explicit feature reader -- multiple phonemes")
{
	std::shared_ptr<tts::phoneme_reader> reader = tts::createPhonemeReader("features");

	const char *test = "{vls,alv,stp}{low,fnt,unr,vwl}{vcd,vel,stp}"; // = /t&g/
	reader->reset(std::make_shared<cainteoir::buffer>(test));

	assert(reader->read());
	assert(*reader == ipa::phoneme(ipa::alveolar | ipa::plosive));

	assert(reader->read());
	assert(*reader == ipa::phoneme(ipa::low | ipa::front | ipa::vowel));

	assert(reader->read());
	assert(*reader == ipa::phoneme(ipa::voiced | ipa::velar | ipa::plosive));

	assert(!reader->read());
	assert(*reader == ipa::phoneme());
}

TEST_CASE("explicit feature parser -- multiple phonemes")
{
	std::shared_ptr<tts::phoneme_parser> parser = tts::createPhonemeParser("features");

	const char *test = "{vls,alv,stp}{low,fnt,unr,vwl}{vcd,vel,stp}"; // = /t&g/
	const char *current = test;
	const char *end = test + strlen(test);
	ipa::phoneme p = ipa::unit;

	assert(parser->parse(current, end, p));
	assert(current == test + 13);
	assert(p == ipa::phoneme(ipa::alveolar | ipa::plosive));

	assert(parser->parse(current, end, p));
	assert(current == test + 30);
	assert(p == ipa::phoneme(ipa::low | ipa::front | ipa::vowel));

	assert(parser->parse(current, end, p));
	assert(current == end);
	assert(p == ipa::phoneme(ipa::voiced | ipa::velar | ipa::plosive));
}

TEST_CASE("explicit feature reader -- multiple phonemes with whitespace")
{
	std::shared_ptr<tts::phoneme_reader> reader = tts::createPhonemeReader("features");

	const char *test = "\r\t{vls,alv,stp}\n {low,fnt,unr,vwl}\r\n{vcd,vel,stp}"; // = /t&g/
	reader->reset(std::make_shared<cainteoir::buffer>(test));

	assert(reader->read());
	assert(*reader == ipa::phoneme(ipa::alveolar | ipa::plosive));

	assert(reader->read());
	assert(*reader == ipa::phoneme(ipa::low | ipa::front | ipa::vowel));

	assert(reader->read());
	assert(*reader == ipa::phoneme(ipa::voiced | ipa::velar | ipa::plosive));

	assert(!reader->read());
	assert(*reader == ipa::phoneme());
}

TEST_CASE("explicit feature parser -- multiple phonemes with whitespace")
{
	std::shared_ptr<tts::phoneme_parser> parser = tts::createPhonemeParser("features");

	const char *test = "\r\t{vls,alv,stp}\n {low,fnt,unr,vwl}\r\n{vcd,vel,stp}"; // = /t&g/
	const char *current = test;
	const char *end = test + strlen(test);
	ipa::phoneme p = ipa::unit;

	assert(parser->parse(current, end, p));
	assert(current == test + 15);
	assert(p == ipa::phoneme(ipa::alveolar | ipa::plosive));

	assert(parser->parse(current, end, p));
	assert(current == test + 34);
	assert(p == ipa::phoneme(ipa::low | ipa::front | ipa::vowel));

	assert(parser->parse(current, end, p));
	assert(current == end);
	assert(p == ipa::phoneme(ipa::voiced | ipa::velar | ipa::plosive));
}

struct phoneme_test_t
{
	const char *phoneme;
	int error_offset;
	const std::string message;
	ipa::phoneme next_phoneme;
};

static const std::initializer_list<phoneme_test_t> invalid_phonemes = {
	// missing '}' at the end of the phoneme ...
	{ "{vcd,alv,frc",      12, "unexpected end of phoneme (expecting '}')", {} },
	{ "{vcd,alv,",          9, "unexpected end of phoneme (expecting '}')", {} },
	{ "{",                  1, "unexpected end of phoneme (expecting '}')", {} },
	// no '{' to start the phoneme ...
	{ "}{dnt}",             1, "unexpected start of phoneme (expecting '{')", { ipa::dental } },
	{ "vcd,alv,frc{dnt}",  11, "unexpected start of phoneme (expecting '{')", { ipa::dental } },
	{ ",alv,frc{dnt}",      8, "unexpected start of phoneme (expecting '{')", { ipa::dental } },
	{ "@{dnt}",             1, "unexpected start of phoneme (expecting '{')", { ipa::dental } },
	{ "5{dnt}",             1, "unexpected start of phoneme (expecting '{')", { ipa::dental } },
	// no phoneme feature specified ...
	{ "{}{dnt}",            2, "missing phoneme feature", { ipa::dental } },
	{ "{vcd,}{dnt}",        6, "missing phoneme feature", { ipa::dental } },
	{ "{,lbd}{dnt}",        6, "missing phoneme feature", { ipa::dental } },
	// features shorter/longer than 3 characters ...
	{ "{s}{dnt}",           3, "a phoneme feature must be 3 characters long", { ipa::dental } },
	{ "{5}{dnt}",           3, "a phoneme feature must be 3 characters long", { ipa::dental } },
	{ "{%}{dnt}",           3, "a phoneme feature must be 3 characters long", { ipa::dental } },
	{ "{st}{dnt}",          4, "a phoneme feature must be 3 characters long", { ipa::dental } },
	{ "{stop}{dnt}",        6, "a phoneme feature must be 3 characters long", { ipa::dental } },
	// unknown feature ...
	{ "{aaa}{dnt}",         5, "unknown phoneme feature 'aaa'", { ipa::dental } },
	{ "{xyz}{dnt}",         5, "unknown phoneme feature 'xyz'", { ipa::dental } },
	{ "{STP}{dnt}",         5, "unknown phoneme feature 'STP'", { ipa::dental } },
	{ "{stP}{dnt}",         5, "unknown phoneme feature 'stP'", { ipa::dental } },
	{ "{st5}{dnt}",         5, "unknown phoneme feature 'st5'", { ipa::dental } },
	{ "{st%}{dnt}",         5, "unknown phoneme feature 'st%'", { ipa::dental } },
	// removed features ...
	{ "{asp}",              5, "unknown phoneme feature 'asp'", {} },
	{ "{fzd}",              5, "unknown phoneme feature 'fzd'", {} },
	{ "{ltr}",              5, "unknown phoneme feature 'ltr'", {} },
	{ "{lzd}",              5, "unknown phoneme feature 'lzd'", {} },
	{ "{nzr}",              5, "unknown phoneme feature 'nzr'", {} },
	{ "{pzd}",              5, "unknown phoneme feature 'pzd'", {} },
	{ "{rtc}",              5, "unknown phoneme feature 'rtc'", {} },
	{ "{vzd}",              5, "unknown phoneme feature 'vzd'", {} },
};

TEST_CASE("explicit feature reader -- phoneme errors")
{
	std::shared_ptr<tts::phoneme_reader> reader = tts::createPhonemeReader("features");

	for (const auto &test : invalid_phonemes)
	{
		fprintf(stdout, "... ... testing invalid phoneme %s\n", test.phoneme);
		reader->reset(std::make_shared<cainteoir::buffer>(test.phoneme));

		assert_throws(reader->read(), tts::phoneme_error, test.message);
		assert(*reader == ipa::phoneme());

		assert(reader->read() == (test.next_phoneme != ipa::phoneme()));
		assert(*reader == test.next_phoneme);
	}
}

TEST_CASE("explicit feature parser -- phoneme errors")
{
	std::shared_ptr<tts::phoneme_parser> parser = tts::createPhonemeParser("features");

	for (const auto &test : invalid_phonemes)
	{
		const char *current = test.phoneme;
		const char *end = test.phoneme + strlen(test.phoneme);
		ipa::phoneme p = ipa::unit;

		fprintf(stdout, "... ... testing invalid phoneme %s\n", test.phoneme);
		assert_throws(parser->parse(current, end, p), tts::phoneme_error, test.message);
		assert(current == test.phoneme + test.error_offset);
		assert(p == ipa::phoneme());

		assert(parser->parse(current, end, p) == (test.next_phoneme != ipa::phoneme()));
		assert(current == end);
		assert(p == test.next_phoneme);
	}
}

TEST_CASE("explicit feature writer -- no input")
{
	std::shared_ptr<tts::phoneme_writer> writer = tts::createPhonemeWriter("features");
	assert(writer.get());

	assert(!writer->write({ ipa::glottal | ipa::plosive }));

	cainteoir::memory_file output;
	writer->reset(output);

	auto data = output.buffer();
	assert(data->empty());

	writer->reset(nullptr);
	assert(!writer->write({ ipa::glottal | ipa::plosive }));
}
