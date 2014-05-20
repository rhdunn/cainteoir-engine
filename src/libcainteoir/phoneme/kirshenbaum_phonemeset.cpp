/* Kirshenbaum-Based Phoneme Set Reader/Writer.
 *
 * Copyright (C) 2014 Reece H. Dunn
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

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;

struct kirshenbaum_reader : public tts::phoneme_parser
{
	kirshenbaum_reader(tts::phoneme_file_reader &aPhonemeSet);

	bool parse(const char * &mCurrent, const char *mEnd, ipa::phoneme &aPhoneme);
private:
	tts::transcription_reader mPhonemes;
};

kirshenbaum_reader::kirshenbaum_reader(tts::phoneme_file_reader &aPhonemeSet)
	: mPhonemes(aPhonemeSet)
{
}

bool kirshenbaum_reader::parse(const char * &mCurrent, const char *mEnd, ipa::phoneme &aPhoneme)
{
	if (*mCurrent == '{')
	{
		auto ret = tts::read_explicit_feature(mCurrent, mEnd);
		aPhoneme = ret.second;
		return ret.first;
	}

	auto ret = mPhonemes.read(mCurrent, mEnd);
	if (ret.first)
		aPhoneme = ret.second;
	return ret.first;
}

struct kirshenbaum_writer : public tts::phoneme_writer
{
	kirshenbaum_writer(tts::phoneme_file_reader &aPhonemeSet, const char *aName);

	void reset(FILE *aOutput);

	bool write(const ipa::phoneme &aPhoneme);

	const char *name() const;
private:
	const char *mPhonemeSet;
	FILE *mOutput;

	tts::transcription_writer mPhonemes;
};

kirshenbaum_writer::kirshenbaum_writer(tts::phoneme_file_reader &aPhonemeSet, const char *aName)
	: mPhonemeSet(aName)
	, mOutput(nullptr)
	, mPhonemes(aPhonemeSet)
{
}

void kirshenbaum_writer::reset(FILE *aOutput)
{
	mOutput = aOutput;
}

bool kirshenbaum_writer::write(const ipa::phoneme &aPhoneme)
{
	bool ret = mPhonemes.write(mOutput, aPhoneme);
	if (!ret)
		tts::write_explicit_feature(mOutput, aPhoneme);
	return true;
}

const char *kirshenbaum_writer::name() const
{
	return mPhonemeSet;
}

std::shared_ptr<tts::phoneme_reader> tts::createKirshenbaumPhonemeReader(phoneme_file_reader &aPhonemeSet, const char *aName)
{
	return std::make_shared<phonemeset_reader<kirshenbaum_reader>>(aPhonemeSet);
}

std::shared_ptr<tts::phoneme_writer> tts::createKirshenbaumPhonemeWriter(phoneme_file_reader &aPhonemeSet, const char *aName)
{
	return std::make_shared<kirshenbaum_writer>(aPhonemeSet, aName);
}
