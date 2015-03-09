/* Phoneme Model.
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
	{ "atr", ipa::advanced_tongue_root, ipa::tongue_root }, // extension
	{ "bck", ipa::back, ipa::vowel_backness },
	{ "blb", ipa::bilabial, ipa::place_of_articulation },
	{ "brv", ipa::breathy_voice, ipa::phonation },
	{ "clk", ipa::click, ipa::manner_of_articulation },
	{ "cnt", ipa::center, ipa::vowel_backness },
	{ "con", 0, ipa::phoneme_type }, // extension
	{ "crv", ipa::creaky_voice, ipa::phonation }, // extension
	{ "ctl", 0, ipa::lateral },
	{ "czd", ipa::centralized, ipa::articulation }, // extension
	{ "dcz", ipa::diacritized, ipa::diacritized }, // extension
	{ "dnt", ipa::dental, ipa::place_of_articulation },
	{ "dst", ipa::downstep, ipa::phoneme_type }, // extension
	{ "dzd", ipa::dentalized, ipa::articulation }, // extension
	{ "ejc", ipa::ejective, ipa::ejective },
	{ "epg", ipa::epiglottal, ipa::place_of_articulation }, // extension
	{ "est", ipa::extra_short, ipa::length }, // extension
	{ "fbr", ipa::foot_break, ipa::phoneme_type }, // extension
	{ "flp", ipa::flap, ipa::manner_of_articulation },
	{ "fnt", ipa::front, ipa::vowel_backness },
	{ "frc", ipa::fricative, ipa::manner_of_articulation },
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
	{ "lwr", ipa::lowered, ipa::articulation }, // extension
	{ "mcz", ipa::mid_centralized, ipa::articulation }, // extension
	{ "mid", ipa::mid, ipa::vowel_height },
	{ "mrd", ipa::more_rounded, ipa::rounding }, // extension
	{ "mrm", ipa::murmured, ipa::voicing },
	{ "nas", ipa::nasal, ipa::manner_of_articulation },
	{ "nsy", ipa::non_syllabic, ipa::syllabicity },
	{ "nzd", ipa::nasalized, ipa::coarticulation },
	{ "orl", 0, 0 }, // ignored -- Kirshenbaum uses {stp}/{nas} instead of {orl,stp}/{nas,stp}.
	{ "pal", ipa::palatal, ipa::place_of_articulation },
	{ "pau", ipa::separator, ipa::phoneme_type }, // extension
	{ "phr", ipa::pharyngeal, ipa::place_of_articulation },
	{ "pla", ipa::palato_alveolar, ipa::place_of_articulation },
	{ "ret", ipa::retracted, ipa::articulation }, // extension
	{ "rfx", ipa::retroflex, ipa::place_of_articulation },
	{ "rnd", ipa::rounded, ipa::rounded },
	{ "rsd", ipa::raised, ipa::articulation }, // extension
	{ "rtr", ipa::retracted_tongue_root, ipa::tongue_root }, // extension
	{ "rzd", ipa::rhoticized, ipa::rhoticized },
	{ "sbr", ipa::syllable_break, ipa::phoneme_type }, // extension
	{ "sib", ipa::sibilant, ipa::sibilant }, // extension
	{ "slv", ipa::slack_voice, ipa::phonation }, // extension
	{ "smh", ipa::semi_high, ipa::vowel_height },
	{ "sml", ipa::semi_low, ipa::vowel_height }, // extension
	{ "st1", ipa::primary_stress, ipa::stress }, // extension
	{ "st2", ipa::secondary_stress, ipa::stress }, // extension
	{ "st3", ipa::extra_stress, ipa::stress }, // extension
	{ "stp", ipa::plosive, ipa::manner_of_articulation },
	{ "stv", ipa::stiff_voice, ipa::phonation }, // extension
	{ "syl", ipa::syllabic, ipa::syllabicity },
	{ "te1", ipa::tone_end_bottom, ipa::tone_end }, // extension
	{ "te2", ipa::tone_end_low, ipa::tone_end }, // extension
	{ "te3", ipa::tone_end_mid, ipa::tone_end }, // extension
	{ "te4", ipa::tone_end_high, ipa::tone_end }, // extension
	{ "te5", ipa::tone_end_top, ipa::tone_end }, // extension
	{ "tie", ipa::joined_to_next_phoneme, ipa::joined_to_next_phoneme }, // extension
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
	{ "unx", ipa::unexploded, ipa::unexploded },
	{ "ust", ipa::upstep, ipa::phoneme_type }, // extension
	{ "uvl", ipa::uvular, ipa::place_of_articulation },
	{ "vcd", ipa::voiced, ipa::voicing },
	{ "vel", ipa::velar, ipa::place_of_articulation },
	{ "vfz", ipa::velarized_or_pharyngealized, ipa::coarticulation }, // extension
	{ "vls", ipa::voiceless, ipa::voicing },
	{ "vwl", ipa::vowel, ipa::phoneme_type },
};

bool ipa::phoneme::get(const char *feature) const
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
			return (item.mask == 0) ? false : (get(item.mask) == item.value);
		else if (comp < 0)
			begin = pos + 1;
		else
			end = pos - 1;
	}

	char msg[64];
	sprintf(msg, i18n("unknown phoneme feature '%s'"), feature);
	throw tts::phoneme_error(msg);
}

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
