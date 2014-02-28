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

tts::phoneme_file_reader::phoneme_file_reader(const std::string &aPhonemeSet)
	: mFeatures(createExplicitFeaturePhonemeReader())
{
	auto location = get_data_path() / "phonemeset" / (aPhonemeSet + ".phon");
	mBuffer = make_file_buffer(location);
	mCurrent = mBuffer->begin();
	mLast = mBuffer->end();

	phoneme_type = "ipa";
}

bool tts::phoneme_file_reader::read()
{
	const char *start = nullptr;
	while (mCurrent <= mLast) switch (*mCurrent)
	{
	case '#':
		while (mCurrent <= mLast && (*mCurrent != '\n'))
			++mCurrent;
		break;
	case ' ': case '\t': case '\r': case '\n':
		++mCurrent;
		break;
	case '/':
		++mCurrent;
		start = mCurrent;
		while (mCurrent <= mLast && (*mCurrent != '/'))
			++mCurrent;
		if (mCurrent == mLast)
			throw std::runtime_error("unterminated phoneme group (expecting '/')");
		mFeatures->reset(std::make_shared<buffer>(start, mCurrent));
		phonemes.clear();
		while (mFeatures->read())
			phonemes.push_back(*mFeatures);
		++mCurrent;
		return true;
	default:
		{
			char data[64];
			char *end = data;
			while (mCurrent <= mLast && end <= data + 60 &&
			       classification[*mCurrent] != type::whitespace)
			{
				if (*mCurrent == '\\')
				{
					++mCurrent;
					if (mCurrent <= mLast && *mCurrent == 'u')
					{
						++mCurrent;
						end = cainteoir::utf8::write(end, hex_to_unicode(mCurrent, mLast));
						continue;
					}
				}
				*end++ = *mCurrent++;
			}
			if (mCurrent <= mLast)
				transcription = cainteoir::make_buffer(data, end-data);
		}
		break;
	}
	return false;
}
