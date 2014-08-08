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

struct cainteoir_dictionary_reader : public tts::dictionary_reader
{
	cainteoir_dictionary_reader(const cainteoir::path &aDictionaryPath);

	bool read();
private:
	struct dictionary_t
	{
		dictionary_t(const cainteoir::path &aDictionaryPath);

		cainteoir::path mBasePath;
		std::shared_ptr<cainteoir::buffer> mData;
		std::shared_ptr<tts::phoneme_reader> mPhonemeSet;
		const char *mCurrent;
		const char *mLast;
	};

	std::stack<dictionary_t> mFiles;
};

cainteoir_dictionary_reader::cainteoir_dictionary_reader(const cainteoir::path &aDictionaryPath)
{
	mFiles.push({ aDictionaryPath });
}

cainteoir_dictionary_reader::dictionary_t::dictionary_t(const cainteoir::path &aDictionaryPath)
	: mBasePath(aDictionaryPath.parent())
	, mData(cainteoir::make_file_buffer(aDictionaryPath))
{
	mCurrent = mData->begin();
	mLast = mData->end();
}

bool cainteoir_dictionary_reader::read()
{
	dictionary_t *top = mFiles.empty() ? nullptr : &mFiles.top();
	while (top)
	{
		if (top->mCurrent == top->mLast)
		{
			mFiles.pop();
			top = mFiles.empty() ? nullptr : &mFiles.top();
			continue;
		}

		if (*top->mCurrent == '#')
		{
			while (top->mCurrent != top->mLast && (*top->mCurrent != '\r' && *top->mCurrent != '\n'))
				++top->mCurrent;
			while (top->mCurrent != top->mLast && (*top->mCurrent == '\r' || *top->mCurrent == '\n'))
				++top->mCurrent;
			continue;
		}

		const char *begin_entry = top->mCurrent;
		const char *end_entry   = top->mCurrent;
		while (end_entry != top->mLast && *end_entry != '\t')
			++end_entry;

		top->mCurrent = end_entry;
		while (top->mCurrent != top->mLast && *top->mCurrent == '\t')
			++top->mCurrent;

		if (top->mCurrent == top->mLast) continue;

		const char *begin_definition = top->mCurrent;
		const char *end_definition   = top->mCurrent;
		while (end_definition != top->mLast && *end_definition != '\r' && *end_definition != '\n' && *end_definition != '#')
			++end_definition;

		top->mCurrent = end_definition;

		if (*end_definition == '#')
		{
			--end_definition;

			// remove trailing whitespace ...
			while (end_definition != begin_definition && (*end_definition == ' ' || *end_definition == '\t'))
				--end_definition;
			++end_definition;

			while (top->mCurrent != top->mLast && *top->mCurrent != '\r' && *top->mCurrent != '\n')
				++top->mCurrent;
		}

		while (top->mCurrent != top->mLast && (*top->mCurrent == '\r' || *top->mCurrent == '\n'))
			++top->mCurrent;

		word = cainteoir::make_buffer(begin_entry, end_entry - begin_entry);
		if (*begin_entry == '.')
		{
			if (word->compare(".import") == 0)
			{
				std::string definition(begin_definition, end_definition);
				mFiles.push({ top->mBasePath / definition });
				top = &mFiles.top();
			}
			else if (word->compare(".phonemeset") == 0)
			{
				std::string phonemes(begin_definition, end_definition);
				top->mPhonemeSet = tts::createPhonemeReader(phonemes.c_str());
			}
		}
		else if (*begin_definition == '/')
		{
			++begin_definition;
			while (end_definition != begin_definition && *end_definition != '/')
				--end_definition;

			if (begin_definition == end_definition) continue;
			if (!top->mPhonemeSet.get())
				throw std::runtime_error("The dictionary does not specify a phonemeset.");

			entry = { cainteoir::make_buffer(begin_definition, end_definition - begin_definition), top->mPhonemeSet };
			return true;
		}
		else
		{
			entry = { cainteoir::make_buffer(begin_definition, end_definition - begin_definition) };
			return true;
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
