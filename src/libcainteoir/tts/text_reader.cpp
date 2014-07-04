/* Text Processing.
 *
 * Copyright (C) 2012-2014 Reece H. Dunn
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
namespace css = cainteoir::css;

#include "text_reader.fsm.h"

struct text_reader_t : public tts::text_reader
{
	text_reader_t(const std::shared_ptr<cainteoir::document_reader> &aReader);

	void bind(const std::shared_ptr<tts::text_reader> &aReader);

	const tts::text_event &event() const { return mMatch; }

	bool read();
private:
	bool matched();

	enum class reader_state
	{
		need_text,
		have_text,
		end_paragraph,
	};

	std::shared_ptr<cainteoir::document_reader> mReader;
	tts::text_event mMatch;

	char mMatchBuffer[512];
	char *mMatchCurrent;
	uint32_t mMatchNext;
	uint32_t mMatchLast;

	bool mNeedEndPara;
	const char *mCurrent;
	const char *mLast;
	reader_state mReaderState;
	uint8_t mState;
};

text_reader_t::text_reader_t(const std::shared_ptr<cainteoir::document_reader> &aReader)
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
	mMatch.type = tts::error;
}

void text_reader_t::bind(const std::shared_ptr<tts::text_reader> &aReader)
{
	throw std::runtime_error("cannot bind to a text_reader object");
}

bool text_reader_t::read()
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

		mMatch.type = tts::paragraph;
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

	for (; (next = cainteoir::utf8::read(mCurrent, cp)) <= mLast; mCurrent = next)
	{
		fsm::language lang;
		switch (cp)
		{
		// Special Case Characters
		case 0x000A: // LINE FEED (LF)
			lang = fsm::language::LF;
			break;
		case 0x0027: // APOSTROPHE
			lang = fsm::language::qQ;
			break;
		case 0x002D: // HYPHEN-MINUS
			lang = fsm::language::HM;
			break;
		case 0x00AD: // SOFT HYPHEN
			lang = fsm::language::SH;
			break;
		case 0x2019: // RIGHT SINGLE QUOTATION MARK
			lang = fsm::language::qR;
			break;
		// Extended Classifications
		case 0x0021: // Exclamation Mark
			lang = fsm::language::EX;
			break;
		case 0x2029: // PARAGRAPH SEPARATOR
			lang = fsm::language::PS;
			break;
		// Unicode General Category
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

		uint8_t new_state = (uint8_t)fsm::transitions[mState][(int)lang];
		if (new_state == (int)fsm::state::terminal)
		{
			if (!fsm::data[mState].is_terminal)
			{
				mState = saved_state;
				mCurrent = saved_current;
				mMatchCurrent = (char *)cainteoir::utf8::next(saved_match_current);
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
			if (data.replacement_character)
				cp = data.replacement_character;
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
	}

	mReaderState = reader_state::need_text;
	return read();
}

bool text_reader_t::matched()
{
	mMatch.type = fsm::data[mState].value;
	mState = 0;
	mMatch.text = cainteoir::make_buffer(mMatchBuffer, mMatchCurrent - mMatchBuffer);
	mMatch.range = { mMatchNext, mMatchLast };
	mMatchCurrent = mMatchBuffer;
	mMatchNext = mMatchLast;
	return true;
}

std::shared_ptr<tts::text_reader> tts::create_text_reader(const std::shared_ptr<document_reader> &aReader)
{
	return std::make_shared<text_reader_t>(aReader);
}
