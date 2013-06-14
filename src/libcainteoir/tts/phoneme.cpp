/* Phoneme Model.
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

#include "config.h"
#include "i18n.h"
#include "compatibility.hpp"

#include <cainteoir/phoneme.hpp>
#include <utility>
#include <string.h>

namespace tts = cainteoir::tts;

static const std::initializer_list<const std::pair<const char *, tts::feature>> abbreviations = {
	{ "afr", tts::feature::affricate },
	{ "alv", tts::feature::alveolar },
	{ "apr", tts::feature::approximant },
	{ "asp", tts::feature::aspirated },
	{ "bck", tts::feature::back },
	{ "blb", tts::feature::bilabial },
	{ "clk", tts::feature::click },
	{ "cnt", tts::feature::center },
	{ "ctl", tts::feature::unspecified },
	{ "dnt", tts::feature::dental },
	{ "dpf", tts::feature::falling_diphthong },
	{ "dpr", tts::feature::rising_diphthong },
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
	{ "smh", tts::feature::semi_high },
	{ "sml", tts::feature::semi_low },
	{ "st1", tts::feature::primary_stress },
	{ "st2", tts::feature::secondary_stress },
	{ "stp", tts::feature::plosive },
	{ "syl", tts::feature::syllabic },
	{ "trl", tts::feature::trill },
	{ "umd", tts::feature::upper_mid },
	{ "unr", tts::feature::unrounded },
	{ "unx", tts::feature::unexploded },
	{ "uvl", tts::feature::uvular },
	{ "vcd", tts::feature::voiced },
	{ "vel", tts::feature::velar },
	{ "vls", tts::feature::voiceless },
	{ "vwl", tts::feature::vowel },
	{ "vzd", tts::feature::velarized },
};

static const std::pair<const char *, const char *> feature_data[] = {
	{ nullptr, nullptr },

	{ "vcd", i18n("voiced") },
	{ "vls", i18n("voiceless") },

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

	{ "vwl", i18n("vowel") },
	{ "dpr", i18n("rising diphthong") },
	{ "dpf", i18n("falling diphthong") },
	{ "lat", i18n("lateral") },

	{ "stp", i18n("plosive") },
	{ "frc", i18n("fricative") },
	{ "afr", i18n("affricate") },
	{ "nas", i18n("nasal") },
	{ "apr", i18n("approximant") },
	{ "trl", i18n("trill") },
	{ "flp", i18n("flap") },
	{ "clk", i18n("click") },
	{ "ejc", i18n("ejective") },
	{ "imp", i18n("implosive") },

	{ "hgh", i18n("high") },
	{ "smh", i18n("semi-high") },
	{ "umd", i18n("upper-mid") },
	{ "mid", i18n("mid") },
	{ "lmd", i18n("lower-mid") },
	{ "sml", i18n("semi-low") },
	{ "low", i18n("low") },

	{ "fnt", i18n("front") },
	{ "cnt", i18n("center") },
	{ "bck", i18n("back") },

	{ "unr", i18n("unrounded") },
	{ "rnd", i18n("rounded") },

	{ "st1", i18n("primary stress") },
	{ "st2", i18n("secondary stress") },

	{ "sbr", i18n("syllable break") },
	{ "pau", i18n("pause") },

	{ "est", i18n("extra short") },
	{ "hlg", i18n("half long") },
	{ "lng", i18n("long") },

	{ "asp", i18n("aspirated") },
	{ "unx", i18n("unexploded") },
	{ "syl", i18n("syllabic") },
	{ "mrm", i18n("murmured") },
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
