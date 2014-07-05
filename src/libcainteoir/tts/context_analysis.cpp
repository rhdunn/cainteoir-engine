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

struct context_analysis_t : public tts::text_reader, public tts::clause_processor
{
public:
	context_analysis_t();

	// tts::text_reader

	void bind(const std::shared_ptr<tts::text_reader> &aReader);

	const tts::text_event &event() const { return mClause.front(); }

	bool read();

	// tts::clause_processor

	void chain(const std::shared_ptr<tts::clause_processor> &aProcessor);

	void process(std::list<tts::text_event> &aClause);
private:
	std::shared_ptr<tts::text_reader> mReader;
	std::list<tts::text_event> mClause;
};

context_analysis_t::context_analysis_t()
{
}

void context_analysis_t::bind(const std::shared_ptr<tts::text_reader> &aReader)
{
	mReader = aReader;
}

bool context_analysis_t::read()
{
	if (mClause.empty())
	{
		if (!tts::next_clause(mReader, mClause)) return false;
		process(mClause);

		// Don't pop the first matching item:
		return !mClause.empty();
	}

	mClause.pop_front();
	if (mClause.empty())
		return read();

	return true;
}

void context_analysis_t::chain(const std::shared_ptr<tts::clause_processor> &aProcessor)
{
}

void context_analysis_t::process(std::list<tts::text_event> &aClause)
{
	for (auto current = aClause.begin(), last = aClause.end(); current != last;)
	{
		if (current->type == tts::number)
		{
			auto number = current;
			++current;
			if (current != last && current->type == tts::word_lowercase)
			{
				const auto &text = current->text;
				if (text->compare("st") == 0 ||
				    text->compare("nd") == 0 ||
				    text->compare("rd") == 0 ||
				    text->compare("th") == 0)
				{
					number->type = tts::ordinal_number;
					number->range = { number->range.begin(), current->range.end() };
					current = aClause.erase(current);
				}
			}
			continue;
		}
		++current;
	}
}

std::shared_ptr<tts::text_reader> tts::context_analysis()
{
	return std::make_shared<context_analysis_t>();
}
