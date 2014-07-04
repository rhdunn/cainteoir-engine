/* Context Analysis.
 *
 * Copyright (C) 2013-2014 Reece H. Dunn
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
	number,       // number (maybe an ordinal number)
	clause_break, // sequence of punctuation
	end,          // next event is a word
};

struct context_analysis_t : public tts::text_reader
{
public:
	context_analysis_t();

	void bind(const std::shared_ptr<tts::text_reader> &aReader);

	const tts::text_event &event() const { return mClause.front(); }

	bool read();
private:
	bool read_clause();

	bool mHaveEvent;
	std::shared_ptr<tts::text_reader> mReader;
	std::queue<tts::text_event> mClause;
};

context_analysis_t::context_analysis_t()
	: mHaveEvent(false)
{
}

void context_analysis_t::bind(const std::shared_ptr<tts::text_reader> &aReader)
{
	mReader = aReader;
	mHaveEvent = mReader->read();
}

bool context_analysis_t::read()
{
	if (mClause.empty())
	{
		if (!read_clause()) return false;

		// Don't pop the first matching item:
		return !mClause.empty();
	}

	mClause.pop();
	if (mClause.empty())
		return read();

	return true;
}

bool context_analysis_t::read_clause()
{
	clause_state state = clause_state::start;

	while (mHaveEvent && state != clause_state::end)
	{
		auto &event = mReader->event();
		switch (state)
		{
		case clause_state::start:
			switch (event.type)
			{
			case tts::word_uppercase:
			case tts::word_lowercase:
			case tts::word_capitalized:
			case tts::word_mixedcase:
			case tts::word_script:
				mClause.push(event);
				break;
			case tts::number:
				mClause.push(event);
				state = clause_state::number;
				break;
			case tts::punctuation:
			case tts::exclamation:
			case tts::question:
			case tts::comma:
			case tts::full_stop:
			case tts::colon:
			case tts::semicolon:
			case tts::em_dash:
			case tts::en_dash:
			case tts::ellipsis:
			case tts::symbol:
			case tts::paragraph:
				state = clause_state::clause_break;
				continue;
			};
			break;
		case clause_state::number:
			switch (event.type)
			{
			case tts::word_uppercase:
			case tts::word_lowercase:
			case tts::word_capitalized:
			case tts::word_mixedcase:
				{
					if (event.text->compare("st") == 0 ||
					    event.text->compare("nd") == 0 ||
					    event.text->compare("rd") == 0 ||
					    event.text->compare("th") == 0)
					{
						auto &n = mClause.back();
						n.type = tts::ordinal_number;
						n.range = { n.range.begin(), event.range.end() };
					}
					else
						mClause.push(event);
					state = clause_state::start;
				}
				break;
			case tts::punctuation:
			case tts::exclamation:
			case tts::question:
			case tts::comma:
			case tts::full_stop:
			case tts::colon:
			case tts::semicolon:
			case tts::em_dash:
			case tts::en_dash:
			case tts::ellipsis:
			case tts::symbol:
			case tts::paragraph:
				state = clause_state::clause_break;
				continue;
			default:
				mClause.push(event);
				state = clause_state::start;
				break;
			}
			break;
		case clause_state::clause_break:
			switch (event.type)
			{
			case tts::word_uppercase:
			case tts::word_lowercase:
			case tts::word_capitalized:
			case tts::word_mixedcase:
			case tts::word_script:
			case tts::number:
				state = mClause.empty() ? clause_state::start : clause_state::end;
				continue;
			case tts::exclamation:
			case tts::question:
			case tts::comma:
			case tts::full_stop:
			case tts::colon:
			case tts::semicolon:
			case tts::em_dash:
			case tts::en_dash:
			case tts::ellipsis:
			case tts::paragraph:
				mClause.push(event);
				break;
			};
			break;
		}
		mHaveEvent = mReader->read();
	}
	return !mClause.empty();
}

std::shared_ptr<tts::text_reader> tts::context_analysis()
{
	return std::make_shared<context_analysis_t>();
}
