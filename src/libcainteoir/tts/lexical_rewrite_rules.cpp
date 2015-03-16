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
	cainteoir::native_endian_buffer mRules;
	uint32_t mRuleGroups[256];
	bool mHaveRules;
};

rewrite_rules::rewrite_rules(const std::shared_ptr<cainteoir::buffer> &aData)
	: mRules((const uint8_t *)aData->begin(), (const uint8_t *)aData->end())
	, mHaveRules(false)
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
	return aBuffer;
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
