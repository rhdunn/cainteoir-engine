/* Letter-to-Phoneme Rule Processor.
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

#include <cainteoir/language.hpp>
#include "../synthesizer/synth.hpp"

namespace tts = cainteoir::tts;

static const uint8_t *
match_l2p_rule(const uint8_t *rule,
               const uint8_t *start,
               const uint8_t *current,
               const uint8_t *end)
{
	enum state_t
	{
		context_match,
		left_match,
		right_match,
	};

	state_t state = context_match;
	const uint8_t *context = current;
	const uint8_t *left = current;
	const uint8_t *right = current;
	while (true) switch (*rule)
	{
	case 0:
		return context;
	case '(':
		right = context;
		state = right_match;
		++rule;
		break;
	case ')':
		state = left_match;
		++rule;
		--left;
		break;
	case '_':
		switch (state)
		{
		case left_match:
			if (left != start - 1) return nullptr;
			++rule;
			break;
		case right_match:
			if (right != end) return nullptr;
			++rule;
			break;
		default:
			return nullptr;
		}
		break;
	default:
		switch (state)
		{
		case context_match:
			if (context > end) return nullptr;
			if (*context == *rule)
			{
				++rule;
				++context;
			}
			else
				return nullptr;
			break;
		case left_match:
			if (left < start) return nullptr;
			if (*left == *rule)
			{
				++rule;
				--left;
			}
			else
				return nullptr;
			break;
		case right_match:
			if (right > end) return nullptr;
			if (*right == *rule)
			{
				++rule;
				++right;
			}
			else
				return nullptr;
			break;
		}
		break;
	}
}

struct ruleset : public tts::phoneme_reader
{
	ruleset(const std::shared_ptr<cainteoir::buffer> &aData);

	void reset(const std::shared_ptr<cainteoir::buffer> &aBuffer);

	bool read();
private:
	enum state_t
	{
		need_phonemes,
		in_rule_group,
		have_phonemes,
	};

	std::shared_ptr<cainteoir::buffer> mBuffer;
	const uint8_t *mStart;
	const uint8_t *mCurrent;
	const uint8_t *mEnd;
	const char *mPhonemeCurrent;
	const char *mPhonemeEnd;
	state_t mState;

	std::shared_ptr<cainteoir::buffer> mData;
	std::shared_ptr<tts::phoneme_parser> mPhonemeSet;
	cainteoir::native_endian_buffer mRules;
	uint16_t mRuleGroups[256];
};

ruleset::ruleset(const std::shared_ptr<cainteoir::buffer> &aData)
	: mData(aData)
	, mRules((const uint8_t *)aData->begin(), (const uint8_t *)aData->end())
	, mState(need_phonemes)
{
	memset(mRuleGroups, 0, sizeof(mRuleGroups));

	mRules.seek(tts::LANGDB_HEADER_ID);
	const char *locale = mRules.pstr();
	const char *phonemeset = mRules.pstr();

	mPhonemeSet = tts::createPhonemeParser(phonemeset);

	while (!mRules.eof()) switch (mRules.magic())
	{
	case tts::STRING_TABLE_MAGIC:
		mRules.seek(mRules.u32());
		break;
	case tts::LETTER_TO_PHONEME_TABLE_MAGIC:
		{
			uint16_t entries = mRules.u16();
			uint8_t  id = mRules.u8();
			uint16_t offset = mRules.offset();
			mRules.seek(offset + (entries * tts::LETTER_TO_PHONEME_TABLE_ENTRY_SIZE));
			mRuleGroups[id] = offset;
		}
		break;
	case tts::DICTIONARY_TABLE_MAGIC:
		{
			uint16_t entries = mRules.u16();
			uint16_t offset = mRules.offset();
			mRules.seek(offset + (entries * tts::DICTIONARY_TABLE_ENTRY_SIZE));
		}
		break;
	default:
		throw std::runtime_error("unsupported section in the language file");
	}
}

void ruleset::reset(const std::shared_ptr<cainteoir::buffer> &aBuffer)
{
	mBuffer = aBuffer;
	if (mBuffer.get())
	{
		mStart = mCurrent = (const uint8_t *)mBuffer->begin();
		mEnd = (const uint8_t *)mBuffer->end();
	}
	else
		mStart = mCurrent = mEnd = nullptr;
	mState = need_phonemes;
}

bool ruleset::read()
{
	const uint8_t *rule = nullptr;
	const uint8_t *next = nullptr;
	while (true) switch (mState)
	{
	case need_phonemes:
		{
			if (mCurrent == mEnd) return false;

			uint16_t offset = mRuleGroups[*mCurrent];
			if (offset == 0)
			{
				++mCurrent;
				throw tts::phoneme_error(i18n("unable to pronounce the text"));
			}

			mRules.seek(offset);
			mState = in_rule_group;
		}
		break;
	case in_rule_group:
		rule = (const uint8_t *)mRules.pstr();
		if (*rule == 0)
		{
			++mCurrent;
			throw tts::phoneme_error(i18n("unable to pronounce the text"));
		}

		next = match_l2p_rule(rule, mStart, mCurrent, mEnd);
		if (next != nullptr)
		{
			mCurrent = next;
			mPhonemeCurrent = mRules.pstr();
			mPhonemeEnd = mPhonemeCurrent + strlen(mPhonemeCurrent);
			mState = have_phonemes;
			continue;
		}
		break;
	case have_phonemes:
		if (mPhonemeSet->parse(mPhonemeCurrent, mPhonemeEnd, *this))
			return true;
		mState = need_phonemes;
		break;
	}
}

std::shared_ptr<tts::phoneme_reader> tts::createPronunciationRules(const char *aRuleSetPath)
{
	if (!aRuleSetPath) return {};

	auto data = cainteoir::make_file_buffer(aRuleSetPath);
	if (!data) return {};

	const char *header = data->begin();

	if (strncmp(header, "LANGDB", 6) != 0 || *(const uint16_t *)(header + 6) != 0x3031)
		return {};

	return std::make_shared<ruleset>(data);
}
