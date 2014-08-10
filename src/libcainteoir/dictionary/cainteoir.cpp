/* Cainteoir Pronunciation Dictionary Format.
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

#include <cainteoir/dictionary.hpp>
#include <cainteoir/unicode.hpp>
#include <cainteoir/path.hpp>

#include <stack>

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;

struct dict_file_reader
{
	enum token_type
	{
		end_of_file,
		directive,
		directive_contents,
		text,
		phonemes,
	};

	dict_file_reader(const cainteoir::path &aFilePath);

	bool read();

	const token_type type() const { return mType; }

	const cainteoir::buffer &match() const { return mMatch; }
private:
	enum state
	{
		start,
		in_comment,
		in_directive,
		pre_directive_body,
		in_directive_body,
		in_text,
		in_phonemes,
	};

	std::shared_ptr<cainteoir::buffer> mData;
	const char *mCurrent;
	const char *mLast;
	state mState;

	token_type mType;
	cainteoir::buffer mMatch;
};

dict_file_reader::dict_file_reader(const cainteoir::path &aFilePath)
	: mData(cainteoir::make_file_buffer(aFilePath))
	, mState(start)
	, mType(end_of_file)
	, mMatch(nullptr, nullptr)
{
	mCurrent = mData->begin();
	mLast = mData->end();
}

bool dict_file_reader::read()
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
			mState = start;
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
		case '\r': case '\n': case '\t': case '#':
			// skip any trailing whitespace ...
			--mCurrent;
			while (mCurrent != begin_match && (*mCurrent == ' '))
				--mCurrent;
			++mCurrent;
			// return the matching token ...
			mState = start;
			mMatch = cainteoir::buffer(begin_match, mCurrent);
			mType = text;
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

struct cainteoir_dictionary_reader : public tts::dictionary_reader
{
	cainteoir_dictionary_reader(const cainteoir::path &aDictionaryPath);

	bool read();
private:
	struct dictionary_t
	{
		dictionary_t(const cainteoir::path &aDictionaryPath);

		bool read() { return mReader.read(); }

		const dict_file_reader::token_type type() const { return mReader.type(); }

		const cainteoir::buffer &match() const { return mReader.match(); }

		std::shared_ptr<cainteoir::buffer> buffer() const
		{
			return cainteoir::make_buffer(mReader.match().begin(), mReader.match().size());
		}

		cainteoir::path mBasePath;
		dict_file_reader mReader;
		std::shared_ptr<tts::phoneme_reader> mPhonemeSet;
	};

	std::stack<dictionary_t> mFiles;
};

cainteoir_dictionary_reader::cainteoir_dictionary_reader(const cainteoir::path &aDictionaryPath)
{
	mFiles.push({ aDictionaryPath });
}

cainteoir_dictionary_reader::dictionary_t::dictionary_t(const cainteoir::path &aDictionaryPath)
	: mBasePath(aDictionaryPath.parent())
	, mReader(aDictionaryPath)
{
}

bool cainteoir_dictionary_reader::read()
{
	dictionary_t *top = mFiles.empty() ? nullptr : &mFiles.top();
	while (top)
	{
		if (!top->read())
		{
			mFiles.pop();
			top = mFiles.empty() ? nullptr : &mFiles.top();
			continue;
		}

		if (top->type() == dict_file_reader::directive)
		{
			auto directive = top->match();
			if (!top->read() || top->type() != dict_file_reader::directive_contents)
			{
				fprintf(stderr, "error: missing directive body\n");
				continue;
			}

			if (directive.compare(".import") == 0)
			{
				mFiles.push({ top->mBasePath / top->match().str() });
				top = &mFiles.top();
			}
			else if (directive.compare(".phonemeset") == 0)
			{
				top->mPhonemeSet = tts::createPhonemeReader(top->match().str().c_str());
			}
		}
		else if (top->type() == dict_file_reader::text)
		{
			word = top->buffer();
			if (top->read()) switch (top->type())
			{
			case dict_file_reader::text:
				entry = { top->buffer() };
				return true;
			case dict_file_reader::phonemes:
				if (top->match().empty())
					continue;
				if (!top->mPhonemeSet.get())
					throw std::runtime_error("The dictionary does not specify a phonemeset.");
				entry = { top->buffer(), top->mPhonemeSet };
				return true;
			}

			throw std::runtime_error("Unknown content in the dictionary file.");
		}
		else
		{
			throw std::runtime_error("Unknown content in the dictionary file.");
		}
	}
	return false;
}

std::shared_ptr<tts::dictionary_reader> tts::createCainteoirDictionaryReader(const char *aDictionaryPath)
{
	return std::make_shared<cainteoir_dictionary_reader>(cainteoir::path(aDictionaryPath));
}

struct cainteoir_formatter : public tts::dictionary_formatter
{
	cainteoir_formatter(FILE *aOut)
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

void cainteoir_formatter::write_phoneme_entry(const std::shared_ptr<cainteoir::buffer> &word,
                                              std::shared_ptr<tts::phoneme_writer> &writer,
                                              const ipa::phonemes &phonemes,
                                              const char *line_separator)
{
	if (fprintf(mOut, "%s", word->str().c_str()) < 8)
		fprintf(mOut, "\t");
	fprintf(mOut, "\t/");
	for (auto p : phonemes)
		writer->write(p);
	writer->flush();
	fprintf(mOut, "/%s", line_separator);
}

void cainteoir_formatter::write_say_as_entry(const std::shared_ptr<cainteoir::buffer> &word,
                                             const std::shared_ptr<cainteoir::buffer> &say_as,
                                             const char *line_separator)
{
	if (fprintf(mOut, "%s", word->str().c_str()) < 8)
		fprintf(mOut, "\t");
	fprintf(mOut, "\t%s%s", say_as->str().c_str(), line_separator);
}

std::shared_ptr<tts::dictionary_formatter> tts::createCainteoirDictionaryFormatter(FILE *out)
{
	return std::make_shared<cainteoir_formatter>(out);
}
