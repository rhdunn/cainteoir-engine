/* CMU Pronunciation Dictionary Format.
 *
 * Copyright (C) 2015 Reece H. Dunn
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
#include "dictionary_format.hpp"

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;

struct cmudict_dictionary_reader : public tts::dictionary_reader
{
	cmudict_dictionary_reader(const char *aDictionaryPath, const char *aPhonemeSet);

	bool read();
private:
	std::shared_ptr<cainteoir::buffer> mData;
	const char *mCurrent;
	const char *mEnd;

	std::shared_ptr<tts::phoneme_reader> mPhonemeSet;
};

cmudict_dictionary_reader::cmudict_dictionary_reader(const char *aDictionaryPath, const char *aPhonemeSet)
	: mData(cainteoir::make_file_buffer(aDictionaryPath))
	, mPhonemeSet(tts::createPhonemeReader(aPhonemeSet))
{
	if (mData)
	{
		mCurrent = mData->begin();
		mEnd = mData->end();
	}
	else
	{
		mCurrent = nullptr;
		mEnd = nullptr;
	}
}

bool cmudict_dictionary_reader::read()
{
	enum state_t
	{
		start,
		new_comment1,
		new_comment2,
		comment,
		entry_word0,
		entry_word,
		entry_context,
		entry_pronunciation,
		entry_comment,
		have_entry,
	};

	state_t state = start;
	const char *word_start = nullptr;
	const char *word_end = nullptr;
	const char *context_start = nullptr;
	const char *context_end = nullptr;
	const char *pronunciation_start = nullptr;
	const char *pronunciation_end = nullptr;

	while (mCurrent != mEnd) switch (state)
	{
	case start:
		switch (*mCurrent)
		{
		case ';':
			state = new_comment1;
			++mCurrent;
			break;
		case ' ': case '\t': case '\r': case '\n':
			++mCurrent;
			break;
		default:
			state = entry_word0;
			word_start = mCurrent;
			break;
		};
		break;
	case new_comment1:
		switch (*mCurrent)
		{
		case ';':
			state = new_comment2;
			++mCurrent;
			break;
		case '\r': case '\n':
			state = start;
			++mCurrent;
			break;
		default:
			state = entry_word0;
			word_start = mCurrent - 1;
			break;
		};
		break;
	case new_comment2:
		switch (*mCurrent)
		{
		case ';':
			state = comment;
			++mCurrent;
			break;
		case '\r': case '\n':
			state = start;
			++mCurrent;
			break;
		default:
			state = entry_word0;
			word_start = mCurrent - 2;
			break;
		};
		break;
	case comment:
	case entry_comment:
		switch (*mCurrent)
		{
		case '\r': case '\n':
			state = (state == comment) ? start : have_entry;
			++mCurrent;
			break;
		default:
			++mCurrent;
			break;
		};
		break;
	case entry_word0:
		switch (*mCurrent)
		{
		case ' ': case '\t': case '\r': case '\n':
			state = start;
			++mCurrent;
			break;
		default:
			state = entry_word;
			++mCurrent;
			break;
		};
		break;
	case entry_word:
		switch (*mCurrent)
		{
		case ' ': case '\t':
			state = entry_pronunciation;
			word_end = mCurrent;
			++mCurrent;
			pronunciation_start = mCurrent;
			break;
		case '(':
			state = entry_context;
			word_end = mCurrent;
			++mCurrent;
			context_start = mCurrent;
			break;
		case '\r': case '\n':
			state = start;
			++mCurrent;
			break;
		default:
			++mCurrent;
			break;
		};
		break;
	case entry_context:
		switch (*mCurrent)
		{
		case ')':
			state = entry_pronunciation;
			context_end = mCurrent;
			++mCurrent;
			pronunciation_start = mCurrent;
			break;
		case '\r': case '\n':
			state = start;
			++mCurrent;
			break;
		default:
			++mCurrent;
			break;
		};
		break;
	case entry_pronunciation:
		switch (*mCurrent)
		{
		case '\r': case '\n':
			state = have_entry;
			pronunciation_end = mCurrent;
			++mCurrent;
			break;
		case '#':
			state = entry_comment;
			pronunciation_end = mCurrent;
			++mCurrent;
			break;
		default:
			++mCurrent;
			break;
		};
		break;
	case have_entry:
		{
			char data[512] = { 0 };
			char *ptr = data;
			for (auto c : cainteoir::buffer(word_start, word_end))
			{
				*ptr++ = tolower(c);
			}
			if (context_start != nullptr)
			{
				*ptr++ = '@';
				for (auto c : cainteoir::buffer(context_start, context_end))
					*ptr++ = c;
			}

			word = cainteoir::make_buffer(data, ptr - data);
			auto pronunciation = std::make_shared<cainteoir::buffer>(pronunciation_start, pronunciation_end);

			entry = { pronunciation, mPhonemeSet };

			return true;
		}
		break;
	}
	return false;
}

std::shared_ptr<tts::dictionary_reader> tts::createCMUDictionaryReader(const char *aDictionaryPath, const char *aPreferredPhonemeSet)
{
	return std::make_shared<cmudict_dictionary_reader>(aDictionaryPath, aPreferredPhonemeSet ? aPreferredPhonemeSet : "cmu");
}

struct cmudict_formatter : public tts::dictionary_formatter
{
	cmudict_formatter(FILE *aOut)
		: mOut(aOut)
	{
	}

	void write_phoneme_entry(const std::shared_ptr<cainteoir::buffer> &word,
	                         std::shared_ptr<tts::phoneme_writer> &writer,
	                         const ipa::phonemes &phonemes,
	                         const char *line_separator);

	void write_say_as_entry(const std::shared_ptr<cainteoir::buffer> &word,
	                        const std::shared_ptr<cainteoir::buffer> &say_as,
	                        const char *line_separator);

	FILE *mOut;
};

void cmudict_formatter::write_phoneme_entry(const std::shared_ptr<cainteoir::buffer> &word,
                                           std::shared_ptr<tts::phoneme_writer> &writer,
                                           const ipa::phonemes &phonemes,
                                           const char *line_separator)
{
	bool in_variant = false;
	for (auto c : *word)
	{
		if (in_variant)
			fputc(c, mOut);
		else if (c == '@')
		{
			in_variant = true;
			fputc('(', mOut);
		}
		else
			fputc(toupper(c), mOut);
	}
	if (in_variant)
		fputc(')', mOut);
	fputc(' ', mOut);
	fputc(' ', mOut);
	for (auto p : phonemes)
		writer->write(p);
	writer->flush();
	fputc('\n', mOut);
}

void cmudict_formatter::write_say_as_entry(const std::shared_ptr<cainteoir::buffer> &word,
                                          const std::shared_ptr<cainteoir::buffer> &say_as,
                                          const char *line_separator)
{
	throw std::runtime_error("cmudict does not support say-as entries");
}

std::shared_ptr<tts::dictionary_formatter> tts::createCMUDictionaryFormatter(FILE *out)
{
	return std::make_shared<cmudict_formatter>(out);
}
