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
#include <utility>

namespace tts = cainteoir::tts;

struct phoneme_file_reader
{
	std::shared_ptr<cainteoir::buffer> transcription;
	tts::phoneme phoneme;

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
			features->reset(std::make_shared<cainteoir::buffer>(start, current));
			++current;
			if (features->read())
			{
				phoneme = *features;
				return true;
			}
			break;
		default:
			start = current;
			while (current <= last && (*current != ' ' && *current != '\t' && *current != '\r' && *current != '\n'))
				++current;
			if (current <= last)
			{
				transcription = cainteoir::make_buffer(start, current-start);
				++current;
			}
			break;
		}
	}
	return false;
}

struct phonemeset_writer: public tts::phoneme_writer
{
	std::list<std::pair<tts::phoneme, std::shared_ptr<cainteoir::buffer>>> data;
	FILE *output;

	phonemeset_writer(const char *aPhonemeSet);

	void reset(FILE *aOutput);

	bool write(const tts::phoneme &aPhoneme);
};

phonemeset_writer::phonemeset_writer(const char *aPhonemeSet)
{
	auto reader = phoneme_file_reader(aPhonemeSet);
	while (reader.read())
		data.push_back({ reader.phoneme, reader.transcription });
}

void phonemeset_writer::reset(FILE *aOutput)
{
	output = aOutput;
}

bool phonemeset_writer::write(const tts::phoneme &aPhoneme)
{
	for (const auto &entry : data)
	{
		if (entry.first == aPhoneme)
		{
			fwrite(entry.second->begin(), 1, entry.second->size(), output);
			return true;
		}
	}
	return false;
}

std::shared_ptr<tts::phoneme_reader> tts::createPhonemeReader(const char *aPhonemeSet)
{
	if (!strcmp(aPhonemeSet, "features"))
		return tts::createExplicitFeaturePhonemeReader();
	throw std::runtime_error("unsupported phoneme set");
}

std::shared_ptr<tts::phoneme_writer> tts::createPhonemeWriter(const char *aPhonemeSet)
{
	if (!strcmp(aPhonemeSet, "features"))
		return tts::createExplicitFeaturePhonemeWriter();
	return std::make_shared<phonemeset_writer>(aPhonemeSet);
}
