/* Arpabet-Based Phoneme Set Reader/Writer.
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

enum class type : uint8_t
{
	unknown,
	space,
	newline,
	stress,
	phoneme,
};

#define _ type::unknown
#define s type::space
#define n type::newline
#define S type::stress
#define P type::phoneme

static const type classification[256] = {
	//////// x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF
	/* 0x */ _, _, _, _, _, _, _, _, _, s, n, _, _, s, _, _,
	/* 1x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 2x */ s, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 3x */ S, S, S, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 4x */ _, P, P, P, P, P, P, P, P, P, P, P, P, P, P, P,
	/* 5x */ P, P, P, P, P, P, P, P, P, P, P, _, _, _, _, _,
	/* 6x */ _, P, P, P, P, P, P, P, P, P, P, P, P, P, P, P,
	/* 7x */ P, P, P, P, P, P, P, P, P, P, P, _, _, _, _, _,
	/* 8x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 9x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Ax */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Bx */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Cx */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Dx */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Ex */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Fx */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	//////// x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF
};

#undef _
#undef s
#undef n
#undef S
#undef P

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;

struct arpabet_reader : public tts::phoneme_parser
{
	arpabet_reader(tts::phoneme_file_reader &aPhonemeSet, tts::arpabet_variant aVariant);

	bool parse(const char * &mCurrent, const char *mEnd, ipa::phoneme &aPhoneme);
private:
	cainteoir::trie<std::pair<ipa::phoneme, ipa::phoneme>> mPhonemes;

	enum class state : uint8_t
	{
		need_phoneme,
		parsing_phoneme,
		error,
		emitting_phoneme1,
		emitting_phoneme2,
	};

	decltype(mPhonemes.root()) mPosition;
	state mState;
	char mStress;
	tts::arpabet_variant mVariant;
};

arpabet_reader::arpabet_reader(tts::phoneme_file_reader &aPhonemeSet, tts::arpabet_variant aVariant)
	: mState(state::need_phoneme)
	, mStress(0)
	, mVariant(aVariant)
{
	while (aPhonemeSet.read()) switch (aPhonemeSet.type)
	{
	case tts::placement::primary:
		switch (aPhonemeSet.phonemes.size())
		{
		case 1:
			mPhonemes.insert(*aPhonemeSet.transcription,
			                { aPhonemeSet.phonemes[0], ipa::phoneme(-1) });
			break;
		case 2:
			mPhonemes.insert(*aPhonemeSet.transcription,
			                { aPhonemeSet.phonemes[0], aPhonemeSet.phonemes[1] });
			break;
		default:
			throw std::runtime_error("arpabet-based phonemesets only support 1 or 2 phonemes per definition");
		}
		break;
	default:
		throw std::runtime_error("arpabet-based phonemesets only support primary transcriptions in phoneme definition files");
	}

	mPosition = mPhonemes.root();
}

bool arpabet_reader::parse(const char * &mCurrent, const char *mEnd, ipa::phoneme &aPhoneme)
{
	while (true) switch (mState)
	{
	case state::need_phoneme:
		if (mCurrent == mEnd)
			return false;
		switch (classification[*mCurrent])
		{
		case type::space:
			++mCurrent;
			break;
		case type::newline:
			aPhoneme = ipa::phoneme(ipa::pause);
			++mCurrent;
			return true;
		case type::stress:
			throw tts::phoneme_error("stress found before phoneme");
		case type::phoneme:
			mState = state::parsing_phoneme;
			break;
		default:
			throw tts::phoneme_error("unknown character in phoneme stream");
		}
		break;
	case state::parsing_phoneme:
		if (mCurrent == mEnd)
			mState = state::emitting_phoneme1;
		else switch (classification[*mCurrent])
		{
		case type::space:
		case type::newline:
			mState = state::emitting_phoneme1;
			break;
		case type::stress:
			mStress = *mCurrent++;
			break;
		case type::phoneme:
			mPosition = mPosition->get(*mCurrent);
			if (mPosition == nullptr)
			{
				mState = state::error;
				throw tts::phoneme_error("unknown phoneme");
			}
			++mCurrent;
			break;
		default:
			throw tts::phoneme_error("unknown character in phoneme stream");
		}
		break;
	case state::error:
		if (mCurrent == mEnd)
			return false;
		switch (classification[*mCurrent])
		{
		case type::space:
		case type::newline:
			mPosition = mPhonemes.root();
			mStress = 0;
			mState = state::need_phoneme;
			break;
		default:
			++mCurrent;
			break;
		}
		break;
	case state::emitting_phoneme1:
		aPhoneme = mPosition->item.first;
		switch (mStress)
		{
		case '1': aPhoneme.set("st1"); break;
		case '2': aPhoneme.set("st2"); break;
		}
		mStress = 0;
		mState = state::emitting_phoneme2;
		return true;
	case state::emitting_phoneme2:
		aPhoneme = mPosition->item.second;
		mPosition = mPhonemes.root();
		mState = state::need_phoneme;
		if (aPhoneme.get(-1) != -1)
			return true;
		break;
	}
	return false;
}

struct arpabet_writer : public tts::phoneme_writer
{
	arpabet_writer(tts::phoneme_file_reader &aPhonemeSet, const char *aName, tts::arpabet_variant aVariant);

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
		have_pause,
		output_phoneme,
	};

	decltype(mPhonemes.root()) mPosition;
	state mState;
	ipa::phoneme::value_type mStress;
	bool mNeedSpace;
	tts::arpabet_variant mVariant;
};

arpabet_writer::arpabet_writer(tts::phoneme_file_reader &aPhonemeSet, const char *aName, tts::arpabet_variant aVariant)
	: mPhonemeSet(aName)
	, mOutput(nullptr)
	, mState(state::need_phoneme)
	, mStress(-1)
	, mNeedSpace(false)
	, mVariant(aVariant)
{
	while (aPhonemeSet.read()) switch (aPhonemeSet.type)
	{
	case tts::placement::primary:
		switch (aPhonemeSet.phonemes.size())
		{
		case 1:
			{
				ipa::phoneme index[] = { aPhonemeSet.phonemes[0] };
				mPhonemes.insert(index, aPhonemeSet.transcription);
			}
			break;
		case 2:
			{
				ipa::phoneme index[] = { aPhonemeSet.phonemes[0], aPhonemeSet.phonemes[1] };
				mPhonemes.insert(index, aPhonemeSet.transcription);
			}
			break;
		default:
			throw std::runtime_error("arpabet-based phonemesets only support 1 or 2 phonemes per definition");
		}
		break;
	default:
		throw std::runtime_error("arpabet-based phonemesets only support primary transcriptions in phoneme definition files");
	}

	mPosition = mPhonemes.root();
}

void arpabet_writer::reset(FILE *aOutput)
{
	mOutput = aOutput;
	mNeedSpace = false;
}

bool arpabet_writer::write(const ipa::phoneme &aPhoneme)
{
	while (true) switch (mState)
	{
	case state::need_phoneme:
		if (aPhoneme == ipa::phoneme(ipa::pause))
			mState = state::have_pause;
		else
		{
			if (aPhoneme.get(ipa::phoneme_type) == ipa::vowel)
				mStress = aPhoneme.get(ipa::stress);
			else
				mStress = -1;

			ipa::phoneme base { aPhoneme.get(~ipa::stress) };
			mPosition = mPhonemes.root()->get(base);
			if (mPosition != nullptr)
				mState = state::have_phoneme;
		}
		return true;
	case state::have_phoneme:
		{
			auto next = mPosition->get(aPhoneme);
			mState = state::output_phoneme;
			if (next != nullptr)
			{
				mPosition = next;
				return true;
			}
		}
		break;
	case state::have_pause:
		fputc('\n', mOutput);
		mNeedSpace = false;
		mState = state::need_phoneme;
		break;
	case state::output_phoneme:
		output_phoneme();
		break;
	}
}

void arpabet_writer::flush()
{
	if (mState == state::have_phoneme)
		output_phoneme();
}

const char *arpabet_writer::name() const
{
	return mPhonemeSet;
}

void arpabet_writer::output_phoneme()
{
	if (mNeedSpace)
		fputc(' ', mOutput);
	mNeedSpace = true;
	fwrite(mPosition->item->begin(), 1, mPosition->item->size(), mOutput);
	switch (mStress)
	{
	case ipa::unstressed:       if (mVariant == tts::arpabet_variant::arpabet) fputc('0', mOutput); break;
	case ipa::primary_stress:   fputc('1', mOutput); break;
	case ipa::secondary_stress: fputc('2', mOutput); break;
	}
	mState = state::need_phoneme;
}

std::shared_ptr<tts::phoneme_parser> tts::createArpabetPhonemeParser(phoneme_file_reader &aPhonemeSet, const char *aName, tts::arpabet_variant aVariant)
{
	return std::make_shared<arpabet_reader>(aPhonemeSet, aVariant);
}

std::shared_ptr<tts::phoneme_reader> tts::createArpabetPhonemeReader(phoneme_file_reader &aPhonemeSet, const char *aName, tts::arpabet_variant aVariant)
{
	return std::make_shared<phonemeset_reader<arpabet_reader>>(aPhonemeSet, aVariant);
}

std::shared_ptr<tts::phoneme_writer> tts::createArpabetPhonemeWriter(phoneme_file_reader &aPhonemeSet, const char *aName, tts::arpabet_variant aVariant)
{
	return std::make_shared<arpabet_writer>(aPhonemeSet, aName, aVariant);
}
