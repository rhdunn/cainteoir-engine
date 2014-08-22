/* Lexer for Cainteoir file formats.
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

#include "cainteoir_file_reader.hpp"

enum state_t
{
	start,
	in_line_contents,
	in_comment,
	in_directive,
	pre_directive_body,
	in_directive_body,
	in_text,
	in_phonemes,
	in_directive_string,
	in_string,
};

cainteoir_file_reader::cainteoir_file_reader(const cainteoir::path &aFilePath)
	: mData(cainteoir::make_file_buffer(aFilePath))
	, mState(start)
	, mType(end_of_file)
	, mMatch(nullptr, nullptr)
{
	mCurrent = mData->begin();
	mLast = mData->end();
}

bool cainteoir_file_reader::read()
{
	const char *begin_match = nullptr;
	while (mCurrent != mLast) switch (mState)
	{
	case start:
		switch (*mCurrent)
		{
		case '\r': case '\n': case '\t': case ' ':
			++mCurrent;
			break;
		case '#':
			mState = in_comment;
			break;
		case '.':
			begin_match = mCurrent;
			mState = in_directive;
			break;
		case '/':
			++mCurrent;
			begin_match = mCurrent;
			mState = in_phonemes;
			break;
		case '\"':
			++mCurrent;
			begin_match = mCurrent;
			mState = in_string;
			break;
		default:
			begin_match = mCurrent;
			mState = in_text;
			break;
		}
		break;
	case in_line_contents:
		switch (*mCurrent)
		{
		case '\r': case '\n': case '#':
			mState = start;
			break;
		case '\t': case ' ':
			++mCurrent;
			break;
		case '/':
			++mCurrent;
			begin_match = mCurrent;
			mState = in_phonemes;
			break;
		case '\"':
			++mCurrent;
			begin_match = mCurrent;
			mState = in_string;
			break;
		default:
			begin_match = mCurrent;
			mState = in_text;
			break;
		}
		break;
	case in_comment:
		switch (*mCurrent)
		{
		case '\r': case '\n':
			mState = start;
			break;
		default:
			++mCurrent;
			break;
		}
		break;
	case in_directive:
		switch (*mCurrent)
		{
		case '\r': case '\n': case '#':
			fprintf(stderr, "error: missing directive body\n");
			mState = start;
			break;
		case '\t':
			mState = pre_directive_body;
			mMatch = cainteoir::buffer(begin_match, mCurrent);
			mType = directive;
			return true;
		default:
			++mCurrent;
			break;
		}
		break;
	case pre_directive_body:
		switch (*mCurrent)
		{
		case '\r': case '\n': case '#':
			fprintf(stderr, "error: missing directive body\n");
			mState = start;
			break;
		case '\t':
			++mCurrent;
			break;
		case '\"':
			++mCurrent;
			begin_match = mCurrent;
			mState = in_directive_string;
			break;
		default:
			begin_match = mCurrent;
			mState = in_directive_body;
			break;
		}
		break;
	case in_directive_body:
		switch (*mCurrent)
		{
		case '\r': case '\n': case '#':
			// skip any trailing whitespace ...
			--mCurrent;
			while (mCurrent != begin_match && (*mCurrent == ' '))
				--mCurrent;
			++mCurrent;
			// return the matching token ...
			mState = start;
			mMatch = cainteoir::buffer(begin_match, mCurrent);
			mType = directive_contents;
			return true;
		default:
			++mCurrent;
			break;
		}
		break;
	case in_phonemes:
		switch (*mCurrent)
		{
		case '\r': case '\n': case '\t': case '#':
			fprintf(stderr, "error: missing end of phoneme block (missing closing '/')\n");
			mState = start;
			break;
		case '/':
			mState = in_line_contents;
			mMatch = cainteoir::buffer(begin_match, mCurrent);
			mType = phonemes;
			++mCurrent;
			return true;
		default:
			++mCurrent;
			break;
		}
		break;
	case in_text:
		switch (*mCurrent)
		{
		case '\r': case '\n': case '\t': case ' ': case '#':
			mState = (*mCurrent == '\t') ? in_line_contents : start;
			mMatch = cainteoir::buffer(begin_match, mCurrent);
			mType = text;
			return true;
		default:
			++mCurrent;
			break;
		}
		break;
	case in_directive_string:
	case in_string:
		switch (*mCurrent)
		{
		case '\r': case '\n': case '\t':
			fprintf(stderr, "error: missing end of string (missing closing '\"')\n");
			mState = start;
			break;
		case '\"':
			mType = mState == in_string ? string : directive_contents;
			mState = mState == in_string ? in_line_contents : start;
			mMatch = cainteoir::buffer(begin_match, mCurrent);
			++mCurrent;
			return true;
		default:
			++mCurrent;
			break;
		}
		break;
	}

	mType = end_of_file;
	return false;
}
