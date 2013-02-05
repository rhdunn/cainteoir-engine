/* Text Processing.
 *
 * Copyright (C) 2012-2013 Reece H. Dunn
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
#include "compatibility.hpp"

#include <cainteoir/text.hpp>
#include <cainteoir/unicode.hpp>

namespace tts = cainteoir::tts;

enum class tts::text_reader::reader_state
{
	skip,
	have_text,
	end_paragraph,
};

enum class state
{
	start,
	upper_case_initial,
	upper_case,
	capitalized,
	lower_case,
	mixed_case,
	number,
	punctuation,
	symbol
};

static const bool state_is_terminal[] = {
	false, // 0 = start
	true,  // 1 = upper case (initial character)
	true,  // 2 = upper case
	true,  // 3 = capitalized
	true,  // 4 = lower case
	true,  // 5 = mixed case
	true,  // 6 = number
	true,  // 7 = punctuation
	true,  // 8 = symbol
};

static const tts::text_reader::token_type state_token[] = {
	tts::text_reader::error,
	tts::text_reader::word_uppercase,
	tts::text_reader::word_uppercase,
	tts::text_reader::word_capitalized,
	tts::text_reader::word_lowercase,
	tts::text_reader::word_mixedcase,
	tts::text_reader::number,
	tts::text_reader::punctuation,
	tts::text_reader::symbol,
};

static const uint8_t state_transitions[][31] = {
	// Cc Cf Cn Co Cs Ii Ll Lm Lo Lt Lu Mc Me Mn Nd Nl No Pc Pd Pe Pf Pi Po Ps Sc Sk Sm So Zl Zp Zs
	{  0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 1, 0, 0, 0, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 0, 0, 0 }, // 0
	{  0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 1
	{  0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 2
	{  0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 3
	{  0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 4
	{  0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 5
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 6
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 7
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 8
};

tts::text_reader::text_reader()
	: mType(error)
	, mMatchEnd(mMatch)
	, mNeedEndPara(false)
	, mCurrent(nullptr)
	, mLast(nullptr)
	, mState(0)
	, mReaderState(reader_state::skip)
	, mScript(ucd::Zzzz)
{
}

void tts::text_reader::next_item(const cainteoir::document_item &aItem)
{
	if (aItem.type & cainteoir::events::text)
	{
		mCurrent = aItem.text->begin();
		mLast = aItem.text->end();
		mReaderState = reader_state::have_text;
	}
	else if (aItem.type & cainteoir::events::end_context)
	{
		if (aItem.styles) switch (aItem.styles->display)
		{
		case css::display::block:
		case css::display::table:
		case css::display::table_row:
		case css::display::table_cell:
		case css::display::list_item:
			if (mNeedEndPara)
			{
				mReaderState = reader_state::end_paragraph;
			}
			break;
		}
	}
	else
		mReaderState = reader_state::skip;
}

#define RIGHT_SINGLE_QUOTATION_MARK 0x2019

bool tts::text_reader::read()
{
	if (mReaderState == reader_state::skip)
		return false;

	if (mReaderState == reader_state::end_paragraph)
	{
		mScript = ucd::Zyyy;
		mMatchEnd = mMatch;
		mType = end_of_paragraph;
		mReaderState = reader_state::skip;
		mNeedEndPara = false;
		return true;
	}

	uint32_t cp = 0;
	const char *next = nullptr;
	mState = 0;
	mMatchEnd = mMatch;
	for (; (next = cainteoir::utf8::read(mCurrent, cp)) <= mLast; mCurrent = next)
	{
		ucd::category category = ucd::lookup_category(cp);
		ucd::script   script   = ucd::lookup_script(cp);

		uint8_t new_state = state_transitions[mState][category];
		if (cp == '\'' || cp == RIGHT_SINGLE_QUOTATION_MARK) switch ((state)mState)
		{
		case state::upper_case_initial:
		case state::upper_case:
		case state::capitalized:
		case state::lower_case:
		case state::mixed_case:
			new_state = mState;
			cp = '\'';
			break;
		}

		if (state_is_terminal[mState] && !state_is_terminal[new_state])
		{
			mType = state_token[mState];
			mNeedEndPara = true;
			return true;
		}

		if (mState != new_state)
		{
			mScript = script;
			mState = new_state;
		}

		if (mState != 0)
			mMatchEnd = cainteoir::utf8::write(mMatchEnd, ucd::tolower(cp));
	}

	if (state_is_terminal[mState])
	{
		mType = state_token[mState];
		mNeedEndPara = true;
		return true;
	}

	return false;
}
