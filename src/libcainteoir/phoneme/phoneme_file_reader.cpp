/* Phoneme File Parser.
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
#include "i18n.h"
#include "compatibility.hpp"

#include "phonemeset.hpp"

#include <cainteoir/unicode.hpp>
#include <cainteoir/path.hpp>
#include <ucd/ucd.h>

namespace tts = cainteoir::tts;

enum class type : uint8_t
{
	unknown,
	hexadecimal,
	whitespace,
};

#define _ type::unknown
#define H type::hexadecimal
#define S type::whitespace

static const type classification[256] = {
	//////// x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF
	/* 0x */ _, _, _, _, _, _, _, _, _, S, S, _, _, S, _, _,
	/* 1x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 2x */ S, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 3x */ H, H, H, H, H, H, H, H, H, H, _, _, _, _, _, _,
	/* 4x */ _, H, H, H, H, H, H, _, _, _, _, _, _, _, _, _,
	/* 5x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 6x */ _, H, H, H, H, H, H, _, _, _, _, _, _, _, _, _,
	/* 7x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 8x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 9x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Ax */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Bx */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Cx */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Dx */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Ex */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Fx */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	//////// x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF
};

#undef _
#undef H
#undef S

static ucd::codepoint_t hex_to_unicode(const char * &current, const char *last)
{
	char escaped[9] = {0};
	char *end = escaped;
	while (current <= last && end <= end + 9 &&
	       classification[*current] == type::hexadecimal)
		*end++ = *current++;
	return strtoul(escaped, nullptr, 16);
}

tts::phoneme_file_reader::context_t::context_t(const std::string &aPhonemeSet)
{
	auto location = get_data_path() / "phonemeset" / (aPhonemeSet + ".phon");
	mBuffer = make_file_buffer(location);
	mCurrent = mBuffer->begin();
	mLast = mBuffer->end();
}

tts::phoneme_file_reader::phoneme_file_reader(const std::string &aPhonemeSet)
{
	mFiles.push({ aPhonemeSet });
	phoneme_type = "ipa";
}

bool tts::phoneme_file_reader::read()
{
	context_t *top = &mFiles.top();
	while (top)
	{
		if (top->mCurrent >= top->mLast)
		{
			mFiles.pop();
			top = mFiles.empty() ? nullptr : &mFiles.top();
			continue;
		}

		const char *start = nullptr;
		while (top->mCurrent <= top->mLast) switch (*top->mCurrent)
		{
		case '#':
			while (top->mCurrent <= top->mLast && (*top->mCurrent != '\n'))
				++top->mCurrent;
			break;
		case ' ': case '\t': case '\r': case '\n':
			++top->mCurrent;
			break;
		case '/':
			++top->mCurrent;
			start = top->mCurrent;
			while (top->mCurrent <= top->mLast && (*top->mCurrent != '/'))
				++top->mCurrent;
			if (top->mCurrent == top->mLast)
				throw std::runtime_error("unterminated phoneme group (expecting '/')");
			phonemes.clear();
			while (start < top->mCurrent)
			{
				auto ret = tts::read_explicit_feature(start, top->mCurrent);
				if (ret.first)
					phonemes.push_back(ret.second);
			}
			++top->mCurrent;
			return true;
		case '.':
			{
				const char *begin_entry = top->mCurrent;
				const char *end_entry   = top->mCurrent;
				while (end_entry != top->mLast &&
				       classification[*end_entry] != type::whitespace)
					++end_entry;

				top->mCurrent = end_entry;
				while (top->mCurrent != top->mLast && *top->mCurrent == '\t')
					++top->mCurrent;

				const char *begin_definition = top->mCurrent;
				const char *end_definition   = top->mCurrent;
				while (end_definition != top->mLast &&
				       classification[*end_definition] != type::whitespace)
					++end_definition;

				top->mCurrent = end_definition;
				while (top->mCurrent != top->mLast &&
				      (*top->mCurrent == '\r' || *top->mCurrent == '\n'))
					++top->mCurrent;

				std::string entry(begin_entry, end_entry);
				if (entry == ".import")
				{
					std::string definition(begin_definition, end_definition);
					mFiles.push({ definition });
					top = &mFiles.top();
				}
			}
			break;
		default:
			{
				char data[64];
				char *end = data;
				while (top->mCurrent <= top->mLast && end <= data + 60 &&
				       classification[*top->mCurrent] != type::whitespace)
				{
					if (*top->mCurrent == '\\')
					{
						++top->mCurrent;
						if (top->mCurrent <= top->mLast && *top->mCurrent == 'u')
						{
							++top->mCurrent;
							end = cainteoir::utf8::write(end, hex_to_unicode(top->mCurrent, top->mLast));
							continue;
						}
					}
					*end++ = *top->mCurrent++;
				}
				if (top->mCurrent <= top->mLast)
					transcription = cainteoir::make_buffer(data, end-data);
			}
			break;
		}
	}
	return false;
}
