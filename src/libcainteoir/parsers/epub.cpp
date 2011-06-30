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
#include <cainteoir/platform.hpp>

namespace xml = cainteoir::xmldom;
namespace rdf = cainteoir::rdf;

void cainteoir::parseEpubDocument(std::tr1::shared_ptr<cainteoir::buffer> aData, const rdf::uri &aSubject, cainteoir::document_events &events)
{
	cainteoir::zip::archive epub(aData);

	xml::document ocf(epub.read("META-INF/container.xml"));
	std::string opffile = cainteoir::parseOcfDocument(ocf.root())["application/oebps-package+xml"];
	if (opffile.empty())
		throw std::runtime_error(_("Unsupported ePub content: OPF file not specified."));

	cainteoir::opffiles files;

	std::tr1::shared_ptr<cainteoir::buffer> data = epub.read(opffile.c_str());
	if (!data)
		throw std::runtime_error(_("Unsupported ePub content: OPF file not found."));

	xml::document opf(data);
	cainteoir::parseOpfDocument(opf.root(), aSubject, events, files);

	foreach_iter(file, files.spine)
	{
		std::string filename;
		std::string::size_type pos = opffile.rfind('/');
		if (pos == std::string::npos)
			filename = file->filename;
		else
			filename = opffile.substr(0, pos + 1) + file->filename;

		std::tr1::shared_ptr<cainteoir::buffer> doc = epub.read(filename.c_str());
		if (doc)
			cainteoir::parseXHtmlDocument(doc, rdf::uri(aSubject.str(), file->id), events);
		else
			fprintf(stderr, _("document '%s' not found in ePub archive.\n"), filename.c_str());
	}
}
