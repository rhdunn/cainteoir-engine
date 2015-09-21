/* Text Processing.
 *
 * Copyright (C) 2012-2015 Reece H. Dunn
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
	text_reader_t(tts::text_callback *aCallback);

	void reset(const std::shared_ptr<cainteoir::document_reader> &aReader);

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
	tts::text_callback *mCallback;

	char mMatchBuffer[512];
	char *mMatchCurrent;
	uint32_t mMatchNext;
	uint32_t mMatchLast;

	bool mNeedEndPara;
	const char *mCurrent;
	const char *mLast;
	reader_state mReaderState;
	uint8_t mState;

	bool mHaveSavedState;
	uint8_t mSavedState;
	const char *mSavedCurrent;
	char *mSavedMatchCurrent;
	uint32_t mSavedMatchLast;
};

text_reader_t::text_reader_t(tts::text_callback *aCallback)
	: mCallback(aCallback)
	, mMatchCurrent(mMatchBuffer)
	, mNeedEndPara(false)
	, mCurrent(nullptr)
	, mLast(nullptr)
	, mState(0)
	, mReaderState(reader_state::end_paragraph)
	, mMatchNext(0)
	, mMatchLast(0)
	, mHaveSavedState(false)
	, mSavedState(0)
	, mSavedCurrent(nullptr)
	, mSavedMatchCurrent(nullptr)
	, mSavedMatchLast(0)
{
}

void text_reader_t::reset(const std::shared_ptr<cainteoir::document_reader> &aReader)
{
	mReader = aReader;
	mReaderState = mReader ? reader_state::need_text : reader_state::end_paragraph;
}

bool text_reader_t::read()
{
	while (mReaderState == reader_state::need_text && mReader->read())
	{
		if (mCallback) mCallback->onevent(*mReader);

		if (mReader->type & cainteoir::events::text)
		{
			mCurrent = mReader->content->begin();
			mLast = mReader->content->end();
			mReaderState = reader_state::have_text;
			mSavedCurrent = mCurrent;
			if (!mHaveSavedState)
			{
				mSavedState = mState;
				mSavedMatchCurrent = mMatchCurrent;
				mSavedMatchLast = mMatchLast;
			}
			else
				mHaveSavedState = false;
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

		token = cainteoir::object(cainteoir::object_type::dictionary);
		token.put("Token:type", tts::paragraph);
		token.put("Token:text", cainteoir::object{});
		token.put("Token:range", cainteoir::range<uint32_t>{ mMatchNext, mMatchLast });

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
			lang = fsm::language::AP;
			break;
		case 0x002D: // HYPHEN-MINUS
			lang = fsm::language::HM;
			break;
		case 0x002E: // FULL STOP
			lang = fsm::language::FS;
			break;
		case 0x00AD: // SOFT HYPHEN
			lang = fsm::language::SH;
			break;
		case 0x2019: // RIGHT SINGLE QUOTATION MARK
			lang = fsm::language::RQ;
			break;
		case 0x2029: // PARAGRAPH SEPARATOR
			lang = fsm::language::PS;
			break;
		// Extended Classifications
		case 0x2014: // EM DASH
			lang = fsm::language::Xm;
			break;
		case 0x2013: // EN DASH
			lang = fsm::language::Xn;
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
				mState = mSavedState;
				mCurrent = mSavedCurrent;
				mMatchCurrent = (char *)cainteoir::utf8::next(mSavedMatchCurrent);
				mMatchLast = mSavedMatchLast + 1;
			}
			return matched();
		}
		else if (fsm::data[new_state].is_terminal)
		{
			mHaveSavedState = true;
			mSavedState = new_state;
			mSavedCurrent = next;
			mSavedMatchCurrent = mMatchCurrent;
			mSavedMatchLast = mMatchLast;
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
	token = cainteoir::object(cainteoir::object_type::dictionary);
	token.put("Token:type", fsm::data[mState].value);
	token.put("Token:text", cainteoir::make_buffer(mMatchBuffer, mMatchCurrent - mMatchBuffer));
	token.put("Token:range", cainteoir::range<uint32_t>{ mMatchNext, mMatchLast });

	mMatchCurrent = mMatchBuffer;
	mMatchNext = mMatchLast;
	mHaveSavedState = false;
	mState = 0;
	return true;
}

std::shared_ptr<tts::text_reader>
tts::create_text_reader(tts::text_callback *aCallback)
{
	return std::make_shared<text_reader_t>(aCallback);
}
