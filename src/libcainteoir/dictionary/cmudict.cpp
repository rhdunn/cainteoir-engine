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

#include <cainteoir/unicode.hpp>
#include <ucd/ucd.h>

namespace tts  = cainteoir::tts;
namespace ipa  = cainteoir::ipa;
namespace utf8 = cainteoir::utf8;

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

bool
cmudict_parser::read()
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

static std::pair<cainteoir::buffer, cainteoir::buffer>
read_key_value_pair(const char * &first, const char *last)
{
	if (first == last)
		return { {nullptr, nullptr}, {nullptr, nullptr} };

	while (first != last && (*first == ' ' || *first == '\t'))
		++first;

	const char *key_first = first;
	while (first != last && *first != '=')
		++first;

	if (first == last)
		return { {nullptr, nullptr}, {nullptr, nullptr} };

	cainteoir::buffer key{ key_first, first };

	++first;

	const char *value_first = first;
	while (first != last && !(*first == ' ' || *first == '\t'))
		++first;

	return { key, { value_first, first } };
}

struct cmudict_dictionary_reader : public tts::dictionary_reader
{
	cmudict_dictionary_reader(const char *aDictionaryPath);

	bool read();
private:
	void parse_line_metadata(cainteoir::buffer comment);

	cmudict_parser mReader;
	std::shared_ptr<tts::phoneme_reader> mPhonemeSet;
};

cmudict_dictionary_reader::cmudict_dictionary_reader(const char *aDictionaryPath)
	: mReader(aDictionaryPath)
	, mPhonemeSet(tts::createPhonemeReader("cmu"))
{
}

bool
cmudict_dictionary_reader::read()
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
		case cmudict_parser::line_comment:
			parse_line_metadata(mReader.match());
			break;
		default: // Ignore errors ...
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

			uint32_t c = 0;
			const char *next = entry_word.begin();
			const char *last = entry_word.end();
			while ((next = utf8::read(next, c)) <= last)
			{
				ptr = utf8::write(ptr, ucd::tolower(c));
			}

			word = cainteoir::make_buffer(data, ptr - data);

			ipa::phonemes phonemes;

			mPhonemeSet->reset(std::make_shared<cainteoir::buffer>(pronunciation.begin(), pronunciation.end()));
			while (mPhonemeSet->read())
				phonemes.push_back(*mPhonemeSet);

			entry = { cainteoir::object_type::dictionary };
			entry.put("Entry::pronunciation", phonemes);
			if (!context.empty())
				entry.put("Entry::context", cainteoir::copy(context, context.size()));
			return true;
		}
		break;
	}
	return false;
}

void
cmudict_dictionary_reader::parse_line_metadata(cainteoir::buffer comment)
{
	std::string phonemeset;
	std::string accent;

	const char *first = comment.begin();
	const char *last  = comment.end();

	if (comment.size() < 5 || !(first[0] == '@' && first[1] == '@' && last[-1] == '@' && last[-2] == '@'))
		return;

	first += 2;
	last  -= 2;

	while (true)
	{
		auto data = read_key_value_pair(first, last);
		if (data.first.begin() == nullptr)
		{
			if (!phonemeset.empty() && !accent.empty())
			{
				std::string name = phonemeset + "/" + accent;
				auto reader = tts::createPhonemeReader(name.c_str());
				if (reader)
					mPhonemeSet = reader;
			}
			return;
		}

		if (data.first.compare("phoneset") == 0)
			phonemeset = data.second.str();
		else if (data.first.compare("accent") == 0)
			accent = data.second.str();
	}
}

std::shared_ptr<tts::dictionary_reader> tts::createCMUDictionaryReader(const char *aDictionaryPath)
{
	return std::make_shared<cmudict_dictionary_reader>(aDictionaryPath);
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
	                         const cainteoir::object &entry,
	                         const char *line_separator);

	void write_say_as_entry(const std::shared_ptr<cainteoir::buffer> &word,
	                        const std::shared_ptr<cainteoir::buffer> &say_as,
	                        const cainteoir::object &entry,
	                        const char *line_separator);

	FILE *mOut;
};

void cmudict_formatter::write_phoneme_entry(const std::shared_ptr<cainteoir::buffer> &word,
                                            std::shared_ptr<tts::phoneme_writer> &writer,
                                            const ipa::phonemes &phonemes,
                                            const cainteoir::object &entry,
                                            const char *line_separator)
{
	for (auto c : *word)
		fputc(toupper(c), mOut);

	const auto &context = entry.get("Entry::context");
	if (context.is_buffer())
	{
		fputc('(', mOut);
		for (auto c : *context.buffer())
			fputc(c, mOut);
		fputc(')', mOut);
	}

	fputc(' ', mOut);
	fputc(' ', mOut);

	for (auto p : phonemes)
		writer->write(p);
	writer->flush();

	fputc('\n', mOut);
}

void cmudict_formatter::write_say_as_entry(const std::shared_ptr<cainteoir::buffer> &word,
                                           const std::shared_ptr<cainteoir::buffer> &say_as,
                                           const cainteoir::object &entry,
                                           const char *line_separator)
{
	throw std::runtime_error("cmudict does not support say-as entries");
}

std::shared_ptr<tts::dictionary_formatter> tts::createCMUDictionaryFormatter(FILE *out)
{
	return std::make_shared<cmudict_formatter>(out);
}
