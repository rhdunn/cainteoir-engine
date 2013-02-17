/* Test for the exception dictionary API.
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

#include <cainteoir/document.hpp>
#include <cainteoir/text.hpp>
#include <cainteoir/stopwatch.hpp>
#include <stdexcept>
#include <map>
#include <unordered_map>

namespace rdf = cainteoir::rdf;
namespace tts = cainteoir::tts;

struct null_dictionary
{
	void insert(const std::shared_ptr<cainteoir::buffer> &word)
	{
	}

	std::size_t size() const
	{
		return 0;
	}
};

struct map_dictionary : public std::map<std::string, std::string>
{
	void insert(const std::shared_ptr<cainteoir::buffer> &word)
	{
		(*this)[word->str()] = std::string();
	}
};

struct unordered_map_dictionary : public std::unordered_map<std::string, std::string>
{
	void insert(const std::shared_ptr<cainteoir::buffer> &word)
	{
		(*this)[word->str()] = std::string();
	}
};

struct test_results
{
	uint32_t    words;
	std::size_t index_size;
};

template <typename Dictionary>
test_results parse_words(cainteoir::document_reader *reader)
{
	uint32_t words = 0;
	Dictionary dict;
	tts::text_reader text;
	while (reader->read())
	{
		text.next_item(*reader);
		while (text.read()) switch (text.match().type)
		{
		case tts::word_uppercase:
		case tts::word_lowercase:
		case tts::word_capitalized:
		case tts::word_mixedcase:
		case tts::word_script:
			dict.insert(text.match().text);
			++words;
			break;
		}
	}
	return { words, dict.size() };
}

int main(int argc, char ** argv)
{
	try
	{
		argc -= 1;
		argv += 1;

		if (argc != 2)
			throw std::runtime_error("no document specified");

		std::string format = argv[0];
		rdf::graph metadata;
		auto reader = cainteoir::createDocumentReader(argv[1], metadata, std::string());
		if (!reader)
		{
			fprintf(stderr, "unsupported document format for file \"%s\"\n", argv[1]);
			return 0;
		}

		cainteoir::stopwatch timer;
		test_results results = { 0, 0 };
		if (format == "null")
			results = parse_words<null_dictionary>(reader.get());
		else if (format == "map")
			results = parse_words<map_dictionary>(reader.get());
		else if (format == "unordered")
			results = parse_words<unordered_map_dictionary>(reader.get());
		printf("time:    %G\n", timer.elapsed());
		printf("words:   %d\n", results.words);
		printf("indexed: %d\n", results.index_size);
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
