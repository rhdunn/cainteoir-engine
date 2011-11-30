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
	epub_document(std::tr1::shared_ptr<cainteoir::buffer> &data, cainteoir::document_events &aEvents, const rdf::uri &aSubject, rdf::graph &aGraph)
		: mEpub(data)
		, mEvents(aEvents)
		, mSubject(aSubject)
		, mGraph(aGraph)
	{
		xml::document ocf(mEpub.read("META-INF/container.xml"));
		mOpfFile = cainteoir::parseOcfDocument(ocf.root())["application/oebps-package+xml"];
		if (mOpfFile.empty())
			throw std::runtime_error(_("Unsupported ePub content: OPF file not specified."));
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

	void anchor(const rdf::uri &aLocation, const std::string &mimetype)
	{
		std::string filename = path_to(aLocation.ns, mOpfFile);
		if (mimetype == "application/x-dtbncx+xml")
		{
			std::tr1::shared_ptr<cainteoir::buffer> ncx = mEpub.read(filename.c_str());
			if (ncx)
				cainteoir::parseNcxDocument(ncx, mSubject, *this, mGraph);
		}
		else if (mimetype == "application/xhtml+xml")
		{
			std::tr1::shared_ptr<cainteoir::buffer> doc = mEpub.read(filename.c_str());
			if (doc)
			{
				const rdf::uri location = rdf::uri(mSubject.str() + "!/" + filename, aLocation.ref);
				mEvents.anchor(location, mimetype);

				cainteoir::parseXHtmlDocument(doc, location, mEvents, mGraph);
			}
			else
				fprintf(stderr, _("document '%s' not found in ePub archive.\n"), filename.c_str());
		}
	}

	std::tr1::shared_ptr<cainteoir::buffer> read(const char *filename)
	{
		return mEpub.read(filename);
	}

	cainteoir::zip::archive mEpub;
	cainteoir::document_events &mEvents;
	const rdf::uri mSubject;
	std::string mOpfFile;
	rdf::graph &mGraph;
};

void cainteoir::parseEpubDocument(std::tr1::shared_ptr<cainteoir::buffer> aData, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph)
{
	epub_document epub(aData, events, aSubject, aGraph);

	std::tr1::shared_ptr<cainteoir::buffer> data = epub.read(epub.mOpfFile.c_str());
	if (!data)
		throw std::runtime_error(_("Unsupported ePub content: OPF file not found."));

	cainteoir::parseOpfDocument(data, aSubject, epub, aGraph);
}
