/* ePub Document Parser.
 *
 * Copyright (C) 2010 Reece H. Dunn
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

#include "parsers.hpp"
#include "zip.hpp"

namespace xml = cainteoir::xmldom;
namespace rdf = cainteoir::rdf;

void cainteoir::parseEpubDocument(const char *aFilename, rdf::model &aMetadata)
{
	cainteoir::zip::archive epub(aFilename);

	xml::document ocf(epub.read("META-INF/container.xml"));
	std::string opffile = cainteoir::parseOcfDocument(ocf.root())["application/oebps-package+xml"];
	if (opffile.empty())
		throw std::runtime_error("Unsupported ePub content: OPF file not found.");

	cainteoir::opffiles files;

	xml::document opf(epub.read(opffile.c_str()));
	cainteoir::parseOpfDocument(opf.root(), rdf::uri(aFilename, std::string()), aMetadata, files);
}

