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

#include "../cainteoir_file_reader.hpp"

#include <stack>

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;

static bool is_multiword_entry(const cainteoir::buffer &s)
{
	for (auto c : s)
	{
		if (c == ' ') return true;
	}
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

		const cainteoir_file_reader::token_type type() const { return mReader.type(); }

		const cainteoir::buffer &match() const { return mReader.match(); }

		std::shared_ptr<cainteoir::buffer> buffer() const
		{
			return cainteoir::make_buffer(mReader.match().begin(), mReader.match().size());
		}

		cainteoir::path mBasePath;
		cainteoir_file_reader mReader;
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

		if (top->type() == cainteoir_file_reader::directive)
		{
			auto directive = top->match();
			if (!top->read() || top->type() != cainteoir_file_reader::directive_contents)
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
		else if (top->type() == cainteoir_file_reader::text || top->type() == cainteoir_file_reader::string)
		{
			word = top->buffer();
			if (top->read()) switch (top->type())
			{
			case cainteoir_file_reader::text:
				entry = { top->buffer() };
				return true;
			case cainteoir_file_reader::phonemes:
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
	if (!aDictionaryPath) return {};

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
	int n = fprintf(mOut, is_multiword_entry(*word) ? "\"%s\"" : "%s", word->str().c_str());
	if (n < 16) fprintf(mOut, "\t");
	if (n <  8) fprintf(mOut, "\t");
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
	int n = fprintf(mOut, is_multiword_entry(*word) ? "\"%s\"" : "%s", word->str().c_str());
	if (n < 16) fprintf(mOut, "\t");
	if (n <  8) fprintf(mOut, "\t");
	fprintf(mOut, "\t%s%s", say_as->str().c_str(), line_separator);
}

std::shared_ptr<tts::dictionary_formatter> tts::createCainteoirDictionaryFormatter(FILE *out)
{
	return std::make_shared<cainteoir_formatter>(out);
}
