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

static std::string path_to(const std::string &filename, const std::string &opffile)
{
	std::string::size_type pos = opffile.rfind('/');
	if (pos == std::string::npos)
		return filename;
	else
		return opffile.substr(0, pos + 1) + filename;
}

struct epub_document : public cainteoir::document_events
{
	epub_document(cainteoir::document_events &aEvents, const rdf::uri &aSubject, const std::string &aOpfFile)
		: mEvents(aEvents)
		, mSubject(aSubject)
		, mOpfFile(aOpfFile)
	{
	}

	void metadata(const std::tr1::shared_ptr<const rdf::triple> &aStatement)
	{
		mEvents.metadata(aStatement);
	}

	const rdf::uri genid()
	{
		return mEvents.genid();
	}

	void text(std::tr1::shared_ptr<cainteoir::buffer> aText)
	{
		return mEvents.text(aText);
	}

	void begin_context(context aContext, uint32_t aParameter)
	{
		mEvents.begin_context(aContext, aParameter);
	}

	void end_context()
	{
		mEvents.end_context();
	}

	void toc_entry(int depth, const rdf::uri &aLocation, const std::string &title)
	{
		const rdf::uri location = rdf::uri(mSubject.str() + "!/" + path_to(aLocation.ns, mOpfFile), aLocation.ref);
		mEvents.toc_entry(depth, location, title);
	}

	void anchor(const rdf::uri &aLocation)
	{
		const rdf::uri location = rdf::uri(mSubject.str() + "!/" + path_to(aLocation.ns, mOpfFile), aLocation.ref);
		mEvents.anchor(location);
	}

	cainteoir::document_events &mEvents;
	const rdf::uri mSubject;
	std::string mOpfFile;
};

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

	std::tr1::shared_ptr<cainteoir::buffer> ncx = epub.read(path_to(files.toc.filename, opffile).c_str());
	if (ncx)
	{
		epub_document doc_events(events, aSubject, opffile);
		xml::document doc(ncx);
		cainteoir::parseNcxDocument(doc.root(), aSubject, doc_events);
	}

	foreach_iter(file, files.spine)
	{
		std::string filename = path_to(file->filename, opffile);

		std::tr1::shared_ptr<cainteoir::buffer> doc = epub.read(filename.c_str());
		if (doc)
		{
			const rdf::uri anchor = rdf::uri(aSubject.str() + "!/" + path_to(file->filename, opffile), std::string());
			events.anchor(anchor);
			cainteoir::parseXHtmlDocument(doc, anchor, events);
		}
		else
			fprintf(stderr, _("document '%s' not found in ePub archive.\n"), filename.c_str());
	}
}
