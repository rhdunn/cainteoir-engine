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
#include <cainteoir/trie.hpp>
#include <map>

namespace tts = cainteoir::tts;

struct ipa_reader : public tts::phoneme_reader
{
	ipa_reader(tts::phoneme_file_reader &aPhonemeSet);

	void reset(const std::shared_ptr<cainteoir::buffer> &aBuffer);

	bool read();
private:
	struct phoneme_t
	{
		tts::phoneme phoneme;

		phoneme_t(const tts::phoneme &aPhoneme = tts::phoneme(-1))
			: phoneme(aPhoneme)
		{
		}
	};

	cainteoir::trie<phoneme_t> mPhonemes;
	std::shared_ptr<cainteoir::buffer> mBuffer;
	const char *mCurrent;
	const char *mEnd;
};

ipa_reader::ipa_reader(tts::phoneme_file_reader &aPhonemeSet)
{
	while (aPhonemeSet.read())
	{
		if (aPhonemeSet.phonemes.size() != 1)
			throw std::runtime_error("ipa-style phonemesets only support mapping to one phoneme");

		mPhonemes.insert(*aPhonemeSet.transcription, { aPhonemeSet.phonemes.front() });
	}
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
	const auto *entry = mPhonemes.root();
	decltype(mPhonemes.root()) match = nullptr;
	const char *pos = mCurrent;
	while (mCurrent < mEnd)
	{
		const auto *next = entry->get(*mCurrent);
		if (next == nullptr)
		{
			if (match)
			{
				mCurrent = pos;
				*(tts::phoneme *)this = match->item.phoneme;
				return true;
			}

			uint8_t c = *mCurrent;
			++mCurrent;

			char msg[64];
			if (c <= 0x20 || c >= 0x80)
				sprintf(msg, i18n("unrecognised character 0x%02X"), c);
			else
				sprintf(msg, i18n("unrecognised character %c"), c);
			throw tts::phoneme_error(msg);
		}
		else
		{
			entry = next;
			++mCurrent;
			if (entry->item.phoneme != tts::phoneme(-1))
			{
				match = entry;
				pos = mCurrent;
			}
		}
	}
	if (match)
	{
		mCurrent = pos;
		*(tts::phoneme *)this = match->item.phoneme;
		return true;
	}
	return false;
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

	std::map<tts::phoneme, std::shared_ptr<cainteoir::buffer>> mPhonemes;
};

ipa_writer::ipa_writer(tts::phoneme_file_reader &aPhonemeSet, const char *aName)
	: mPhonemeSet(aName)
	, mOutput(nullptr)
{
	while (aPhonemeSet.read())
	{
		if (aPhonemeSet.phonemes.size() != 1)
			throw std::runtime_error("ipa-style phonemesets only support mapping to one phoneme");

		mPhonemes[aPhonemeSet.phonemes.front()] = aPhonemeSet.transcription;
	}
}

void ipa_writer::reset(FILE *aOutput)
{
	mOutput = aOutput;
}

bool ipa_writer::write(const tts::phoneme &aPhoneme)
{
	auto match = mPhonemes.find(aPhoneme);
	if (match != mPhonemes.end())
	{
		fwrite(match->second->begin(), 1, match->second->size(), mOutput);
		return true;
	}
	return false;
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