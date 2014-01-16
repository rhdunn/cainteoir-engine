/* ePub Document Parser.
 *
 * Copyright (C) 2010-2013 Reece H. Dunn
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

#include <cainteoir/path.hpp>
#include <stdexcept>

namespace rdf    = cainteoir::rdf;
namespace rql    = cainteoir::rdf::query;
namespace xml    = cainteoir::xml;
namespace events = cainteoir::events;
namespace css    = cainteoir::css;

enum class state
{
	title,       // The document title toc-entry event.
	publication, // Processing OPF toc-entry events.
	toc,         // Processing table of content document (NCX, etc.) events.
	content,     // Processing content document (HTML, etc.) events.
	eof,         // End of the file.
};

struct epub_document_reader : public cainteoir::document_reader
{
	epub_document_reader(std::shared_ptr<cainteoir::archive> &aData, const rdf::uri &aSubject, rdf::graph &aPrimaryMetadata, const char *aDefaultEncoding);

	bool read(rdf::graph *aMetadata);

	cainteoir::path opf_file;
	cainteoir::path opf_root;
	std::shared_ptr<cainteoir::document_reader> opf;
	std::shared_ptr<cainteoir::document_reader> child;
	state mState;

	std::shared_ptr<cainteoir::archive> mData;
	rdf::uri mSubject;
	const char *mDefaultEncoding;

	std::shared_ptr<cainteoir::document_reader> media_overlay;
	rdf::uri mTextRef;
	cainteoir::document_item mMediaItem;
	std::string mDocTitle;

	void next_media_overlay_entry();
};

epub_document_reader::epub_document_reader(std::shared_ptr<cainteoir::archive> &aData, const rdf::uri &aSubject, rdf::graph &aPrimaryMetadata, const char *aDefaultEncoding)
	: mData(aData)
	, mSubject(aSubject)
	, mState(state::title)
	, mDefaultEncoding(aDefaultEncoding)
{
	auto ocf = cainteoir::createOcfReader(cainteoir::createXmlReader(mData->read("META-INF/container.xml"), mDefaultEncoding));
	if (!ocf.get())
		throw std::runtime_error(i18n("Unsupported ePub content: OCF file not found."));

	while (ocf->read() && opf_file.empty())
	{
		if (!ocf->content->compare("application/oebps-package+xml"))
			opf_file = cainteoir::path(ocf->anchor.str());
	}

	if (opf_file.empty())
		throw std::runtime_error(i18n("Unsupported ePub content: OPF file not specified."));

	auto reader = cainteoir::createXmlReader(mData->read(opf_file), mDefaultEncoding);
	if (!reader)
		throw std::runtime_error(i18n("Unsupported ePub content: OPF file not found."));

	opf = cainteoir::createOpfReader(reader, aSubject, aPrimaryMetadata, "application/epub+zip");
	opf_root = opf_file.parent();

	for (auto &query : rql::select(aPrimaryMetadata, rql::subject == aSubject))
	{
		if (rql::predicate(query).ns == rdf::dc || rql::predicate(query).ns == rdf::dcterms)
		{
			if (rql::predicate(query).ref == "title")
				mDocTitle = rql::value(query);
		}
	}
}

bool epub_document_reader::read(rdf::graph *aMetadata)
{
	while (true) switch (mState)
	{
	case state::title:
		styles  = &cainteoir::heading0;
		type    = events::toc_entry | events::anchor;
		content = cainteoir::make_buffer(mDocTitle);
		anchor  = mSubject;
		mState  = state::publication;
		return true;
	case state::toc:
		while (child->read())
		{
			if (!(child->type & cainteoir::events::toc_entry))
				continue;

			if (child->styles == &cainteoir::heading0)
				continue;

			if (child->anchor == mSubject)
				anchor = mSubject;
			else
			{
				auto filename = opf_root / child->anchor.ns;
				anchor = mData->location(filename.str(), child->anchor.ref);
			}

			type    = cainteoir::events::toc_entry;
			styles  = child->styles;
			content = child->content;
			return true;
		}
		child.reset();
		mState = state::publication;
		break;
	case state::content:
		if (child->type & cainteoir::events::anchor)
		{
			if (mTextRef == child->anchor)
			{
				auto audiofile = cainteoir::path(mMediaItem.anchor.str()).zip_path();

				type        = mMediaItem.type;
				styles      = mMediaItem.styles;
				content     = mData->read(audiofile);
				anchor      = mMediaItem.anchor;
				media_begin = mMediaItem.media_begin;
				media_end   = mMediaItem.media_end;

				next_media_overlay_entry();
				return true;
			}
		}

		while (child->read())
		{
			if (child->type & cainteoir::events::toc_entry)
				child->type &= ~cainteoir::events::toc_entry;

			if (child->type)
			{
				type    = child->type;
				styles  = child->styles;
				content = child->content;
				anchor  = child->anchor;
				return true;
			}
		}
		child.reset();
		mState = state::publication;
		break;
	case state::publication:
		if (!opf->read())
			mState = state::eof;
		else if (opf->type & cainteoir::events::toc_entry)
		{
			cainteoir::path filename = opf_root / opf->anchor.ns;
			auto reader = cainteoir::createXmlReader(mData->read(filename), mDefaultEncoding);
			if (!reader)
			{
				fprintf(stderr, i18n("document '%s' not found in ePub archive.\n"), (const char *)filename);
				continue;
			}

			if (!opf->content->compare("application/x-dtbncx+xml"))
			{
				rdf::graph innerMetadata;
				child = cainteoir::createNcxReader(reader, mSubject, innerMetadata, std::string());
				mState = state::toc;
			}
			else if (!opf->content->compare("application/xhtml+xml"))
			{
				anchor = mData->location(filename.str(), opf->anchor.ref);

				rdf::graph innerMetadata;
				child = cainteoir::createHtmlReader(reader, anchor, innerMetadata, std::string(), "application/xhtml+xml");
				mState = (opf->type & cainteoir::events::navigation_document)
				       ? state::toc
				       : state::content;
			}
			else if (!opf->content->compare("application/smil+xml"))
			{
				anchor = mData->location(filename.str(), opf->anchor.ref);

				rdf::graph innerMetadata;
				media_overlay = cainteoir::createSmilReader(reader, anchor, innerMetadata, std::string());
				next_media_overlay_entry();
			}
		}
		break;
	case state::eof:
		return false;
	}
}

void epub_document_reader::next_media_overlay_entry()
{
	if (!media_overlay)
	{
		mTextRef = {};
		mMediaItem = {};
		return;
	}

	while (media_overlay->read())
	{
		if (media_overlay->type & cainteoir::events::begin_context && media_overlay->styles)
		{
			if (media_overlay->styles->media_synchronisation == css::media_synchronisation::parallel)
			{
				mTextRef = {};
				mMediaItem = {};
			}
		}

		if (media_overlay->type & cainteoir::events::end_context && media_overlay->styles)
		{
			if (media_overlay->styles->media_synchronisation == css::media_synchronisation::parallel)
				return;
		}

		if (media_overlay->type & cainteoir::events::text_ref)
			mTextRef = media_overlay->anchor;

		if (media_overlay->type & cainteoir::events::media_ref)
			mMediaItem = *media_overlay;
	}

	media_overlay.reset();
	mTextRef = {};
	mMediaItem = {};
}

std::shared_ptr<cainteoir::document_reader>
cainteoir::createEpubReader(std::shared_ptr<archive> &aData, const rdf::uri &aSubject, rdf::graph &aPrimaryMetadata, const char *aDefaultEncoding)
{
	return std::make_shared<epub_document_reader>(aData, aSubject, aPrimaryMetadata, aDefaultEncoding);
}
