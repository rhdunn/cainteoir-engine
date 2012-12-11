/* Test driver for generated speech events from XML documents using CSS.
 *
 * Copyright (C) 2012 Reece H. Dunn
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

#include <cainteoir/xmlreader.hpp>
#include <stdexcept>
#include <list>

namespace xml = cainteoir::xml;

namespace cainteoir
{
	struct style_manager
	{
		typedef std::string context;

		std::list<context> ctx;

		void push_context(const std::string &ns, const std::string &name);

		void pop_context(const std::string &ns, const std::string &name);
	};
}

void cainteoir::style_manager::push_context(const std::string &ns, const std::string &name)
{
	ctx.push_back(name);

	fprintf(stdout, "context >> ");
	for (auto &item : ctx)
	{
		fprintf(stdout, " %s", item.c_str());
	}
	fprintf(stdout, "\n");
}

void cainteoir::style_manager::pop_context(const std::string &ns, const std::string &name)
{
	ctx.pop_back();

	fprintf(stdout, "context << ");
	for (auto &item : ctx)
	{
		fprintf(stdout, " %s", item.c_str());
	}
	fprintf(stdout, "\n");
}

int main(int argc, char ** argv)
{
	try
	{
		argc -= 1;
		argv += 1;

		if (argc == 0)
			throw std::runtime_error("no document specified");

		cainteoir::style_manager styles;
		xml::reader reader(cainteoir::make_file_buffer(argv[0]), "windows-1252");
		/*
		 * reader.nodePrefix()   -- namespace prefix
		 * reader.namespaceUri() -- namespace value
		 * reader.nodeName()     -- local name
		 * reader.nodeValue()    -- node context
		 */
		while (reader.read()) switch (reader.nodeType())
		{
		case xml::reader::beginTagNode:
			styles.push_context(reader.namespaceUri(), reader.nodeName().str());
			break;
		case xml::reader::endTagNode:
			styles.pop_context(reader.namespaceUri(), reader.nodeName().str());
			break;
		case xml::reader::attribute:
			break;
		case xml::reader::cdataNode:
		case xml::reader::textNode:
			break;
		case xml::reader::commentNode:
		case xml::reader::beginProcessingInstructionNode:
		case xml::reader::endProcessingInstructionNode:
			// ignore
			break;
		case xml::reader::error:
			fprintf(stdout, "|error| internal parser error\n");
			break;
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
