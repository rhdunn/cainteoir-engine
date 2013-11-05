/* Test driver for generated speech events from XML documents using CSS.
 *
 * Copyright (C) 2012-2013 Reece H. Dunn
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
#include <map>

namespace xml = cainteoir::xml;

int main(int argc, char ** argv)
{
	try
	{
		argc -= 1;
		argv += 1;

		if (argc == 0)
			throw std::runtime_error("no document specified");

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
			fprintf(stdout, "begin-tag : [%s|%s]\n",
			        reader.namespaceUri().c_str(), reader.nodeName().str().c_str());
			break;
		case xml::reader::endTagNode:
			fprintf(stdout, "end-tag   : [%s|%s]\n",
			        reader.namespaceUri().c_str(), reader.nodeName().str().c_str());
			break;
		case xml::reader::attribute:
			if (!reader.nodeName().compare("xmlns") || !reader.nodePrefix().compare("xmlns"))
				continue;
			fprintf(stdout, "attribute : [%s|%s] = %s\n",
			        reader.namespaceUri().c_str(), reader.nodeName().str().c_str(),
			        reader.nodeValue().str().c_str());
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
		default:
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
