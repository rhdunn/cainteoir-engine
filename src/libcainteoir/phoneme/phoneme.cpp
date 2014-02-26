/* Phoneme Model.
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

#include "config.h"
#include "i18n.h"
#include "compatibility.hpp"

#include <cainteoir/phoneme.hpp>
#include <utility>
#include <string.h>

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;

struct feature_data
{
	const char *abbreviation;
	ipa::phoneme::value_type value;
	ipa::phoneme::value_type mask;
};

static const std::initializer_list<const feature_data> kirshenbaum = {
	{ "adv", ipa::advanced, ipa::articulation }, // extension
	{ "alp", ipa::alveolo_palatal, ipa::place_of_articulation }, // extension
	{ "alv", ipa::alveolar, ipa::place_of_articulation },
	{ "apc", ipa::apical, ipa::articulation }, // extension
	{ "apr", ipa::approximant, ipa::manner_of_articulation },
	{ "asp", ipa::aspirated, ipa::release },
	{ "atr", ipa::advanced_tongue_root, ipa::tongue_root }, // extension
	{ "bck", ipa::back, ipa::vowel_backness },
	{ "blb", ipa::bilabial, ipa::place_of_articulation },
	{ "clk", ipa::click, ipa::manner_of_articulation },
	{ "cnt", ipa::center, ipa::vowel_backness },
	{ "crv", ipa::creaky_voice, ipa::phonation }, // extension
	{ "ctl", 0, ipa::lateral },
	{ "czd", ipa::centralized, ipa::articulation }, // extension
	{ "dnt", ipa::dental, ipa::place_of_articulation },
	{ "dst", ipa::downstep, ipa::phoneme_type }, // extension
	{ "dzd", ipa::dentalized, ipa::articulation }, // extension
	{ "ejc", ipa::ejective, ipa::manner_of_articulation },
	{ "epg", ipa::epiglottal, ipa::place_of_articulation }, // extension
	{ "est", ipa::extra_short, ipa::length }, // extension
	{ "fbr", ipa::foot_break, ipa::phoneme_type }, // extension
	{ "flp", ipa::flap, ipa::manner_of_articulation },
	{ "fnt", ipa::front, ipa::vowel_backness },
	{ "frc", ipa::fricative, ipa::manner_of_articulation },
	{ "fzd", ipa::pharyngealized, ipa::coarticulation },
	{ "glf", ipa::global_fall, ipa::phoneme_type }, // extension
	{ "glr", ipa::global_rise, ipa::phoneme_type }, // extension
	{ "glt", ipa::glottal, ipa::place_of_articulation },
	{ "hgh", ipa::high, ipa::vowel_height },
	{ "hlg", ipa::half_long, ipa::length }, // extension
	{ "ibr", ipa::intonation_break, ipa::phoneme_type }, // extension
	{ "imp", ipa::implosive, ipa::manner_of_articulation },
	{ "lat", ipa::lateral, ipa::lateral },
	{ "lbd", ipa::labio_dental, ipa::place_of_articulation },
	{ "lbp", ipa::labio_palatal, ipa::place_of_articulation }, // extension
	{ "lbv", ipa::labio_velar, ipa::place_of_articulation },
	{ "lgl", ipa::linguolabial, ipa::articulation }, // extension
	{ "lmd", ipa::lower_mid, ipa::vowel_height },
	{ "lmn", ipa::laminal, ipa::articulation }, // extension
	{ "lng", ipa::long_, ipa::length },
	{ "lnk", ipa::linking, ipa::phoneme_type }, // extension
	{ "low", ipa::low, ipa::vowel_height },
	{ "lrd", ipa::less_rounded, ipa::rounding }, // extension
	{ "ltr", ipa::lateral_release, ipa::release }, // extension
	{ "lwr", ipa::lowered, ipa::articulation }, // extension
	{ "lzd", ipa::labialized, ipa::coarticulation },
	{ "mcz", ipa::mid_centralized, ipa::articulation }, // extension
	{ "mid", ipa::mid, ipa::vowel_height },
	{ "mrd", ipa::more_rounded, ipa::rounding }, // extension
	{ "mrm", ipa::breathy_voice, ipa::phonation },
	{ "nas", ipa::nasal, ipa::manner_of_articulation },
	{ "nsy", 0, ipa::syllabic },
	{ "nzd", ipa::nasalized, ipa::coarticulation },
	{ "nzr", ipa::nasal_release, ipa::release }, // extension
	{ "orl", 0, 0 }, // ignored -- Kirshenbaum uses {stp}/{nas} instead of {orl,stp}/{nas,stp}.
	{ "pal", ipa::palatal, ipa::place_of_articulation },
	{ "pau", ipa::pause, ipa::phoneme_type }, // extension
	{ "phr", ipa::pharyngeal, ipa::place_of_articulation },
	{ "pla", ipa::palato_alveolar, ipa::place_of_articulation },
	{ "pzd", ipa::palatalized, ipa::coarticulation },
	{ "ret", ipa::retracted, ipa::articulation }, // extension
	{ "rfx", ipa::retroflex, ipa::place_of_articulation },
	{ "rnd", ipa::rounded, ipa::rounded },
	{ "rsd", ipa::raised, ipa::articulation }, // extension
	{ "rtr", ipa::retracted_tongue_root, ipa::tongue_root }, // extension
	{ "rzd", ipa::rhoticized, ipa::rhoticized },
	{ "sbr", ipa::syllable_break, ipa::phoneme_type }, // extension
	{ "slv", ipa::slack_voice, ipa::phonation }, // extension
	{ "smh", ipa::semi_high, ipa::vowel_height },
	{ "sml", ipa::semi_low, ipa::vowel_height }, // extension
	{ "st1", ipa::primary_stress, ipa::stress }, // extension
	{ "st2", ipa::secondary_stress, ipa::stress }, // extension
	{ "st3", ipa::extra_stress, ipa::stress }, // extension
	{ "stp", ipa::plosive, ipa::manner_of_articulation },
	{ "stv", ipa::stiff_voice, ipa::phonation }, // extension
	{ "syl", ipa::syllabic, ipa::syllabic },
	{ "te1", ipa::tone_end_bottom, ipa::tone_end }, // extension
	{ "te2", ipa::tone_end_low, ipa::tone_end }, // extension
	{ "te3", ipa::tone_end_mid, ipa::tone_end }, // extension
	{ "te4", ipa::tone_end_high, ipa::tone_end }, // extension
	{ "te5", ipa::tone_end_top, ipa::tone_end }, // extension
	{ "tm1", ipa::tone_middle_bottom, ipa::tone_middle }, // extension
	{ "tm2", ipa::tone_middle_low, ipa::tone_middle }, // extension
	{ "tm3", ipa::tone_middle_mid, ipa::tone_middle }, // extension
	{ "tm4", ipa::tone_middle_high, ipa::tone_middle }, // extension
	{ "tm5", ipa::tone_middle_top, ipa::tone_middle }, // extension
	{ "trl", ipa::trill, ipa::manner_of_articulation },
	{ "ts1", ipa::tone_start_bottom, ipa::tone_start }, // extension
	{ "ts2", ipa::tone_start_low, ipa::tone_start }, // extension
	{ "ts3", ipa::tone_start_mid, ipa::tone_start }, // extension
	{ "ts4", ipa::tone_start_high, ipa::tone_start }, // extension
	{ "ts5", ipa::tone_start_top, ipa::tone_start }, // extension
	{ "umd", ipa::upper_mid, ipa::vowel_height },
	{ "unr", 0, ipa::rounded },
	{ "unx", ipa::unexploded, ipa::release },
	{ "ust", ipa::upstep, ipa::phoneme_type }, // extension
	{ "uvl", ipa::uvular, ipa::place_of_articulation },
	{ "vcd", ipa::voiced, ipa::voiced },
	{ "vel", ipa::velar, ipa::place_of_articulation },
	{ "vfz", ipa::velarized_or_pharyngealized, ipa::coarticulation }, // extension
	{ "vls", 0, ipa::voiced },
	{ "vwl", ipa::vowel, ipa::phoneme_type },
	{ "vzd", ipa::velarized, ipa::coarticulation },
};

ipa::phoneme &ipa::phoneme::set(const char *feature)
{
	if (!feature) throw tts::phoneme_error("unknown phoneme feature '(null)'");

	int begin = 0;
	int end = kirshenbaum.size() - 1;

	while (begin <= end)
	{
		int pos = (begin + end) / 2;
		auto &item = *(kirshenbaum.begin() + pos);

		int comp = strcmp(item.abbreviation, feature);
		if (comp == 0)
			return set(item.value, item.mask);
		else if (comp < 0)
			begin = pos + 1;
		else
			end = pos - 1;
	}

	char msg[64];
	sprintf(msg, i18n("unknown phoneme feature '%s'"), feature);
	throw tts::phoneme_error(msg);
}

static const std::initializer_list<const std::pair<const char *, tts::feature>> abbreviations = {
	{ "alv", tts::feature::alveolar },
	{ "apr", tts::feature::approximant },
	{ "asp", tts::feature::aspirated },
	{ "bck", tts::feature::back },
	{ "blb", tts::feature::bilabial },
	{ "clk", tts::feature::click },
	{ "cnt", tts::feature::center },
	{ "ctl", tts::feature::unspecified },
	{ "dnt", tts::feature::dental },
	{ "ejc", tts::feature::ejective },
	{ "epg", tts::feature::epiglottal },
	{ "est", tts::feature::extra_short },
	{ "flp", tts::feature::flap },
	{ "fnt", tts::feature::front },
	{ "frc", tts::feature::fricative },
	{ "fzd", tts::feature::pharyngealized },
	{ "glt", tts::feature::glottal },
	{ "hgh", tts::feature::high },
	{ "hlg", tts::feature::half_long },
	{ "imp", tts::feature::implosive },
	{ "lat", tts::feature::lateral },
	{ "lbd", tts::feature::labio_dental },
	{ "lmd", tts::feature::lower_mid },
	{ "lng", tts::feature::long_ },
	{ "low", tts::feature::low },
	{ "lzd", tts::feature::labialized },
	{ "mid", tts::feature::mid },
	{ "mrm", tts::feature::murmured },
	{ "nas", tts::feature::nasal },
	{ "nsy", tts::feature::non_syllabic },
	{ "nzd", tts::feature::nasalized },
	{ "orl", tts::feature::unspecified },
	{ "pal", tts::feature::palatal },
	{ "pau", tts::feature::silent_pause },
	{ "phr", tts::feature::pharyngeal },
	{ "pla", tts::feature::palato_alveolar },
	{ "pzd", tts::feature::palatalized },
	{ "rfx", tts::feature::retroflex },
	{ "rnd", tts::feature::rounded },
	{ "rzd", tts::feature::rhoticized },
	{ "sbr", tts::feature::syllable_break },
	{ "sib", tts::feature::sibilant },
	{ "smh", tts::feature::semi_high },
	{ "sml", tts::feature::semi_low },
	{ "st1", tts::feature::primary_stress },
	{ "st2", tts::feature::secondary_stress },
	{ "stp", tts::feature::plosive },
	{ "syl", tts::feature::syllabic },
	{ "trl", tts::feature::trill },
	{ "umd", tts::feature::upper_mid },
	{ "unr", tts::feature::unrounded },
	{ "unx", tts::feature::unreleased },
	{ "uvl", tts::feature::uvular },
	{ "vcd", tts::feature::voiced },
	{ "vel", tts::feature::velar },
	{ "vls", tts::feature::voiceless },
	{ "vwl", tts::feature::vowel },
	{ "vzd", tts::feature::velarized },
};

static const std::pair<const char *, const char *> feature_data[] = {
	{ nullptr, nullptr },

	{ "vwl", i18n("vowel") },

	// phonation

	{ "vcd", i18n("voiced") },
	{ "vls", i18n("voiceless") },
	{ "mrm", i18n("murmured") },

	// place of articulation

	{ "blb", i18n("bilabial") },
	{ "lbd", i18n("labio-dental") },
	{ "dnt", i18n("dental") },
	{ "alv", i18n("alveolar") },
	{ "rfx", i18n("retroflex") },
	{ "pla", i18n("palato-alveolar") },
	{ "pal", i18n("palatal") },
	{ "vel", i18n("velar") },
	{ "uvl", i18n("uvular") },
	{ "phr", i18n("pharyngeal") },
	{ "epg", i18n("epiglottal") },
	{ "glt", i18n("glottal") },

	// manner of articulation

	{ "lat", i18n("lateral") },
	{ "sib", i18n("sibilant") },

	{ "stp", i18n("plosive") },
	{ "frc", i18n("fricative") },
	{ "nas", i18n("nasal") },
	{ "apr", i18n("approximant") },
	{ "trl", i18n("trill") },
	{ "flp", i18n("flap") },
	{ "clk", i18n("click") },
	{ "ejc", i18n("ejective") },
	{ "imp", i18n("implosive") },

	// vowel height

	{ "hgh", i18n("high") },
	{ "smh", i18n("semi-high") },
	{ "umd", i18n("upper-mid") },
	{ "mid", i18n("mid") },
	{ "lmd", i18n("lower-mid") },
	{ "sml", i18n("semi-low") },
	{ "low", i18n("low") },

	// vowel backness

	{ "fnt", i18n("front") },
	{ "cnt", i18n("center") },
	{ "bck", i18n("back") },

	// vowel rounding

	{ "unr", i18n("unrounded") },
	{ "rnd", i18n("rounded") },

	// stress

	{ "st1", i18n("primary stress") },
	{ "st2", i18n("secondary stress") },

	// prosody

	{ "sbr", i18n("syllable break") },
	{ "pau", i18n("pause") },

	// length

	{ "est", i18n("extra short") },
	{ "hlg", i18n("half long") },
	{ "lng", i18n("long") },

	// consonant release

	{ "asp", i18n("aspirated") },
	{ "unx", i18n("unreleased") },

	// syllabicity

	{ "syl", i18n("syllabic") },
	{ "nsy", i18n("non-syllabic") },

	// co-articulation

	{ "vzd", i18n("velarized") },
	{ "lzd", i18n("labialized") },
	{ "pzd", i18n("palatalized") },
	{ "rzd", i18n("rhoticized") },
	{ "nzd", i18n("nasalized") },
	{ "fzd", i18n("pharyngealized") },
};

tts::phoneme::phoneme()
{
	features[0] = tts::feature::unspecified;
	features[1] = tts::feature::unspecified;
	features[2] = tts::feature::unspecified;
	features[3] = tts::feature::unspecified;
	features[4] = tts::feature::unspecified;
	features[5] = tts::feature::unspecified;
	features[6] = tts::feature::unspecified;
	features[7] = tts::feature::unspecified;
}

tts::phoneme::phoneme(const feature a,
                      const feature b,
                      const feature c,
                      const feature d,
                      const feature e,
                      const feature f,
                      const feature g,
                      const feature h)
{
	features[0] = a;
	features[1] = b;
	features[2] = c;
	features[3] = d;
	features[4] = e;
	features[5] = f;
	features[6] = g;
	features[7] = h;
}

bool tts::phoneme::contains(const feature f) const
{
	if (f != feature::unspecified) for (const feature x : features)
	{
		if (x == f) return true;
	}
	return false;
}

bool tts::phoneme::remove(const feature f)
{
	if (f != feature::unspecified) for (feature &x : features)
	{
		if (x == f)
		{
			x = tts::feature::unspecified;
			return true;
		}
	}
	return false;
}

bool tts::phoneme::add(const feature f)
{
	if (f != feature::unspecified && !contains(f)) for (feature &x : features)
	{
		if (x == tts::feature::unspecified)
		{
			x = f;
			return true;
		}
	}
	return false;
}

bool tts::phoneme::operator==(const phoneme &rhs) const
{
	int lhs_unspecified = 0;
	for (const feature x : features)
	{
		if (x == feature::unspecified) { ++lhs_unspecified; continue; }

		if (!rhs.contains(x)) return false;
	}

	int rhs_unspecified = 0;
	for (const feature x : rhs.features)
	{
		if (x == feature::unspecified) ++rhs_unspecified;
	}
	return lhs_unspecified == rhs_unspecified;
}

std::pair<bool, tts::feature> tts::get_feature_id(const char *abbreviation)
{
	if (!abbreviation) return { false, tts::feature::unspecified };

	int begin = 0;
	int end = abbreviations.size() - 1;

	while (begin <= end)
	{
		int pos = (begin + end) / 2;
		auto &item = *(abbreviations.begin() + pos);

		int comp = strcmp(item.first, abbreviation);
		if (comp == 0)
			return { true, (abbreviations.begin() + pos)->second };
		else if (comp < 0)
			begin = pos + 1;
		else
			end = pos - 1;
	}

	return { false, tts::feature::unspecified };
}

const char *tts::get_feature_abbreviation(const feature f)
{
	if ((int)f > (int)feature::pharyngealized) return nullptr;
	return feature_data[(int)f].first;
}

const char *tts::get_feature_name(const feature f)
{
	if ((int)f > (int)feature::pharyngealized) return nullptr;
	return feature_data[(int)f].second;
}
