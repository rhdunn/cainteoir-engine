/* Text Processing API.
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

#ifndef CAINTEOIR_ENGINE_TEXT_HPP
#define CAINTEOIR_ENGINE_TEXT_HPP

#include "document.hpp"
#include <ucd/ucd.h>
#include <queue>

namespace cainteoir { namespace tts
{
	enum event_type
	{
		error,
		word_uppercase,
		word_lowercase,
		word_capitalized,
		word_mixedcase,
		word_script,
		number,
		punctuation,
		comma,
		semicolon,
		colon,
		ellipsis,
		full_stop,
		exclamation,
		question,
		symbol,
		paragraph,
	};

	struct text_event
	{
		std::shared_ptr<buffer> text;
		event_type type;
		ucd::script script;
		cainteoir::range<uint32_t> range;
		ucd::codepoint_t codepoint;

		text_event(const std::shared_ptr<buffer> &aText,
		           event_type aType,
		           ucd::script aScript,
		           const cainteoir::range<uint32_t> &aRange,
		           ucd::codepoint_t aCodePoint)
			: text(aText)
			, type(aType)
			, script(aScript)
			, range(aRange)
			, codepoint(aCodePoint)
		{
		}

		text_event()
			: range(0, 0)
		{
		}
	};

	struct text_reader
	{
		text_reader(const std::shared_ptr<document_reader> &aReader);

		const text_event &event() const { return mMatch; }

		bool read();
	private:
		bool matched();

		enum class reader_state;

		std::shared_ptr<document_reader> mReader;
		text_event mMatch;

		char mMatchBuffer[512];
		char *mMatchCurrent;
		uint32_t mMatchFirst;
		uint32_t mMatchNext;
		uint32_t mMatchLast;

		bool mNeedEndPara;
		const char *mCurrent;
		const char *mLast;
		reader_state mReaderState;
		uint8_t mState;
	};

	struct word_stream
	{
	public:
		word_stream(const std::shared_ptr<document_reader> &aReader)
			: mReader(aReader)
		{
		}

		const text_event &event() const { return mEntries.front(); }

		bool read();
	private:
		text_reader mReader;
		std::queue<text_event> mEntries;
	};
}}

#endif
