/* Phoneme Set Reader/Writer.
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
#include "i18n.h"
#include "compatibility.hpp"

#include <cainteoir/phoneme.hpp>
#include <cainteoir/path.hpp>
#include <cainteoir/unicode.hpp>
#include <cainteoir/trie.hpp>
#include <utility>
#include <stdlib.h>

namespace tts = cainteoir::tts;

#define _ 0

static const uint8_t hex_digit[256] = {
	//////// x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF
	/* 0x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 1x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 2x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 3x */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, _, _, _, _, _, _,
	/* 4x */ _, 1, 1, 1, 1, 1, 1, _, _, _, _, _, _, _, _, _,
	/* 5x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 6x */ _, 1, 1, 1, 1, 1, 1, _, _, _, _, _, _, _, _, _,
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

enum class modifier_placement
{
	none,
	before,
	after,
	combining,
	phoneme,
};

struct phoneme_file_reader
{
	std::shared_ptr<cainteoir::buffer> transcription;
	tts::phoneme phoneme;
	tts::feature feature;
	modifier_placement placement;

	phoneme_file_reader(const char *aPhonemeSet);

	bool read();
private:
	std::shared_ptr<cainteoir::buffer> mBuffer;
	std::shared_ptr<tts::phoneme_reader> features;
	const char *current;
	const char *last;
};

phoneme_file_reader::phoneme_file_reader(const char *aPhonemeSet)
	: phoneme(tts::feature::unspecified, tts::feature::unspecified, tts::feature::unspecified)
	, features(tts::createExplicitFeaturePhonemeReader())
	, feature(tts::feature::unspecified)
	, placement(modifier_placement::before)
{
	auto location = cainteoir::get_data_path() / "phonemeset" / (std::string(aPhonemeSet) + ".phon");
	mBuffer = cainteoir::make_file_buffer(location);
	current = mBuffer->begin();
	last    = mBuffer->end();
}

bool phoneme_file_reader::read()
{
	const char *start = nullptr;
	while (current <= last)
	{
		switch (*current)
		{
		case '#':
			while (current <= last && (*current != '\n'))
				++current;
			break;
		case ' ': case '\t':
			++current;
			break;
		case '\r': case '\n':
			++current;
			break;
		case '/':
			++current;
			start = current;
			while (current <= last && (*current != '/'))
				++current;
			if (*current != '/')
				throw std::runtime_error("unterminated phoneme group (expecting '/')");
			if (*start == '<' && *(current - 1) == '>')
			{
				if (current - start != 5)
					throw std::runtime_error("unexpected feature length in phoneme modification rule (expected 3 characters)");
				++current;

				char abbreviation[] = { start[1], start[2], start[3], 0 };
				auto f = tts::get_feature_id(abbreviation);
				if (f.second == tts::feature::unspecified)
					throw std::runtime_error("unrecognised feature in phoneme modification rule");

				feature = f.second;
				if (*current == '<')
				{
					++current;
					placement = modifier_placement::before;
				}
				else if (*current == '>')
				{
					++current;
					placement = modifier_placement::after;
				}
				else if (*current == '+')
				{
					++current;
					placement = modifier_placement::combining;
				}
				else
					throw std::runtime_error("unrecognised modifier placement in phoneme modification rule");
				return true;
			}
			else
			{
				features->reset(std::make_shared<cainteoir::buffer>(start, current));
				++current;
				if (features->read())
				{
					feature = tts::feature::unspecified;
					phoneme = *features;
					return true;
				}
			}
			break;
		default:
			{
				char data[64];
				char *end = data;
				start = data;
				while (current <= last && data <= data + 60 && (*current != ' ' && *current != '\t' && *current != '\r' && *current != '\n'))
				{
					if (current <= (last - 6) &&
					    current[0] == '\\' &&
					    current[1] == 'u' &&
					    hex_digit[current[2]] &&
					    hex_digit[current[3]] &&
					    hex_digit[current[4]] &&
					    hex_digit[current[5]])
					{
						current += 2;
						char escape[5] = { 0 };
						escape[0] = *current++;
						escape[1] = *current++;
						escape[2] = *current++;
						escape[3] = *current++;
						end = cainteoir::utf8::write(end, strtol(escape, nullptr, 16));
					}
					else if (current[0] == '\\' && current[1] == '#')
					{
						current += 2;
						*end++ = '#';
					}
					else
						*end++ = *current++;
				}
				if (current <= last)
				{
					transcription = cainteoir::make_buffer(data, end-data);
					++current;
				}
			}
			break;
		}
	}
	return false;
}

struct phonemeset_reader: public tts::phoneme_reader
{
	phonemeset_reader(const char *aPhonemeSet);

	void reset(const std::shared_ptr<cainteoir::buffer> &aBuffer);

	bool read();

	std::pair<const char *, std::pair<modifier_placement, tts::phoneme>>
	next_match();

	cainteoir::trie<std::pair<modifier_placement, tts::phoneme>> mPhonemes;
	std::shared_ptr<cainteoir::buffer> mBuffer;
	const char *mCurrent;
	const char *mEnd;
};

phonemeset_reader::phonemeset_reader(const char *aPhonemeSet)
{
	auto reader = phoneme_file_reader(aPhonemeSet);
	while (reader.read())
	{
		if (reader.feature == tts::feature::unspecified)
			mPhonemes.insert(*reader.transcription,
			                 { modifier_placement::phoneme, reader.phoneme });
		else
			mPhonemes.insert(*reader.transcription,
			                 { reader.placement, { reader.feature, tts::feature::unspecified, tts::feature::unspecified } });
	}
}

void phonemeset_reader::reset(const std::shared_ptr<cainteoir::buffer> &aBuffer)
{
	mBuffer = aBuffer;
	if (mBuffer.get())
	{
		mCurrent = mBuffer->begin();
		mEnd = mBuffer->end();
	}
	else
	{
		mCurrent = mEnd = nullptr;
	}
}

bool phonemeset_reader::read()
{
	modifier_placement state = modifier_placement::before;
	const char *pos = mCurrent;
	tts::phoneme p;
	while (true)
	{
		auto m = next_match();
		switch (state)
		{
		case modifier_placement::before:
			switch (m.second.first)
			{
			case modifier_placement::before:
				p.add(*m.second.second.begin());
				break;
			case modifier_placement::phoneme:
				for (tts::feature f : m.second.second)
					p.add(f);
				state = modifier_placement::after;
				pos = m.first;
				break;
			case modifier_placement::after:
			case modifier_placement::combining:
				fflush(stdout);
				fprintf(stderr, "\nno phoneme before post-phoneme modifiers\n");
				fflush(stderr);
				break;
			case modifier_placement::none:
				return false;
			}
			break;
		case modifier_placement::after:
			switch (m.second.first)
			{
			case modifier_placement::before:
			case modifier_placement::phoneme:
			case modifier_placement::none:
				mCurrent = pos;
				*(tts::phoneme *)this = p;
				return true;
			case modifier_placement::combining:
			case modifier_placement::after:
				p.add(*m.second.second.begin());
				pos = m.first;
				break;
			}
			break;
		}
	}
	return false;
}

std::pair<const char *, std::pair<modifier_placement, tts::phoneme>>
phonemeset_reader::next_match()
{
	const auto *entry = mPhonemes.root();
	decltype(mPhonemes.root()) match = nullptr;
	const char *pos = mCurrent;
	while (mCurrent < mEnd)
	{
		const auto *next = entry->get(*mCurrent);
		if (next == nullptr)
		{
			if (match)
			{
				*(tts::phoneme *)this = match->item.second;
				mCurrent = pos;
				return { mCurrent, match->item };
			}

			uint8_t c = *mCurrent;
			fflush(stdout);
			if (c <= 0x20 || c >= 0x80)
				fprintf(stderr, "\nunrecognised character 0x%02X\n", c);
			else
				fprintf(stderr, "\nunrecognised character '%c'\n", c);
			fflush(stderr);

			entry = mPhonemes.root();
			++mCurrent;
		}
		else
		{
			entry = next;
			++mCurrent;
			if (entry->item.first != modifier_placement::none)
			{
				match = entry;
				pos = mCurrent;
			}
		}
	}
	if (match)
	{
		*(tts::phoneme *)this = match->item.second;
		mCurrent = pos;
		return { mCurrent, match->item };
	}
	return { mCurrent, { modifier_placement::none, {} } };
}

std::shared_ptr<tts::phoneme_reader> tts::createPhonemeReader(const char *aPhonemeSet)
{
	if (!strcmp(aPhonemeSet, "features"))
		return tts::createExplicitFeaturePhonemeReader();
	return std::make_shared<phonemeset_reader>(aPhonemeSet);
}

struct phonemeset_writer: public tts::phoneme_writer
{
	std::list<std::pair<tts::phoneme, std::shared_ptr<cainteoir::buffer>>> data;
	std::list<std::pair<tts::feature, std::pair<modifier_placement, std::shared_ptr<cainteoir::buffer>>>> rule_data;
	FILE *output;

	phonemeset_writer(const char *aPhonemeSet);

	void reset(FILE *aOutput);

	bool write(const tts::phoneme &aPhoneme);
};

phonemeset_writer::phonemeset_writer(const char *aPhonemeSet)
{
	auto reader = phoneme_file_reader(aPhonemeSet);
	while (reader.read())
	{
		if (reader.feature == tts::feature::unspecified)
			data.push_back({ reader.phoneme, reader.transcription });
		else
			rule_data.push_back({ reader.feature, { reader.placement, reader.transcription }});
	}
}

void phonemeset_writer::reset(FILE *aOutput)
{
	output = aOutput;
}

bool phonemeset_writer::write(const tts::phoneme &aPhoneme)
{
	tts::phoneme phoneme(aPhoneme);
	tts::phoneme phoneme_combining(aPhoneme);

	std::shared_ptr<cainteoir::buffer> before;
	std::shared_ptr<cainteoir::buffer> combining;
	std::shared_ptr<cainteoir::buffer> after;

	for (const auto &entry : rule_data)
	{
		if (phoneme_combining.remove(entry.first))
		{
			switch (entry.second.first)
			{
			case modifier_placement::before:
				before = entry.second.second;
				phoneme.remove(entry.first);
				break;
			case modifier_placement::after:
				after = entry.second.second;
				phoneme.remove(entry.first);
				break;
			case modifier_placement::combining:
				combining = entry.second.second;
				break;
			}
		}
	}

	for (const auto &entry : data)
	{
		if (entry.first == aPhoneme)
		{
			fwrite(entry.second->begin(), 1, entry.second->size(), output);
			return true;
		}
		else if (entry.first == phoneme)
		{
			if (before.get())
				fwrite(before->begin(), 1, before->size(), output);
			fwrite(entry.second->begin(), 1, entry.second->size(), output);
			if (after.get())
				fwrite(after->begin(), 1, after->size(), output);
			return true;
		}
	}

	for (const auto &entry : data)
	{
		if (entry.first == phoneme_combining)
		{
			if (before.get())
				fwrite(before->begin(), 1, before->size(), output);
			fwrite(entry.second->begin(), 1, entry.second->size(), output);
			if (combining.get())
				fwrite(combining->begin(), 1, combining->size(), output);
			if (after.get())
				fwrite(after->begin(), 1, after->size(), output);
			return true;
		}
	}

	return false;
}

std::shared_ptr<tts::phoneme_writer> tts::createPhonemeWriter(const char *aPhonemeSet)
{
	if (!strcmp(aPhonemeSet, "features"))
		return tts::createExplicitFeaturePhonemeWriter();
	return std::make_shared<phonemeset_writer>(aPhonemeSet);
}
