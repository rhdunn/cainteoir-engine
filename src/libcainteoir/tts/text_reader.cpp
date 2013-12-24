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

#include <ucd/ucd.h>
#include <cainteoir/text.hpp>
#include <cainteoir/unicode.hpp>

namespace tts = cainteoir::tts;

#include "text_reader.fsm.h"

enum class tts::text_reader::reader_state
{
	need_text,
	have_text,
	end_paragraph,
};

tts::text_reader::text_reader(const std::shared_ptr<document_reader> &aReader)
	: mReader(aReader)
	, mMatchCurrent(mMatchBuffer)
	, mNeedEndPara(false)
	, mCurrent(nullptr)
	, mLast(nullptr)
	, mState(0)
	, mReaderState(reader_state::need_text)
	, mMatchNext(0)
	, mMatchLast(0)
{
	mMatch.type = error;
}

#define LINE_FEED                   0x000A
#define SOFT_HYPHEN                 0x00AD
#define RIGHT_SINGLE_QUOTATION_MARK 0x2019

bool tts::text_reader::read()
{
	while (mReaderState == reader_state::need_text && mReader->read())
	{
		if (mReader->type & cainteoir::events::text)
		{
			mCurrent = mReader->content->begin();
			mLast = mReader->content->end();
			mReaderState = reader_state::have_text;
		}
		else if (mReader->type & cainteoir::events::end_context)
		{
			if (mReader->styles) switch (mReader->styles->display)
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
	}

	if (mReaderState == reader_state::end_paragraph)
	{
		if (mState != (int)fsm::state::start)
			return matched();

		mMatch.type = paragraph;
		mMatch.range = { mMatchNext, mMatchLast };
		mReaderState = reader_state::need_text;
		mNeedEndPara = false;
		mMatchNext = mMatchLast;
		return true;
	}

	if (mReaderState == reader_state::need_text)
		return mMatchCurrent == mMatchBuffer ? false : matched();

	ucd::codepoint_t cp = 0;
	const char *next = nullptr;
	ucd::script current_script = ucd::Zzzz;

	uint8_t saved_state = mState;
	const char *saved_current = mCurrent;
	char *saved_match_current = mMatchCurrent;
	uint32_t saved_match_last = mMatchLast;

	const char *quote_match = nullptr;
	int newline_count = 0;
	uint32_t newline_start = mMatchNext;
	while ((next = cainteoir::utf8::read(mCurrent, cp)) <= mLast)
	{
		fsm::language lang;
		switch (cp)
		{
		case 0x002D: lang = fsm::language::HM; break; // Hyphen-Minus
		case 0x2029: lang = fsm::language::PS; break; // Paragraph Separator
		default:
			lang = (fsm::language)ucd::lookup_category(cp);
			if (lang == fsm::language::Lo)
			{
				ucd::script script = ucd::lookup_script(cp);
				if (current_script != script && current_script != ucd::Zzzz)
					lang = fsm::language::Cc; // The script has changed, so split the token here.
				current_script = script;
			}
			break;
		}

		if (quote_match)
		{
			if (lang == fsm::language::Lu || lang == fsm::language::Ll)
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
				--mMatchCurrent;
				return matched();
			}
		}

		if (cp == LINE_FEED)
		{
			++newline_count;
		}
		else if (newline_count >= 2)
		{
			mMatch.type = paragraph;
			mMatch.range = { newline_start, mMatchLast };
			mNeedEndPara = false;
			mMatchNext = mMatchLast;
			return true;
		}

		uint8_t new_state = (uint8_t)fsm::transitions[mState][(int)lang];
		switch ((fsm::state)mState)
		{
		case fsm::state::upper_case_initial:
		case fsm::state::upper_case:
		case fsm::state::capitalized:
		case fsm::state::lower_case:
		case fsm::state::mixed_case:
		case fsm::state::title_case_initial:
			if (cp == '\'' || cp == RIGHT_SINGLE_QUOTATION_MARK)
			{
				quote_match = mCurrent;
				new_state = mState;
				cp = '\'';
			}
			else if (cp == SOFT_HYPHEN)
			{
				mCurrent = next;
				continue;
			}
			break;
		}

		if (new_state == (int)fsm::state::terminal)
		{
			if (!fsm::data[mState].is_terminal)
			{
				mState = saved_state;
				mCurrent = saved_current;
				mMatchCurrent = saved_match_current + 1;
				mMatchLast = saved_match_last + 1;
			}
			return matched();
		}
		else if (fsm::data[new_state].is_terminal)
		{
			saved_state = mState;
			saved_current = next;
			saved_match_current = mMatchCurrent;
			saved_match_last = mMatchLast;
		}

		++mMatchLast;

		if (mState != new_state)
		{
			mState = new_state;
			mNeedEndPara = true;
		}

		const auto &data = fsm::data[mState];

		if (data.emit_character)
		{
			mMatch.codepoint = cp;
			mMatchCurrent = cainteoir::utf8::write(mMatchCurrent, ucd::tolower(cp));
			if ((mMatchCurrent - mMatchBuffer) >= (sizeof(mMatchBuffer) - 12))
			{
				// The match is too long for the internal buffer, so split the word here.
				mCurrent = next;
				return matched();
			}
		}

		if (data.advance_match_start)
			++mMatchNext;

		if (data.consume_input)
			mCurrent = next;
	}

	mReaderState = reader_state::need_text;
	return read();
}

bool tts::text_reader::matched()
{
	mMatch.type = fsm::data[mState].value;
	mState = 0;
	mMatch.text = make_buffer(mMatchBuffer, mMatchCurrent - mMatchBuffer);
	mMatch.range = { mMatchNext, mMatchLast };
	mMatchCurrent = mMatchBuffer;
	mMatchNext = mMatchLast;
	return true;
}
