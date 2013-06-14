/* Explicit Feature Phoneme Set.
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

namespace tts = cainteoir::tts;

struct explicit_feature_reader : public tts::phoneme_reader
{
	void reset(const std::shared_ptr<cainteoir::buffer> &aBuffer);

	bool read();

	enum state_t
	{
		begin_phoneme,
		in_phoneme,
		in_feature,
	};

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
	state_t s = begin_phoneme;
	char abbrev[4] = { '\0', '\0', '\0', '\0' };
	int abbrev_pos = 0;
	tts::feature features[8] = {
		tts::feature::unspecified,
		tts::feature::unspecified,
		tts::feature::unspecified,
		tts::feature::unspecified,
		tts::feature::unspecified,
		tts::feature::unspecified,
		tts::feature::unspecified,
		tts::feature::unspecified,
	};
	int feature_pos = 0;

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

				if (feature_pos < 8)
				{
					auto match = tts::get_feature_id(abbrev);
					if (!match.first)
					{
						if (abbrev[0] == 'l' && abbrev[1] == 'b' && abbrev[2] == 'v')
						{
							// Special handling for 'lbv' ...
							features[feature_pos++] = tts::feature::velar;
							features[feature_pos++] = tts::feature::labialized;
						}
						else
						{
							char msg[64];
							sprintf(msg, i18n("unknown phoneme feature '%s'"), abbrev);
							throw tts::phoneme_error(msg);
						}
					}
					if (match.second != tts::feature::unspecified)
						features[feature_pos++] = match.second;
				}
				else
					throw tts::phoneme_error(i18n("a phoneme must specify no more than 8 features"));

				if (*mCurrent == ',')
					s = in_phoneme;
				else
				{
					*(tts::phoneme *)this = {
						features[0], features[1], features[2], features[3],
						features[4], features[5], features[6], features[7] };
					++mCurrent;
					return true;
				}
				break;
			}
			break;
		}
		++mCurrent;
	}

	if (s != begin_phoneme)
		throw tts::phoneme_error(i18n("unexpected end of phoneme (expecting '}')"));

	*(tts::phoneme *)this = {
		features[0], features[1], features[2], features[3],
		features[4], features[5], features[6], features[7] };
	return false;
}

struct explicit_feature_writer : public tts::phoneme_writer
{
	explicit_feature_writer();

	void reset(FILE *aOutput);

	bool write(const tts::phoneme &aPhoneme);

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

	bool need_comma = false;

	fputc('{', output);
	for (auto f : aPhoneme)
	{
		const char *abbreviation = tts::get_feature_abbreviation(f);
		if (abbreviation)
		{
			if (need_comma) fputc(',', output);
			fputs(abbreviation, output);
			need_comma = true;
		}
	}
	fputc('}', output);

	return true;
}

std::shared_ptr<tts::phoneme_reader> tts::createExplicitFeaturePhonemeReader()
{
	return std::make_shared<explicit_feature_reader>();
}

std::shared_ptr<tts::phoneme_writer> tts::createExplicitFeaturePhonemeWriter()
{
	return std::make_shared<explicit_feature_writer>();
}
