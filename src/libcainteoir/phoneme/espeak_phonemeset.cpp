/* eSpeak-Based Phoneme Set Reader/Writer.
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

struct espeak_reader : public tts::phoneme_reader
{
	espeak_reader(tts::phoneme_file_reader &aPhonemeSet, const char *aName);

	void reset(const std::shared_ptr<cainteoir::buffer> &aBuffer);

	bool read();
private:
	std::shared_ptr<cainteoir::buffer> mBuffer;
	const char *mCurrent;
	const char *mEnd;

	struct phoneme_t
	{
		bool match;
		std::vector<ipa::phoneme> phonemes;

		phoneme_t() : match(false) {}

		phoneme_t(const std::vector<ipa::phoneme> &aPhonemes)
			: match(true)
			, phonemes(aPhonemes)
		{
		}
	};

	cainteoir::trie<phoneme_t> mPhonemes;

	enum class state : uint8_t
	{
		need_phoneme,
		parsing_phoneme,
		emitting_phoneme,
	};

	decltype(mPhonemes.root()) mPosition;
	state mState;
	char mStress;
	int mIndex;
};

espeak_reader::espeak_reader(tts::phoneme_file_reader &aPhonemeSet, const char *aName)
	: mState(state::need_phoneme)
	, mStress(0)
	, mIndex(0)
{
	while (aPhonemeSet.read()) switch (aPhonemeSet.type)
	{
	case tts::placement::primary:
		mPhonemes.insert(*aPhonemeSet.transcription, aPhonemeSet.phonemes);
		break;
	default:
		throw std::runtime_error("espeak-based phonemesets only support primary transcriptions in phoneme definition files");
	}

	mPosition = mPhonemes.root();
}

void espeak_reader::reset(const std::shared_ptr<cainteoir::buffer> &aBuffer)
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

bool espeak_reader::read()
{
	decltype(mPhonemes.root()) match = nullptr;
	while (true) switch (mState)
	{
	case state::need_phoneme:
		if (mCurrent == mEnd)
			return false;
		switch (*mCurrent)
		{
		case ' ':
			*(ipa::phoneme *)this = ipa::phoneme(ipa::pause | ipa::extra_short);
			++mCurrent;
			return true;
		case '\n':
			*(ipa::phoneme *)this = ipa::phoneme(ipa::pause);
			++mCurrent;
			return true;
		case '\'': case ',': // stress
			mStress = *mCurrent++;
			break;
		default:
			mState = state::parsing_phoneme;
			break;
		}
		break;
	case state::parsing_phoneme:
		if (mCurrent == mEnd)
			mState = state::emitting_phoneme;
		else switch (*mCurrent)
		{
		case ' ':
		case '\n':
		case '\'': case ',': // stress
			mState = state::emitting_phoneme;
			break;
		default:
			{
				auto next = mPosition->get(*mCurrent);
				if (next == nullptr)
				{
					if (match == nullptr)
					{
						++mCurrent;
						throw tts::phoneme_error("unknown phoneme");
					}
					else
						mState = state::emitting_phoneme;
				}
				else
				{
					mPosition = next;
					++mCurrent;
					if (mPosition->item.match)
						match = mPosition;
				}
			}
			break;
		}
		break;
	case state::emitting_phoneme:
		if (mIndex == mPosition->item.phonemes.size())
		{
			mStress = 0;
			mIndex = 0;
			mPosition = mPhonemes.root();
			mState = state::need_phoneme;
		}
		else
		{
			*(ipa::phoneme *)this = mPosition->item.phonemes[mIndex];
			if (mIndex == 0) switch (mStress)
			{
			case '\'': set("st1"); break;
			case ',':  set("st2"); break;
			}
			++mIndex;
			return true;
		}
		break;
	}
	return false;
}

struct espeak_writer : public tts::phoneme_writer
{
	espeak_writer(tts::phoneme_file_reader &aPhonemeSet, const char *aName);

	void reset(FILE *aOutput);

	bool write(const ipa::phoneme &aPhoneme);

	void flush();

	const char *name() const;
private:
	void output_phoneme();

	const char *mPhonemeSet;
	FILE *mOutput;

	cainteoir::trie<std::shared_ptr<cainteoir::buffer>, ipa::phoneme> mPhonemes;

	enum class state : uint8_t
	{
		need_phoneme,
		have_phoneme,
		output_phoneme,
	};

	decltype(mPhonemes.root()) mPosition;
	state mState;
	ipa::phoneme::value_type mStress;
};

espeak_writer::espeak_writer(tts::phoneme_file_reader &aPhonemeSet, const char *aName)
	: mPhonemeSet(aName)
	, mOutput(nullptr)
	, mState(state::need_phoneme)
	, mStress(-1)
{
	while (aPhonemeSet.read()) switch (aPhonemeSet.type)
	{
	case tts::placement::primary:
		if (!aPhonemeSet.phonemes.empty())
			mPhonemes.insert(aPhonemeSet.phonemes, aPhonemeSet.transcription);
		break;
	default:
		throw std::runtime_error("espeak-based phonemesets only support primary transcriptions in phoneme definition files");
	}

	mPosition = mPhonemes.root();
}

void espeak_writer::reset(FILE *aOutput)
{
	mOutput = aOutput;
}

bool espeak_writer::write(const ipa::phoneme &aPhoneme)
{
	while (true) switch (mState)
	{
	case state::need_phoneme:
		if (aPhoneme == ipa::phoneme(ipa::pause))
			fputc('\n', mOutput);
		else if (aPhoneme == ipa::phoneme(ipa::pause | ipa::extra_short))
			fputc(' ', mOutput);
		else
		{
			mStress = aPhoneme.get(ipa::stress);

			ipa::phoneme base { aPhoneme.get(~ipa::stress) };
			mPosition = mPhonemes.root()->get(base);
			if (mPosition != nullptr)
				mState = state::have_phoneme;
		}
		return true;
	case state::have_phoneme:
		{
			auto next = mPosition->get(aPhoneme);
			if (next == nullptr)
				mState = state::output_phoneme;
			else
			{
				mPosition = next;
				return true;
			}
		}
		break;
	case state::output_phoneme:
		output_phoneme();
		break;
	}
}

void espeak_writer::flush()
{
	if (mState == state::have_phoneme)
		output_phoneme();
}

const char *espeak_writer::name() const
{
	return mPhonemeSet;
}

void espeak_writer::output_phoneme()
{
	mState = state::need_phoneme;
	if (!mPosition->item) return;
	switch (mStress)
	{
	case ipa::unstressed:       break;
	case ipa::primary_stress:   fputc('\'', mOutput); break;
	case ipa::secondary_stress: fputc(',',  mOutput); break;
	}
	fwrite(mPosition->item->begin(), 1, mPosition->item->size(), mOutput);
}

std::shared_ptr<tts::phoneme_reader> tts::createEspeakPhonemeReader(phoneme_file_reader &aPhonemeSet, const char *aName)
{
	return std::make_shared<espeak_reader>(aPhonemeSet, aName);
}

std::shared_ptr<tts::phoneme_writer> tts::createEspeakPhonemeWriter(phoneme_file_reader &aPhonemeSet, const char *aName)
{
	return std::make_shared<espeak_writer>(aPhonemeSet, aName);
}
