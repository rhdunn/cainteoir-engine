/* Read the next clause.
 *
 * Copyright (C) 2014 Reece H. Dunn
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

namespace tts = cainteoir::tts;

enum class clause_state
{
	start,        // sequence of words
	clause_break, // sequence of punctuation
	end,          // next event is a word
};

bool
tts::next_clause(const std::shared_ptr<tts::text_reader> &aReader,
                 std::list<tts::text_event> &aClause)
{
	aClause.clear();

	clause_state state = aReader->read() ? clause_state::start : clause_state::end;
	while (state != clause_state::end)
	{
		auto &event = aReader->event();

		switch (event.type)
		{
		case tts::punctuation:
		case tts::symbol:
			break;
		case tts::exclamation:
		case tts::question:
		case tts::comma:
		case tts::full_stop:
		case tts::double_stop:
		case tts::colon:
		case tts::semicolon:
		case tts::em_dash:
		case tts::en_dash:
		case tts::ellipsis:
		case tts::paragraph:
			aClause.push_back(event);
			return true;
		default:
			aClause.push_back(event);
			break;
		}

		if (!aReader->read())
			state = clause_state::end;
	}
	return !aClause.empty();
}
