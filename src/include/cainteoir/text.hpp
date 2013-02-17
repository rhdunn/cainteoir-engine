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
		symbol,
		end_of_paragraph,
	};

	struct text_event
	{
		std::shared_ptr<buffer> text;
		event_type type;
		ucd::script script;
		cainteoir::range<uint32_t> range;

		text_event()
			: range(0, 0)
		{
		}
	};

	struct text_reader
	{
		text_reader();

		const text_event &match() const { return mMatch; }

		void next_item(const cainteoir::document_item &aItem);

		bool read();
	private:
		bool matched();

		enum class reader_state;

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
		struct entry_t
		{
			std::shared_ptr<buffer> text;
		};

		void next_item(const cainteoir::document_item &aItem);

		bool read();

		const entry_t &entry() const { return mEntries.front(); }
	private:
		text_reader mReader;
		std::queue<entry_t> mEntries;
	};
}}

#endif
