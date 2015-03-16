/* Lexical Rewrite Rule Processor.
 *
 * Copyright (C) 2015 Reece H. Dunn
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

struct rewrite_rules : public tts::rewriter
{
	rewrite_rules(const std::shared_ptr<cainteoir::buffer> &aData);

	std::shared_ptr<cainteoir::buffer>
	rewrite(const std::shared_ptr<cainteoir::buffer> &aBuffer);
private:
	const uint8_t *next_match(const uint8_t *current, FILE *out);

	std::shared_ptr<cainteoir::buffer> mData;
	cainteoir::native_endian_buffer mRules;
	uint32_t mRuleGroups[256];
	bool mHaveRules;

	const uint8_t *mStart;
	const uint8_t *mEnd;
};

rewrite_rules::rewrite_rules(const std::shared_ptr<cainteoir::buffer> &aData)
	: mData(aData)
	, mRules((const uint8_t *)aData->begin(), (const uint8_t *)aData->end())
	, mHaveRules(false)
	, mStart(nullptr)
	, mEnd(nullptr)
{
	memset(mRuleGroups, 0, sizeof(mRuleGroups));

	mRules.seek(tts::LANGDB_HEADER_ID);
	const char *locale = mRules.pstr();
	const char *phonemeset = mRules.pstr();

	while (!mRules.eof()) switch (mRules.magic())
	{
	case tts::STRING_TABLE_MAGIC:
		mRules.seek(mRules.u32());
		break;
	case tts::CONDRULE_TABLE_MAGIC:
		{
			uint16_t entries = mRules.u16();
			uint32_t offset = mRules.offset();
			mRules.seek(offset + (entries * tts::CONDRULE_TABLE_ENTRY_SIZE));
		}
		break;
	case tts::CLASSDEF_TABLE_MAGIC:
		{
			uint16_t entries = mRules.u16();
			uint8_t  id = mRules.u8();
			uint32_t offset = mRules.offset();
			mRules.seek(offset + (entries * tts::CLASSDEF_TABLE_ENTRY_SIZE));
		}
		break;
	case tts::LETTER_TO_PHONEME_TABLE_MAGIC:
		{
			uint16_t entries = mRules.u16();
			uint8_t  id = mRules.u8();
			uint32_t offset = mRules.offset();
			mRules.seek(offset + (entries * tts::LETTER_TO_PHONEME_TABLE_ENTRY_SIZE));
		}
		break;
	case tts::LEXICAL_REWRITE_RULE_TABLE_MAGIC:
		{
			uint16_t entries = mRules.u16();
			uint8_t  id = mRules.u8();
			uint32_t offset = mRules.offset();
			mRules.seek(offset + (entries * tts::LEXICAL_REWRITE_RULES_TABLE_ENTRY_SIZE));
			mRuleGroups[id] = offset;
			mHaveRules = true;
		}
		break;
	case tts::DICTIONARY_TABLE_MAGIC:
		{
			uint16_t entries = mRules.u16();
			uint32_t offset = mRules.offset();
			mRules.seek(offset + (entries * tts::DICTIONARY_TABLE_ENTRY_SIZE));
		}
		break;
	default:
		throw std::runtime_error("unsupported section in the language file");
	}

}

std::shared_ptr<cainteoir::buffer>
rewrite_rules::rewrite(const std::shared_ptr<cainteoir::buffer> &aBuffer)
{
	if (!mHaveRules)
		return aBuffer;

	mStart = (const uint8_t *)aBuffer->begin();
	mEnd   = (const uint8_t *)aBuffer->end();

	const uint8_t *current = mStart;
	cainteoir::memory_file out;
	while (true)
	{
		auto match = next_match(current, out);
		if (!match)
			return out.buffer();

		current = match;
	}
}

const uint8_t *rewrite_rules::next_match(const uint8_t *current, FILE *out)
{
	if (current == mEnd) return nullptr;

	uint32_t offset = mRuleGroups[*current];
	if (offset == 0)
	{
		fputc(*current++, out);
		return current;
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
	const uint8_t *rule        = null_rule;
	const uint8_t *context     = nullptr;
	const uint8_t *left        = nullptr;
	const uint8_t *right       = nullptr;
	const char    *replacement = nullptr;
	while (true) switch (*rule)
	{
	case 0:
		switch (state)
		{
		case in_rule_group:
			rule = (const uint8_t *)mRules.pstr();
			if (*rule == 0)
			{
				fputc(*current++, out);
				return current;
			}

			replacement = mRules.pstr();
			context = left = right = current;
			state = context_match;
			break;
		default:
			fputs(replacement, out);
			return context;
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

std::shared_ptr<tts::rewriter>
tts::createLexicalRewriteRules(const char *aLanguageFile)
{
	if (!aLanguageFile) return {};

	auto data = cainteoir::make_file_buffer(aLanguageFile);
	if (!data) return {};

	const char *header = data->begin();

	if (strncmp(header, "LANGDB", 6) != 0 || *(const uint16_t *)(header + 6) != 0x3031)
		return {};

	return std::make_shared<rewrite_rules>(data);
}
