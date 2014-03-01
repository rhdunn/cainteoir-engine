/* IPA-Based Phoneme Set Reader/Writer.
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

namespace tts = cainteoir::tts;

struct ipa_reader : public tts::phoneme_reader
{
	ipa_reader(tts::phoneme_file_reader &aPhonemeSet);

	void reset(const std::shared_ptr<cainteoir::buffer> &aBuffer);

	bool read();
private:
	tts::transcription_reader mPhonemes;
	std::shared_ptr<cainteoir::buffer> mBuffer;
	const char *mCurrent;
	const char *mEnd;
};

ipa_reader::ipa_reader(tts::phoneme_file_reader &aPhonemeSet)
	: mPhonemes(aPhonemeSet)
{
}

void ipa_reader::reset(const std::shared_ptr<cainteoir::buffer> &aBuffer)
{
	mBuffer = aBuffer;
	if (mBuffer.get())
	{
		mCurrent = mBuffer->begin();
		mEnd = mBuffer->end();
	}
	else
		mCurrent = mEnd = nullptr;
}

bool ipa_reader::read()
{
	auto ret = mPhonemes.read(mCurrent, mEnd);
	if (ret.first)
		*(tts::phoneme *)this = ret.second;
	return ret.first;
}

struct ipa_writer : public tts::phoneme_writer
{
	ipa_writer(tts::phoneme_file_reader &aPhonemeSet, const char *aName);

	void reset(FILE *aOutput);

	bool write(const tts::phoneme &aPhoneme);

	const char *name() const;
private:
	const char *mPhonemeSet;
	FILE *mOutput;

	tts::transcription_writer mPhonemes;
};

ipa_writer::ipa_writer(tts::phoneme_file_reader &aPhonemeSet, const char *aName)
	: mPhonemes(aPhonemeSet)
	, mOutput(nullptr)
{
}

void ipa_writer::reset(FILE *aOutput)
{
	mOutput = aOutput;
}

bool ipa_writer::write(const tts::phoneme &aPhoneme)
{
	return mPhonemes.write(mOutput, aPhoneme);
}

const char *ipa_writer::name() const
{
	return mPhonemeSet;
}

std::shared_ptr<tts::phoneme_reader> tts::createIpaPhonemeReader(phoneme_file_reader &aPhonemeSet, const char *aName)
{
	return std::make_shared<ipa_reader>(aPhonemeSet);
}

std::shared_ptr<tts::phoneme_writer> tts::createIpaPhonemeWriter(phoneme_file_reader &aPhonemeSet, const char *aName)
{
	return std::make_shared<ipa_writer>(aPhonemeSet, aName);
}
