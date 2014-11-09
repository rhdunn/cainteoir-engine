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

struct ruleset : public tts::phoneme_reader
{
	ruleset(const std::shared_ptr<cainteoir::buffer> &aData);

	void reset(const std::shared_ptr<cainteoir::buffer> &aBuffer);

	bool read();
private:
	std::shared_ptr<cainteoir::buffer> mBuffer;
	const uint8_t *mStart;
	const uint8_t *mCurrent;
	const uint8_t *mEnd;
	const char *mPhonemeCurrent;
	const char *mPhonemeEnd;

	std::shared_ptr<cainteoir::buffer> mData;
	std::shared_ptr<tts::phoneme_parser> mPhonemeSet;
	cainteoir::native_endian_buffer mRules;
	uint16_t mRuleGroups[256];

	std::pair<const uint8_t *, const char *>
	next_match();
};

ruleset::ruleset(const std::shared_ptr<cainteoir::buffer> &aData)
	: mData(aData)
	, mRules((const uint8_t *)aData->begin(), (const uint8_t *)aData->end())
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
	mPhonemeCurrent = mPhonemeEnd = nullptr;
}

bool ruleset::read()
{
	while (!mPhonemeSet->parse(mPhonemeCurrent, mPhonemeEnd, *this))
	{
		auto match = next_match();
		if (!match.first)
			return false;

		mCurrent        = match.first;
		mPhonemeCurrent = match.second;
		mPhonemeEnd     = match.second + strlen(match.second);
	}
	return true;
}

std::pair<const uint8_t *, const char *>
ruleset::next_match()
{
	static const uint8_t null_rule[] = { 0 };

	enum state_t
	{
		need_phonemes,
		in_rule_group,
		context_match,
		left_match,
		right_match,
	};

	state_t state = need_phonemes;
	const uint8_t *rule     = null_rule;
	const uint8_t *context  = nullptr;
	const uint8_t *left     = nullptr;
	const uint8_t *right    = nullptr;
	const char    *phonemes = nullptr;
	uint16_t       offset   = 0;
	while (true) switch (*rule)
	{
	case 0:
		switch (state)
		{
		case need_phonemes:
			if (mCurrent == mEnd) return { nullptr, nullptr };

			offset = mRuleGroups[*mCurrent];
			if (offset == 0)
			{
				++mCurrent;
				throw tts::phoneme_error(i18n("unable to pronounce the text"));
			}

			mRules.seek(offset);
			state = in_rule_group;
			break;
		case in_rule_group:
			rule = (const uint8_t *)mRules.pstr();
			if (*rule == 0)
			{
				++mCurrent;
				throw tts::phoneme_error(i18n("unable to pronounce the text"));
			}

			phonemes = mRules.pstr();
			context = left = right = mCurrent;
			state = context_match;
			break;
		default:
			return { context, phonemes };
		}
		break;
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
			if (left != mStart - 1)
			{
				state = in_rule_group;
				rule = null_rule;
			}
			else
				++rule;
			break;
		case right_match:
			if (right != mEnd)
			{
				state = in_rule_group;
				rule = null_rule;
			}
			else
				++rule;
			break;
		default:
			state = in_rule_group;
			rule = null_rule;
			break;
		}
		break;
	default:
		switch (state)
		{
		case context_match:
			if (context <= mEnd && *context == *rule)
			{
				++rule;
				++context;
			}
			else
			{
				state = in_rule_group;
				rule = null_rule;
			}
			break;
		case left_match:
			if (left >= mStart && *left == *rule)
			{
				++rule;
				--left;
			}
			else
			{
				state = in_rule_group;
				rule = null_rule;
			}
			break;
		case right_match:
			if (right <= mEnd && *right == *rule)
			{
				++rule;
				++right;
			}
			else
			{
				state = in_rule_group;
				rule = null_rule;
			}
			break;
		}
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
