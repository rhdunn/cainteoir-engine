/* Word Stream.
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

#include "config.h"
#include "compatibility.hpp"

#include <cainteoir/text.hpp>

namespace tts = cainteoir::tts;

void tts::word_stream::next_item(const cainteoir::document_item &aItem)
{
	mReader.next_item(aItem);
}

bool tts::word_stream::read()
{
	if (mEntries.empty())
	{
		while (mReader.read())
		{
			bool end_of_clause = false;
			switch (mReader.type())
			{
			case tts::text_reader::word_uppercase:
			case tts::text_reader::word_lowercase:
			case tts::text_reader::word_capitalized:
			case tts::text_reader::word_mixedcase:
			case tts::text_reader::word_script:
				mEntries.push({ mReader.match() });
				break;
			case tts::text_reader::end_of_paragraph:
				end_of_clause = true;
				break;
			}

			if (end_of_clause && !mEntries.empty())
				return true;
		}

		// Don't pop the first matching item:
		return !mEntries.empty();
	}

	mEntries.pop();
	return !mEntries.empty();
}
