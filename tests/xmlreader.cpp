/* Test for parsing XML/HTML documents.
 *
 * Copyright (C) 2010-2011 Reece H. Dunn
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
#include "options.hpp"

#include <cainteoir/xmlreader.hpp>
#include <stdexcept>

namespace xml = cainteoir::xml;

namespace cainteoir
{
	// Internal API helper for testing HTML tree construction...
	void print_html_tree(const std::shared_ptr<xml::reader> &reader, bool silent);
}

void print_xml_tree(xml::reader &reader, bool silent)
{
	while (reader.read())
	{
		std::string ns = reader.namespaceUri();
		if (!silent) switch (reader.nodeType())
		{
		default:
			if (reader.nodePrefix().empty() && ns.empty())
			{
				fprintf(stdout, "|%s| %s\n",
				        xml::node_type_name(reader.nodeType()),
				        reader.nodeName().str().c_str());
			}
			else
			{
				fprintf(stdout, "|%s| [%s|%s]%s\n",
				        xml::node_type_name(reader.nodeType()),
				        reader.nodePrefix().str().c_str(),
				        ns.c_str(),
				        reader.nodeName().str().c_str());
			}
			break;
		case xml::reader::commentNode:
		case xml::reader::cdataNode:
		case xml::reader::textNode:
			fprintf(stdout, "|%s| \"\"\"%s\"\"\"\n",
			        xml::node_type_name(reader.nodeType()),
			        reader.nodeValue().str().c_str());
			break;
		case xml::reader::attribute:
			if (reader.nodePrefix().empty() && ns.empty())
			{
				fprintf(stdout, "|%s| %s=\"\"\"%s\"\"\"\n",
				        xml::node_type_name(reader.nodeType()),
				        reader.nodeName().str().c_str(),
				        reader.nodeValue().str().c_str());
			}
			else
			{
				fprintf(stdout, "|%s| [%s|%s]%s=\"\"\"%s\"\"\"\n",
				        xml::node_type_name(reader.nodeType()),
				        reader.nodePrefix().str().c_str(),
				        ns.c_str(),
				        reader.nodeName().str().c_str(),
				        reader.nodeValue().str().c_str());
			}
			break;
		case xml::reader::error:
			fprintf(stdout, "|error| internal parser error\n");
			break;
		}
	}
}

int main(int argc, char ** argv)
{
	try
	{
		int repeatCount = 1;
		bool time = false;
		bool silent = false;
		bool htmlTree = false;

		const option_group general_options = { nullptr, {
			{ 't', "time", bind_value(time, true),
			  i18n("Time how long it takes to parse the document") },
			{ 's', "silent", bind_value(silent, true),
			  i18n("Don't print out the document events") },
			{ 'r', "repeat", repeatCount, "REPEAT",
			  i18n("Repeat parsing the document REPEAT times") },
			{ 'H', "html", bind_value(htmlTree, true),
			  i18n("Use HTML tree construction rules to parse the file") },
		}};

		const std::initializer_list<const option_group *> options = {
			&general_options,
		};

		const std::initializer_list<const char *> usage = {
			i18n("xmlreader [OPTION..] DOCUMENT"),
		};

		if (!parse_command_line(options, usage, argc, argv))
			return 0;

		if (argc != 1)
			throw std::runtime_error("no document specified");

		clock_t start_time = ::clock();

		for (int i = 0; i != repeatCount; ++i)
		{
			auto data = cainteoir::make_file_buffer(argv[0]);
			if (htmlTree)
			{
				auto reader = cainteoir::createXmlReader(data, "windows-1252");
				cainteoir::print_html_tree(reader, silent);
			}
			else
			{
				xml::reader reader(data, "windows-1252");
				print_xml_tree(reader, silent);
			}
		}

		clock_t end_time = ::clock();
		if (time)
			fprintf(stderr, "elapsed time: %G\n", (float(end_time - start_time) / CLOCKS_PER_SEC));
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
