/* Test for extracting words, numbers and other entries from a document.
 *
 * Copyright (C) 2012-2013 Reece H. Dunn
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

#include <cainteoir/document.hpp>
#include <cainteoir/unicode.hpp>
#include <ucd/ucd.h>

#include <stdexcept>
#include <cstdio>

namespace rdf    = cainteoir::rdf;
namespace events = cainteoir::events;

struct text_reader
{
	enum token_type
	{
		error,
		word_uppercase,
		word_lowercase,
		word_capitalized,
		word_mixedcase,
		number,
		punctuation,
		symbol,
	};

	text_reader();

	const cainteoir::buffer &match() const { return mMatch; }

	token_type type() const { return mType; }

	void set_buffer(const std::shared_ptr<cainteoir::buffer> &aBuffer);

	bool read();
private:
	cainteoir::buffer mMatch;
	token_type mType;

	const char *mStart;
	const char *mCurrent;
	const char *mLast;
	uint8_t mState;
};

static const char *token_name[] = {
	"error",
	"word",
	"word",
	"word",
	"word",
	"number",
	"punctuation",
	"symbol",
};

enum class state
{
	start,
	upper_case_initial,
	upper_case,
	capitalized,
	lower_case,
	mixed_case,
	number,
	punctuation,
	symbol
};

static const bool state_is_terminal[] = {
	false, // 0 = start
	true,  // 1 = upper case (initial character)
	true,  // 2 = upper case
	true,  // 3 = capitalized
	true,  // 4 = lower case
	true,  // 5 = mixed case
	true,  // 6 = number
	true,  // 7 = punctuation
	true,  // 8 = symbol
};

static const text_reader::token_type state_token[] = {
	text_reader::error,
	text_reader::word_uppercase,
	text_reader::word_uppercase,
	text_reader::word_capitalized,
	text_reader::word_lowercase,
	text_reader::word_mixedcase,
	text_reader::number,
	text_reader::punctuation,
	text_reader::symbol,
};

static const uint8_t state_transitions[][31] = {
	// Cc Cf Cn Co Cs Ii Ll Lm Lo Lt Lu Mc Me Mn Nd Nl No Pc Pd Pe Pf Pi Po Ps Sc Sk Sm So Zl Zp Zs
	{  0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 1, 0, 0, 0, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 0, 0, 0 }, // 0
	{  0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 1
	{  0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 2
	{  0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 3
	{  0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 4
	{  0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 5
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 6
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 7
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 8
};

text_reader::text_reader()
	: mMatch(nullptr, nullptr)
	, mType(error)
	, mStart(nullptr)
	, mCurrent(nullptr)
	, mLast(nullptr)
	, mState(0)
{
}

void text_reader::set_buffer(const std::shared_ptr<cainteoir::buffer> &aBuffer)
{
	mStart = mCurrent = aBuffer->begin();
	mLast = aBuffer->end();
}

bool text_reader::read()
{
	uint32_t cp = 0;
	const char *next = nullptr;
	mState = 0;
	for (; (next = cainteoir::utf8::read(mCurrent, cp)) <= mLast; mCurrent = next)
	{
		ucd::category category = ucd::lookup_category(cp);
		ucd::script   script   = ucd::lookup_script(cp);

		uint8_t new_state = state_transitions[mState][category];
		if (cp == '\'') switch ((state)mState)
		{
		case state::upper_case_initial:
		case state::upper_case:
		case state::capitalized:
		case state::lower_case:
		case state::mixed_case:
			// Don't transition states here.
			new_state = mState;
			break;
		}

		if (state_is_terminal[mState] && !state_is_terminal[new_state])
		{
			mType = state_token[mState];
			mMatch = cainteoir::buffer(mStart, mCurrent);
			mStart = mCurrent;
			return true;
		}

		mState = new_state;
		if (mState == 0)
			mStart = next;
	}

	return false;
}

int main(int argc, char ** argv)
{
	try
	{
		argc -= 1;
		argv += 1;

		if (argc == 0)
			throw std::runtime_error("no document specified");

		rdf::graph metadata;
		auto reader = cainteoir::createDocumentReader(argv[0], metadata, std::string());
		if (!reader)
		{
			fprintf(stderr, "unsupported document format for file \"%s\"\n", argv[0]);
			return 0;
		}

		text_reader text;
		while (reader->read())
		{
			if (reader->type & cainteoir::events::text)
			{
				text.set_buffer(reader->text);
				while (text.read())
				{
					fprintf(stdout, ".%-13s %s\n",
					        token_name[text.type()],
					        text.match().str().c_str());
				}
			}
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
