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
	symbol,
	title_case_initial,
	script,
	error_modifier,
};

static const bool state_is_terminal[] = {
	false, // 0 = start
	true,  // A = upper case (initial character)
	true,  // B = upper case
	true,  // C = capitalized
	true,  // D = lower case
	true,  // E = mixed case
	true,  // F = number
	true,  // G = punctuation
	true,  // H = symbol
	true,  // I = title case (initial character) -- capitalized
	true,  // J = script
	true,  // K = error (unrecognised modifier/marker)
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
	tts::text_reader::word_capitalized,
	tts::text_reader::word_script,
	tts::text_reader::error,
};

#define A  1
#define B  2
#define C  3
#define D  4
#define E  5
#define F  6
#define G  7
#define H  8
#define I  9
#define J 10
#define K 11

static const uint8_t state_transitions[][31] = {
	// Cc Cf Cn Co Cs Ii Ll Lm Lo Lt Lu Mc Me Mn Nd Nl No Pc Pd Pe Pf Pi Po Ps Sc Sk Sm So Zl Zp Zs
	{  0, 0, 0, 0, 0, 0, D, K, J, I, A, K, K, K, F, F, F, G, G, G, G, G, G, G, H, H, H, H, 0, 0, 0 }, // 0
	{  0, 0, 0, 0, 0, 0, C, 0, 0, E, B, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // A
	{  0, 0, 0, 0, 0, 0, E, 0, 0, E, B, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // B
	{  0, 0, 0, 0, 0, 0, C, 0, 0, E, E, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // C
	{  0, 0, 0, 0, 0, 0, D, 0, 0, E, E, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // D
	{  0, 0, 0, 0, 0, 0, E, 0, 0, E, E, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // E
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, F, F, F, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // F
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // G
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // H
	{  0, 0, 0, 0, 0, 0, C, 0, 0, E, E, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // I
	{  0, 0, 0, 0, 0, 0, 0, 0, J, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // J
	{  0, 0, 0, 0, 0, 0, 0, K, 0, 0, 0, K, K, K, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // K
};

#undef A
#undef B
#undef C
#undef D
#undef E
#undef F
#undef G
#undef H
#undef I
#undef J
#undef K

tts::text_reader::text_reader()
	: mType(error)
	, mMatchCurrent(mMatchBuffer)
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

#define SOFT_HYPHEN                 0x00AD
#define RIGHT_SINGLE_QUOTATION_MARK 0x2019

bool tts::text_reader::read()
{
	if (mReaderState == reader_state::skip)
		return false;

	if (mReaderState == reader_state::end_paragraph)
	{
		if (state_is_terminal[mState])
		{
			mType = state_token[mState];
			mState = 0;
			mMatch = make_buffer(mMatchBuffer, mMatchCurrent - mMatchBuffer);
			mMatchCurrent = mMatchBuffer;
		}
		else
		{
			mScript = ucd::Zzzz;
			mType = end_of_paragraph;
			mReaderState = reader_state::skip;
			mNeedEndPara = false;
		}
		return true;
	}

	uint32_t cp = 0;
	const char *next = nullptr;
	const char *quote_match = nullptr;
	for (; (next = cainteoir::utf8::read(mCurrent, cp)) <= mLast; mCurrent = next)
	{
		ucd::category category = ucd::lookup_category(cp);
		ucd::script   script   = ucd::lookup_script(cp);

		if (quote_match)
		{
			if (category == ucd::Lu || category == ucd::Ll)
			{
				// The single curly quote is inbetween letters, so keep as part of
				// the word token.
				quote_match = nullptr;
			}
			else
			{
				// The curly quote is at the end of the word token, so don't make
				// it part of the word token (keep it as a separate punctuation
				// token).
				mCurrent = quote_match;
				mType = state_token[mState];
				mState = 0;
				mMatch = make_buffer(mMatchBuffer, mMatchCurrent - mMatchBuffer - 1);
				mMatchCurrent = mMatchBuffer;
				return true;
			}
		}

		uint8_t new_state = state_transitions[mState][category];
		switch ((state)mState)
		{
		case state::upper_case_initial:
		case state::upper_case:
		case state::capitalized:
		case state::lower_case:
		case state::mixed_case:
		case state::title_case_initial:
			if (cp == '\'' || cp == RIGHT_SINGLE_QUOTATION_MARK)
			{
				quote_match = mCurrent;
				new_state = mState;
				cp = '\'';
			}
			else if (cp == SOFT_HYPHEN)
				continue;
			break;
		case state::script:
			if (mScript != script && mScript != ucd::Zzzz)
			{
				// The script has changed, so split the token here.
				new_state = 0;
			}
			break;
		}

		if (state_is_terminal[mState] && !state_is_terminal[new_state])
		{
			mType = state_token[mState];
			mState = 0;
			mMatch = make_buffer(mMatchBuffer, mMatchCurrent - mMatchBuffer);
			mMatchCurrent = mMatchBuffer;
			return true;
		}

		if (mState != new_state)
		{
			mScript = script;
			mState = new_state;
			mNeedEndPara = true;
		}

		if (mState != 0)
		{
			mMatchCurrent = cainteoir::utf8::write(mMatchCurrent, ucd::tolower(cp));
			if ((mMatchCurrent - mMatchBuffer) >= (sizeof(mMatchBuffer) - 12))
			{
				// The match is too long for the internal buffer, so split the word here.
				mType = state_token[mState];
				mCurrent = next;
				mState = 0;
				mMatch = make_buffer(mMatchBuffer, mMatchCurrent - mMatchBuffer);
				mMatchCurrent = mMatchBuffer;
				return true;
			}
		}
	}

	return false;
}
