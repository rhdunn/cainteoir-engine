/* Explicit Feature Phoneme Set.
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

#include "phonemeset.hpp"
#include <utility>

namespace tts = cainteoir::tts;

static const std::initializer_list<const char *> voicing = {
	"vcd",
	"vls",
};

static const std::initializer_list<const char *> place_of_articulation = {
	"blb",
	"lbd",
	"dnt",
	"alv",
	"pla",
	"rfx",
	"alp",
	"pal",
	"lbp",
	"vel",
	"lbv",
	"uvl",
	"phr",
	"epg",
	"glt",
};

static const std::initializer_list<const char *> articulation_pre = {
	"lat",
	"sib",
};

static const std::initializer_list<const char *> manner_of_articulation = {
	"nas",
	"stp",
	"frc",
	"apr",
	"trl",
	"flp",
	"clk",
	"imp",
};

static const std::initializer_list<const char *> vowel_backness = {
	"fnt",
	"cnt",
	"bck",
};

static const std::initializer_list<const char *> vowel_height = {
	"hgh",
	"smh",
	"umd",
	"mid",
	"lmd",
	"sml",
	"low",
};

static const std::initializer_list<const char *> vowel_rounding = {
	"unr",
	"rnd",
};

static const std::initializer_list<const char *> phoneme_type = {
	"vwl",
	"ejc",
};

static const std::initializer_list<const char *> release = {
	"asp",
	"unx",
	"nzr",
	"ltr",
};

static const std::initializer_list<const char *> phonation = {
	"slv",
	"stv",
	"mrm",
	"crv",
};

static const std::initializer_list<const char *> articulation = {
	"dzd",
	"apc",
	"lmn",
	"lgl",
};

static const std::initializer_list<const char *> coarticulation = {
	"lzd",
	"pzd",
	"vzd",
	"fzd",
	"vfz",
	"nzd",
};

static const std::initializer_list<const char *> rhoticized = {
	"rzd",
};

static const std::initializer_list<const char *> rounding = {
	"mrd",
	"lrd",
};

static const std::initializer_list<const char *> positional = {
	"adv",
	"ret",
	"czd",
	"mcz",
	"rsd",
	"lwr",
};

static const std::initializer_list<const char *> tongue_root = {
	"atr",
	"rtr",
};

static const std::initializer_list<const char *> syllabicity = {
	"syl",
	"nsy",
};

static const std::initializer_list<const char *> stress = {
	"st1",
	"st2",
	"st3",
};

enum state_t
{
	begin_phoneme,
	in_phoneme,
	in_feature,
};

std::pair<bool, tts::phoneme> tts::read_explicit_feature(const char * &mCurrent, const char *mEnd)
{
	state_t s = begin_phoneme;
	char abbrev[4] = { '\0', '\0', '\0', '\0' };
	int abbrev_pos = 0;

	tts::phoneme p;
	while (mCurrent < mEnd)
	{
		switch (s)
		{
		case begin_phoneme:
			switch (*mCurrent)
			{
			case '{':
				s = in_phoneme;
				break;
			case '\0': case ' ': case '\t': case '\r': case '\n':
				break;
			default:
				throw tts::phoneme_error(i18n("unexpected start of phoneme (expecting '{')"));
			}
			break;
		case in_phoneme:
			switch (*mCurrent)
			{
			default:
				s = in_feature;
				abbrev_pos = 0;
				abbrev[abbrev_pos] = *mCurrent;
				break;
			case ',': case '}':
				throw tts::phoneme_error(i18n("missing phoneme feature"));
			}
			break;
		case in_feature:
			switch (*mCurrent)
			{
			default:
				++abbrev_pos;
				if (abbrev_pos <= 2)
					abbrev[abbrev_pos] = *mCurrent;
				break;
			case ',': case '}':
				if (abbrev_pos != 2)
					throw tts::phoneme_error(i18n("a phoneme feature must be 3 characters long"));

				p.set(abbrev);

				if (*mCurrent == ',')
					s = in_phoneme;
				else
				{
					++mCurrent;
					return { true, p };
				}
				break;
			}
			break;
		}
		++mCurrent;
	}

	if (s != begin_phoneme)
		throw tts::phoneme_error(i18n("unexpected end of phoneme (expecting '}')"));

	return { false, {}};
}

static void write_feature(FILE *output, const tts::phoneme &aPhoneme, const std::initializer_list<const char *> &features, bool &need_comma)
{
	for (auto &&abbreviation : features)
	{
		if (aPhoneme.get(abbreviation))
		{
			if (need_comma) fputc(',', output);
			fputs(abbreviation, output);
			need_comma = true;
			return;
		}
	}
}

void tts::write_explicit_feature(FILE *output, const tts::phoneme &aPhoneme)
{
	bool need_comma = false;

	fputc('{', output);
	if (aPhoneme.get(ipa::phoneme_type) == ipa::vowel)
	{
		write_feature(output, aPhoneme, vowel_height, need_comma);
		write_feature(output, aPhoneme, vowel_backness, need_comma);
		write_feature(output, aPhoneme, vowel_rounding, need_comma);
	}
	else
	{
		write_feature(output, aPhoneme, voicing, need_comma);
		write_feature(output, aPhoneme, place_of_articulation, need_comma);
		write_feature(output, aPhoneme, articulation_pre, need_comma);
		write_feature(output, aPhoneme, manner_of_articulation, need_comma);
	}
	write_feature(output, aPhoneme, phoneme_type, need_comma);
	write_feature(output, aPhoneme, release, need_comma);
	write_feature(output, aPhoneme, phonation, need_comma);
	write_feature(output, aPhoneme, articulation, need_comma);
	write_feature(output, aPhoneme, coarticulation, need_comma);
	write_feature(output, aPhoneme, positional, need_comma);
	write_feature(output, aPhoneme, rhoticized, need_comma);
	write_feature(output, aPhoneme, rounding, need_comma);
	write_feature(output, aPhoneme, tongue_root, need_comma);
	write_feature(output, aPhoneme, syllabicity, need_comma);
	write_feature(output, aPhoneme, stress, need_comma);
	fputc('}', output);
}

struct explicit_feature_reader : public tts::phoneme_reader
{
	void reset(const std::shared_ptr<cainteoir::buffer> &aBuffer);

	bool read();

	std::shared_ptr<cainteoir::buffer> mBuffer;
	const char *mCurrent;
	const char *mEnd;
};

void explicit_feature_reader::reset(const std::shared_ptr<cainteoir::buffer> &aBuffer)
{
	mBuffer = aBuffer;
	if (mBuffer.get())
	{
		mCurrent = mBuffer->begin();
		mEnd = mBuffer->end();
	}
	else
	{
		mCurrent = mEnd = nullptr;
	}
}

bool explicit_feature_reader::read()
{
	*(tts::phoneme *)this = tts::phoneme();
	auto ret = tts::read_explicit_feature(mCurrent, mEnd);
	*(tts::phoneme *)this = ret.second;
	return ret.first;
}

struct explicit_feature_writer : public tts::phoneme_writer
{
	explicit_feature_writer();

	void reset(FILE *aOutput);

	bool write(const tts::phoneme &aPhoneme);

	const char *name() const;

	FILE *output;
};

explicit_feature_writer::explicit_feature_writer()
	: output(nullptr)
{
}

void explicit_feature_writer::reset(FILE *aOutput)
{
	output = aOutput;
}

bool explicit_feature_writer::write(const tts::phoneme &aPhoneme)
{
	if (!output) return false;

	tts::write_explicit_feature(output, aPhoneme);
	return true;
}

const char *explicit_feature_writer::name() const
{
	return "features";
}

std::shared_ptr<tts::phoneme_reader> tts::createExplicitFeaturePhonemeReader()
{
	return std::make_shared<explicit_feature_reader>();
}

std::shared_ptr<tts::phoneme_writer> tts::createExplicitFeaturePhonemeWriter()
{
	return std::make_shared<explicit_feature_writer>();
}
