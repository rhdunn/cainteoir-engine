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

struct punctuation_t
{
	ucd::codepoint_t codepoint;
	tts::event_type event;
};

static const std::initializer_list<punctuation_t> punctuation =
{
	{ 0x00003A, tts::colon       }, // COLON
	{ 0x00003B, tts::semicolon   }, // SEMICOLON
	{ 0x00003F, tts::question    }, // QUESTION MARK
	{ 0x002013, tts::en_dash     }, // EN DASH
	{ 0x002014, tts::em_dash     }, // EM DASH
	{ 0x002026, tts::ellipsis    }, // HORIZONTAL ELLIPSIS
	{ 0x0022EE, tts::ellipsis    }, // VERTICAL ELLIPSIS
};

static tts::event_type punctuation_type(ucd::codepoint_t cp)
{
	for (auto &item : punctuation)
	{
		if (cp == item.codepoint)
			return item.event;
		if (cp < item.codepoint)
			return tts::punctuation;
	}
	return tts::punctuation;
}

struct punctuation_sequence
{
	punctuation_sequence()
		: mBegin(0)
		, mEnd(0)
		, mCount(0)
		, mCodePoint(0)
	{
	}

	void add(const cainteoir::range<uint32_t> &aRange);

	void flush(std::queue<tts::text_event> &aClause);

	ucd::codepoint_t codepoint() const { return mCodePoint; }

	void set_codepoint(ucd::codepoint_t aCodePoint) { mCodePoint = aCodePoint; }
private:
	uint32_t mBegin;
	uint32_t mEnd;
	uint32_t mCount;
	ucd::codepoint_t mCodePoint;
};

void punctuation_sequence::add(const cainteoir::range<uint32_t> &aRange)
{
	if (mBegin == 0)
	{
		mBegin = aRange.begin();
		mEnd   = aRange.end();
		mCount = 1;
	}
	else
	{
		mEnd = aRange.end();
		++mCount;
	}
}

void punctuation_sequence::flush(std::queue<tts::text_event> &aClause)
{
	if (mCount == 0) return;

	#define _(x) std::make_shared<cainteoir::buffer>(x)

	if (mCodePoint == '.') switch (mCount)
	{
	case 1:
		aClause.push({ _("."), tts::full_stop, { mBegin, mEnd }, '.' });
		break;
	case 2:
		aClause.push({ _(".."), tts::double_stop, { mBegin, mEnd }, '.' });
		break;
	default:
		aClause.push({ _("..."), tts::ellipsis, { mBegin, mEnd }, '.' });
		break;
	}
	else if (mCodePoint == '-') switch (mCount)
	{
	case 1:
		break;
	case 2:
		aClause.push({ _("--"), tts::en_dash, { mBegin, mEnd }, '-' });
		break;
	case 3:
		aClause.push({ _("---"), tts::em_dash, { mBegin, mEnd }, '-' });
		break;
	}

	#undef _

	mBegin = mEnd = mCount = 0;
	mCodePoint = 0;
}

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
	punctuation_sequence sequence;

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
			case tts::comma:
			case tts::full_stop:
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
			case tts::comma:
			case tts::full_stop:
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
				sequence.flush(mClause);
				state = mClause.empty() ? clause_state::start : clause_state::end;
				continue;
			case tts::exclamation:
			case tts::comma:
				mClause.push(event);
				break;
			case tts::full_stop:
				switch (event.codepoint)
				{
				case '.':
				case '-':
					if (sequence.codepoint() != event.codepoint)
					{
						sequence.flush(mClause);
						sequence.set_codepoint(event.codepoint);
					}
					sequence.add(event.range);
					break;
				default:
					sequence.flush(mClause);
					mClause.push({ event.text, event.type, event.range, event.codepoint });
				}
				break;
			case tts::punctuation:
			case tts::symbol:
				{
					tts::event_type type = punctuation_type(event.codepoint);
					if (type != tts::punctuation || event.codepoint == '-') switch (event.codepoint)
					{
					case '-':
						if (sequence.codepoint() != event.codepoint)
						{
							sequence.flush(mClause);
							sequence.set_codepoint(event.codepoint);
						}
						sequence.add(event.range);
						break;
					default:
						sequence.flush(mClause);
						mClause.push({ event.text, type, event.range, event.codepoint });
					}
				}
				break;
			case tts::paragraph:
				sequence.flush(mClause);
				mClause.push(event);
				break;
			};
			break;
		}
		mHaveEvent = mReader->read();
	}
	sequence.flush(mClause);
	return !mClause.empty();
}

std::shared_ptr<tts::text_reader> tts::context_analysis()
{
	return std::make_shared<context_analysis_t>();
}
