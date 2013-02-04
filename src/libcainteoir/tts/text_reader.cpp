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
	: mMatch(nullptr, nullptr)
	, mType(error)
	, mStart(nullptr)
	, mCurrent(nullptr)
	, mLast(nullptr)
	, mState(0)
	, mScript(ucd::Zzzz)
{
}

void tts::text_reader::set_buffer(const std::shared_ptr<cainteoir::buffer> &aBuffer)
{
	mStart = mCurrent = aBuffer->begin();
	mLast = aBuffer->end();
}

#define RIGHT_SINGLE_QUOTATION_MARK 0x2019

bool tts::text_reader::read()
{
	uint32_t cp = 0;
	const char *next = nullptr;
	mState = 0;
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
			// Don't transition states here.
			new_state = mState;
			break;
		}

		if (state_is_terminal[mState] && !state_is_terminal[new_state])
		{
			mType = state_token[mState];
			mMatch = cainteoir::buffer(mStart, mCurrent);
			mStart = mCurrent;
			return true;
		}

		if (mState != new_state)
		{
			mScript = script;
			mState = new_state;
		}

		if (mState == 0)
			mStart = next;
	}

	return false;
}
