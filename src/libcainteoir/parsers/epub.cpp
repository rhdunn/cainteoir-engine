/* ePub Document Parser.
 *
 * Copyright (C) 2010-2012 Reece H. Dunn
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
#include "parsers.hpp"

#include <stdexcept>

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
	epub_document(std::shared_ptr<cainteoir::archive> &data, cainteoir::document_events &aEvents, const rdf::uri &aSubject, rdf::graph &aGraph)
		: mEpub(data)
		, mEvents(aEvents)
		, mSubject(aSubject)
		, mGraph(aGraph)
		, mTocEvents(false)
	{
		cainteoir::ocf_reader ocf(mEpub->read("META-INF/container.xml"));
		while (ocf.read() && mOpfFile.empty())
		{
			if (ocf.mediaType() == "application/oebps-package+xml")
				mOpfFile = ocf.path();
		}

		if (mOpfFile.empty())
			throw std::runtime_error(i18n("Unsupported ePub content: OPF file not specified."));
	}

	void text(std::shared_ptr<cainteoir::buffer> aText)
	{
		return mEvents.text(aText);
	}

	void begin_context(cainteoir::events::context aContext, uint32_t aParameter)
	{
		mEvents.begin_context(aContext, aParameter);
	}

	void end_context()
	{
		mEvents.end_context();
	}

	void toc_entry(int depth, const rdf::uri &aLocation, const std::string &title)
	{
		if (mTocEvents)
		{
			if (aLocation == mSubject)
				mEvents.toc_entry(depth, aLocation, title);
			else
			{
				const rdf::uri location = mEpub->location(path_to(aLocation.ns, mOpfFile), aLocation.ref);
				mEvents.toc_entry(depth, location, title);
			}
		}
	}

	void anchor(const rdf::uri &aLocation, const std::string &mimetype)
	{
		if (!mimetype.empty())
		{
			std::string filename = path_to(aLocation.ns, mOpfFile);
			auto doc = mEpub->read(filename.c_str());
			if (doc)
			{
				cainteoir::xml::reader reader(doc);

				while (reader.read() && reader.nodeType() != cainteoir::xml::reader::beginTagNode)
					;

				if (mimetype == "application/x-dtbncx+xml")
				{
					mTocEvents = true;
					cainteoir::parseNcxDocument(reader, mSubject, *this, mGraph);
					mTocEvents = false;
				}
				else if (mimetype == "application/xhtml+xml")
				{
					const rdf::uri location = mEpub->location(filename, aLocation.ref);
					mEvents.anchor(location, std::string());

					rdf::graph mInnerMetadata;
					auto html = cainteoir::createHtmlReader(reader, location, mInnerMetadata);
					if (html) while (html->read())
					{
						if (html->type & cainteoir::events::toc_entry)
							toc_entry((int)html->parameter, html->anchor, html->text->str());
						if (html->type & cainteoir::events::anchor)
							anchor(html->anchor, std::string());
						if (html->type & cainteoir::events::begin_context)
							begin_context(html->context, html->parameter);
						if (html->type & cainteoir::events::text)
							text(html->text);
						if (html->type & cainteoir::events::end_context)
							end_context();
					}
				}
			}
			else
				fprintf(stderr, i18n("document '%s' not found in ePub archive.\n"), filename.c_str());
		}
	}

	std::shared_ptr<cainteoir::buffer> read(const char *filename)
	{
		return mEpub->read(filename);
	}

	std::shared_ptr<cainteoir::archive> mEpub;
	cainteoir::document_events &mEvents;
	const rdf::uri mSubject;
	std::string mOpfFile;
	rdf::graph &mGraph;
	bool mTocEvents;
};

void cainteoir::parseEpubDocument(std::shared_ptr<cainteoir::archive> aData, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph)
{
	epub_document epub(aData, events, aSubject, aGraph);

	auto data = epub.read(epub.mOpfFile.c_str());
	if (!data)
		throw std::runtime_error(i18n("Unsupported ePub content: OPF file not found."));

	parseDocumentBuffer(data, aSubject, epub, aGraph, no_document_flags);
}
