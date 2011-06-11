/* Generate a tag cloud from a document.
 *
 * Copyright (C) 2011 Reece H. Dunn
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

#include <cainteoir/metadata.hpp>
#include <cainteoir/audio.hpp>
#include <cainteoir/document.hpp>
#include <cainteoir/platform.hpp>
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <getopt.h>
#include <math.h>

#include <sstream>
#include <map>

namespace rdf = cainteoir::rdf;

enum args
{
	ARG_HTML = 300,
	ARG_TEXT = 301,
};

static struct option options[] =
{
	{ "html", no_argument, 0, ARG_HTML },
	{ "text", no_argument, 0, ARG_TEXT },
	{ "all",  no_argument, 0, 'a' },
	{ "help", no_argument, 0, 'h' },
	{ 0, 0, 0, 0 }
};

void help()
{
	fprintf(stdout, _("usage: tagcloud [OPTION..] document\n"));
	fprintf(stdout, "\n");
	fprintf(stdout, _("Formats:\n"));
	fprintf(stdout, _(" --html                 Output a HTML tag cloud\n"));
	fprintf(stdout, _(" --text                 Output a text word list\n"));
	fprintf(stdout, "\n");
	fprintf(stdout, _("General:\n"));
	fprintf(stdout, _(" -a, --all              Show all words\n"));
	fprintf(stdout, _(" -h, --help             This help text\n"));
	fprintf(stdout, "\n");
	fprintf(stdout, _("Report bugs to msclrhd@gmail.com\n"));
}

static const char * common_words[] = {
	"a", "an", "and", "as", "at", "about", "all", "after",
	"but", "be", "been", "by", "back",
	"could",
	"did", "down", "do", "done",
	"even",
	"for", "from",
	"get", "got",
	"he", "her", "had", "his", "him", "have", "here",
	"i", "is", "it", "in", "if", "into", "its",
	"just",
	"like",
	"me", "more", "my", "mine", "man",
	"no", "not", "now",
	"of", "on", "or", "out", "one", "over", "off", "only",
	"she", "so", "said",
	"the", "to", "that", "than", "then", "they", "this", "there", "them", "their",
	"up",
	"was", "with", "were", "would", "what", "which", "when", "we", "who", "woman",
	"you",
	NULL,
};

bool common(const std::string & word)
{
	for (const char ** words = common_words; *words; ++words)
	{
		if (word == *words)
			return true;
	}
	return false;
}

std::string normalise(const std::string & word)
{
	std::string ret;
	foreach_iter(s, word)
	{
		if (*s >= 'A' && *s <= 'Z')
			ret.push_back((*s - 'A') + 'a');
		else if (*s >= 'a' && *s <= 'z')
			ret.push_back(*s);
		else if (*s == '-')
			ret.push_back(*s);
	}
	return ret;
}

struct cloud : public cainteoir::document_events
{
	void metadata(const rdf::statement &aStatement)
	{
	}

	const rdf::bnode genid()
	{
		return rdf::bnode(std::string());
	}

	void text(std::tr1::shared_ptr<cainteoir::buffer> aText)
	{
		std::istringstream ss(aText->str());

		std::string word;
		while (ss >> word)
		{
			++words[normalise(word)];
		}
	}

	std::map<std::string, int> words;
};

int main(int argc, char ** argv)
{
	cainteoir::initialise();

	try
	{
		enum
		{
			html_format,
			text_format,
		} format = html_format;

		bool show_all = false;

		while (1)
		{
			int option_index = 0;
			int c = getopt_long(argc, argv, "h", options, &option_index);
			if (c == -1)
				break;

			switch (c)
			{
			case ARG_HTML:
				format = html_format;
				break;
			case ARG_TEXT:
				format = text_format;
				break;
			case 'a':
				show_all = true;
				break;
			case 'h':
				help();
				return 0;
			}
		}

		argc -= optind;
		argv += optind;

		if (argc != 1)
			throw std::runtime_error("no document specified");

		cloud cloud;
		if (!cainteoir::parseDocument(argv[0], cloud))
			fprintf(stderr, "unsupported document format for file \"%s\"\n", argv[0]);

		if (format == html_format)
		{
			printf("<html><body><p>\n");
			foreach_iter(word, cloud.words)
			{
				if (show_all || (word->second > 3 && !common(word->first)))
				{
					int size = (log(word->second) * 30) + 80;
					printf("<span style=\"font-size: %d%%;\">%s</span>\n", size, word->first.c_str());
				}
			}
			printf("</p></body></html>\n");
		}
		else
		{
			foreach_iter(word, cloud.words)
			{
				if (show_all || (word->second > 3 && !common(word->first)))
					printf("%8d %s\n", word->second, word->first.c_str());
			}
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	cainteoir::cleanup();
	return 0;
}
