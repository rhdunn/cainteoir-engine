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

struct cmudict_parser
{
	enum type_t
	{
		unknown,
		line_comment,
		entry,
		context,
		pronunciation,
		comment,
	};

	cmudict_parser(const char *aDictionaryPath);

	bool read();

	type_t type() const { return mType; }

	cainteoir::buffer match() const { return { mMatchBegin, mMatchEnd }; }
private:
	enum class state_t
	{
		start,
		line_comment1,
		line_comment2,
		entry_comment,
		comment,
		entry,
		context1,
		context2,
		pronunciation1,
		pronunciation2,
	};

	std::shared_ptr<cainteoir::buffer> mData;
	const char *mCurrent;
	const char *mEnd;
	state_t mState;

	type_t mType;
	const char *mMatchBegin;
	const char *mMatchEnd;
};

cmudict_parser::cmudict_parser(const char *aDictionaryPath)
	: mData(cainteoir::make_file_buffer(aDictionaryPath))
	, mState(state_t::start)
	, mType(unknown)
	, mMatchBegin(nullptr)
	, mMatchEnd(nullptr)
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

bool cmudict_parser::read()
{
	while (mCurrent != mEnd) switch (mState)
	{
	case state_t::start:
		switch (*mCurrent)
		{
		case ';':
			mState = state_t::line_comment1;
			++mCurrent;
			break;
		case ' ': case '\t': case '\r': case '\n':
			++mCurrent;
			break;
		default:
			mState = state_t::entry;
			mMatchBegin = mCurrent;
			mType = entry;
			++mCurrent;
			break;
		};
		break;
	case state_t::line_comment1:
		switch (*mCurrent)
		{
		case ';':
			mState = state_t::line_comment2;
			++mCurrent;
			break;
		case '\r': case '\n':
			mState = state_t::start;
			++mCurrent;
			break;
		default:
			mState = state_t::entry;
			mMatchBegin = mCurrent - 1;
			mType = entry;
			++mCurrent;
			break;
		};
		break;
	case state_t::line_comment2:
		switch (*mCurrent)
		{
		case ';':
			mState = state_t::comment;
			++mCurrent;
			mMatchBegin = mCurrent;
			mType = line_comment;
			break;
		case '\r': case '\n':
			mState = state_t::start;
			++mCurrent;
			break;
		default:
			mState = state_t::entry;
			mMatchBegin = mCurrent - 2;
			mType = entry;
			++mCurrent;
			break;
		};
		break;
	case state_t::entry_comment:
		mState = state_t::comment;
		mMatchBegin = mCurrent;
		mType = comment;
		break;
	case state_t::comment:
		switch (*mCurrent)
		{
		case '\r': case '\n':
			mState = state_t::start;
			mMatchEnd = mCurrent;
			++mCurrent;
			return true;
		default:
			++mCurrent;
			break;
		};
		break;
	case state_t::entry:
		switch (*mCurrent)
		{
		case ' ': case '\t':
			mState = state_t::pronunciation1;
			mMatchEnd = mCurrent;
			++mCurrent;
			return true;
		case '(':
			mState = state_t::context1;
			mMatchEnd = mCurrent;
			++mCurrent;
			return true;
		case '\r': case '\n':
			mState = state_t::start;
			mMatchEnd = mCurrent;
			++mCurrent;
			return true;
		default:
			++mCurrent;
			break;
		};
		break;
	case state_t::context1:
		mState = state_t::context2;
		mMatchBegin = mCurrent;
		mType = context;
		break;
	case state_t::context2:
		switch (*mCurrent)
		{
		case ')':
			mState = state_t::pronunciation1;
			mMatchEnd = mCurrent;
			++mCurrent;
			return true;
		case '\r': case '\n':
			mState = state_t::start;
			++mCurrent;
			break;
		default:
			++mCurrent;
			break;
		};
		break;
	case state_t::pronunciation1:
		mState = state_t::pronunciation2;
		mMatchBegin = mCurrent;
		mType = pronunciation;
		break;
	case state_t::pronunciation2:
		switch (*mCurrent)
		{
		case '\r': case '\n':
			mState = state_t::start;
			mMatchEnd = mCurrent;
			++mCurrent;
			return true;
		case '#':
			mState = state_t::entry_comment;
			mMatchEnd = mCurrent;
			++mCurrent;
			return true;
		default:
			++mCurrent;
			break;
		};
		break;
	};
	return false;
}

struct cmudict_dictionary_reader : public tts::dictionary_reader
{
	cmudict_dictionary_reader(const char *aDictionaryPath, const char *aPhonemeSet);

	bool read();
private:
	cmudict_parser mReader;
	std::shared_ptr<tts::phoneme_reader> mPhonemeSet;
};

cmudict_dictionary_reader::cmudict_dictionary_reader(const char *aDictionaryPath, const char *aPhonemeSet)
	: mReader(aDictionaryPath)
	, mPhonemeSet(tts::createPhonemeReader(aPhonemeSet))
{
}

bool cmudict_dictionary_reader::read()
{
	enum class state_t
	{
		start,
		entry,
		context,
		pronunciation,
	};

	state_t state = state_t::start;
	cainteoir::buffer entry_word(nullptr, nullptr);
	cainteoir::buffer context(nullptr, nullptr);
	cainteoir::buffer pronunciation(nullptr, nullptr);

	while (true) switch (state)
	{
	case state_t::start:
		if (!mReader.read())
			return false;
		switch (mReader.type())
		{
		case cmudict_parser::entry:
			entry_word = mReader.match();
			state = state_t::entry;
			break;
		default: // Ignore line comments and errors ...
			break;
		}
		break;
	case state_t::entry:
		if (!mReader.read())
			return false;
		switch (mReader.type())
		{
		case cmudict_parser::context:
			context = mReader.match();
			state = state_t::context;
			break;
		case cmudict_parser::pronunciation:
			pronunciation = mReader.match();
			state = state_t::pronunciation;
			break;
		case cmudict_parser::entry:
			entry_word = mReader.match();
			state = state_t::entry;
			break;
		default: // Invalid entry ... reset
			state = state_t::start;
			break;
		}
		break;
	case state_t::context:
		if (!mReader.read())
			return false;
		switch (mReader.type())
		{
		case cmudict_parser::pronunciation:
			pronunciation = mReader.match();
			if (pronunciation.empty())
			{
				state = state_t::start;
				context = { nullptr, nullptr };
			}
			else
				state = state_t::pronunciation;
			break;
		default: // Invalid entry ... reset
			state = state_t::start;
			context = { nullptr, nullptr };
			break;
		}
		break;
	case state_t::pronunciation:
		{
			char data[512] = { 0 };
			char *ptr = data;
			for (auto c : entry_word)
			{
				*ptr++ = tolower(c);
			}
			if (!context.empty())
			{
				*ptr++ = '@';
				for (auto c : context)
					*ptr++ = c;
			}

			std::shared_ptr<cainteoir::buffer> phonemes = std::make_shared<cainteoir::buffer>(pronunciation.begin(), pronunciation.end());

			word = cainteoir::make_buffer(data, ptr - data);
			entry = { phonemes, mPhonemeSet };
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
