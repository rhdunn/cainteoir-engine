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

namespace cainteoir { namespace tts
{
	struct text_reader
	{
		enum token_type
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

		text_reader();

		std::shared_ptr<buffer> match() const
		{
			return make_buffer(mMatch, mMatchEnd - mMatch);
		}

		token_type type() const { return mType; }

		ucd::script script() const { return mScript; }

		void next_item(const cainteoir::document_item &aItem);

		bool read();
	private:
		enum class reader_state;

		token_type mType;
		ucd::script mScript;

		char mMatch[512];
		char *mMatchCurrent;
		char *mMatchEnd;

		bool mNeedEndPara;
		const char *mCurrent;
		const char *mLast;
		reader_state mReaderState;
		uint8_t mState;
	};
}}

#endif
