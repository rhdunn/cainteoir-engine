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
namespace ipa = cainteoir::ipa;

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

	bool match_phoneme(const char *phonemes, const uint8_t *&rules);

	std::pair<const uint8_t *, const char *>
	next_match(const uint8_t *current);
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
		auto match = next_match(mCurrent);
		if (!match.first)
			return false;

		mCurrent        = match.first;
		mPhonemeCurrent = match.second;
		mPhonemeEnd     = match.second + strlen(match.second);
	}
	return true;
}

bool ruleset::match_phoneme(const char *phonemes, const uint8_t *&rules)
{
	const char *phonemes_end = phonemes + strlen(phonemes);
	ipa::phoneme p;
	char feature[4] = { 0, 0, 0, 0 };
	uint8_t feature_pos = 0xFF;
	while (true) switch (*rules)
	{
	case 0:
		return !mPhonemeSet->parse(phonemes, phonemes_end, p);
	case '{':
		if (!mPhonemeSet->parse(phonemes, phonemes_end, p))
			return true;
		feature_pos = 0;
		++rules;
		break;
	case '}':
		if (feature_pos != 3)
			return false;
		if (!p.get(feature))
			return false;
		++rules;
		feature_pos = 0xFF;
		break;
	default:
		if (feature_pos == 0xFF)
			return !mPhonemeSet->parse(phonemes, phonemes_end, p);
		if (feature_pos >= 4)
			return false;
		feature[feature_pos++] = *rules++;
		break;
	}
}

std::pair<const uint8_t *, const char *>
ruleset::next_match(const uint8_t *current)
{
	if (current == mEnd) return { nullptr, nullptr };

	uint16_t offset = mRuleGroups[*current];
	if (offset == 0)
	{
		++current;
		throw tts::phoneme_error(i18n("unable to pronounce the text"));
	}

	mRules.seek(offset);

	static const uint8_t null_rule[] = { 0 };

	enum state_t
	{
		in_rule_group,
		context_match,
		left_match,
		right_match,
	};

	state_t state = in_rule_group;
	const uint8_t *rule     = null_rule;
	const uint8_t *context  = nullptr;
	const uint8_t *left     = nullptr;
	const uint8_t *right    = nullptr;
	const char    *phonemes = nullptr;
	std::pair<const uint8_t *, const char *> ctx;
	while (true) switch (*rule)
	{
	case 0:
		switch (state)
		{
		case in_rule_group:
			rule = (const uint8_t *)mRules.pstr();
			if (*rule == 0)
			{
				++current;
				throw tts::phoneme_error(i18n("unable to pronounce the text"));
			}

			phonemes = mRules.pstr();
			context = left = right = current;
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
	case '{':
		switch (state)
		{
		case right_match:
			offset = mRules.offset();
			ctx = next_match(right);
			mRules.seek(offset);
			if (ctx.first != nullptr && match_phoneme(ctx.second, rule))
			{
				right = ctx.first;
			}
			else
			{
				state = in_rule_group;
				rule = null_rule;
			}
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
			if (context < mEnd && *context == *rule)
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
			if (right < mEnd && *right == *rule)
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
