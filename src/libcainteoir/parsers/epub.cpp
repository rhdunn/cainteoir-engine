/* ePub Document Parser.
 *
 * Copyright (C) 2010-2015 Reece H. Dunn
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
	title,           // The document title.
	publication_toc, // Processing the document's table of contents.
	publication,     // Processing the document's spine items.
	toc,             // Processing table of content document (NCX, etc.) events.
	content,         // Processing content document (HTML, etc.) events.
	eof,             // End of the file.
};

struct epub_document_reader : public cainteoir::document_reader
{
	epub_document_reader(std::shared_ptr<cainteoir::archive> &aData, const rdf::uri &aSubject, rdf::graph &aPrimaryMetadata, const char *aDefaultEncoding);

	bool read(rdf::graph *aMetadata);

	cainteoir::path opf_file;
	cainteoir::path opf_root;
	std::shared_ptr<cainteoir::document_reader> child;
	state mState;

	std::shared_ptr<cainteoir::archive> mData;
	rdf::uri mSubject;
	const char *mDefaultEncoding;

	rdf::graph mManifest;
	rql::results mManifestItem;

	std::shared_ptr<cainteoir::document_reader> media_overlay;
	rdf::uri mTextRef;
	cainteoir::document_item mMediaItem;
	std::string mDocTitle;
	cainteoir::path mCurrentOverlay;

	enum class document_type
	{
		toc,
		text,
		media_overlay,
	};

	bool load_document(const rql::results &aItem, document_type aType);
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

	auto opf = cainteoir::createOpfReader(reader, aSubject, aPrimaryMetadata, "application/epub+zip");
	opf_root = opf_file.parent();

	for (auto &query : rql::select(aPrimaryMetadata, rql::subject == aSubject))
	{
		if (rql::predicate(query).ns == rdf::dc || rql::predicate(query).ns == rdf::dcterms)
		{
			if (rql::predicate(query).ref == "title")
				mDocTitle = rql::value(query);
		}
	}

	mManifest.set_base(aPrimaryMetadata.get_base());
	while (opf->read(&mManifest))
		;

	mManifestItem = rql::select(mManifest, rql::subject == mSubject && rql::predicate == rdf::ref("spine"));
}

bool epub_document_reader::read(rdf::graph *aMetadata)
{
	while (true) switch (mState)
	{
	case state::title:
		clear().anchor_event(mSubject);
		mState = state::publication_toc;
		return true;
	case state::toc:
		while (child->read(aMetadata))
			;
		child.reset();
		mState = state::publication;
		break;
	case state::content:
		if (child->type & cainteoir::events::anchor)
		{
			if (mTextRef == child->anchor)
			{
				auto audiofile = cainteoir::path(mMediaItem.anchor.str()).zip_path();

				if (!mMediaItem.anchor.empty())
				{
					clear().media_ref_event(mMediaItem.anchor,
					                        mData->read(audiofile),
					                        mMediaItem.media_begin,
					                        mMediaItem.media_end);

					next_media_overlay_entry();
					return true;
				}
				else
					next_media_overlay_entry();
			}
		}

		while (child->read())
		{
			copy(*child);
			return true;
		}
		child.reset();
		mState = state::publication;
		break;
	case state::publication_toc:
		{
			auto results = rql::select(mManifest, rql::subject == mSubject && rql::predicate == rdf::ref("toc"));
			if (!results.empty())
			{
				auto item = rql::select(mManifest, rql::subject == rql::object(results.front()));
				if (load_document(item, document_type::toc))
				{
					mState = state::toc;
					continue;
				}
			}
		}
		mState = state::publication;
		break;
	case state::publication:
		if (mManifestItem.empty())
			mState = state::eof;
		else
		{
			const rdf::uri &item = rql::object(mManifestItem.front());
			auto first = rql::select(mManifest, rql::subject == item && rql::predicate == rdf::rdf("first"));

			mManifestItem = rql::select(mManifest, rql::subject == item && rql::predicate == rdf::rdf("rest"));
			if (!mManifestItem.empty())
			{
				if (rql::object(mManifestItem.front()) == rdf::rdf("nil"))
					mManifestItem.clear();
			}

			if (!first.empty())
			{
				auto item = rql::select(mManifest, rql::subject == rql::object(first.front()));

				auto media = rql::select(item, rql::predicate == rdf::ref("media-overlay"));
				if (!media.empty())
				{
					auto item = rql::select(mManifest, rql::subject == rql::object(media.front()));

					if (load_document(item, document_type::media_overlay))
						next_media_overlay_entry();
				}

				if (load_document(item, document_type::text))
					mState = state::content;
			}
		}
		break;
	case state::eof:
		return false;
	}
}

bool epub_document_reader::load_document(const rql::results &aItem, document_type aType)
{
	auto target = rql::object(rql::select(aItem, rql::predicate == rdf::ref("target")).front());
	auto mimetype = rql::select_value<std::string>(aItem, rql::predicate == rdf::ref("mimetype"));

	cainteoir::path filename = opf_root / target.str();
	auto reader = cainteoir::createXmlReader(mData->read(filename), mDefaultEncoding);
	if (!reader)
	{
		fprintf(stderr, i18n("document '%s' not found in ePub archive.\n"), (const char *)filename);
		return false;
	}

	switch (aType)
	{
	case document_type::toc:
		if (mimetype == "application/x-dtbncx+xml")
		{
			cainteoir::path location{ mData->location(filename.str(), {}).ns };
			rdf::graph innerMetadata;
			child = cainteoir::createNcxReader(reader, mSubject, innerMetadata, std::string(), location.parent());
		}
		else if (mimetype == "application/xhtml+xml")
		{
			cainteoir::path location{ mData->location(filename.str(), {}).ns };
			rdf::graph innerMetadata;
			child = cainteoir::createHtmlReader(reader, mSubject, innerMetadata, std::string(), "application/xhtml+xml", location.parent());
		}
		else
			return false;
		break;
	case document_type::text:
		if (mimetype == "application/xhtml+xml")
		{
			anchor = mData->location(filename.str(), {});
			rdf::graph innerMetadata;
			child = cainteoir::createHtmlReader(reader, anchor, innerMetadata, std::string(), "application/xhtml+xml", {});
		}
		else
			return false;
		break;
	case document_type::media_overlay:
		if (filename.str() == mCurrentOverlay.str())
			return false;
		if (mimetype == "application/smil+xml")
		{
			anchor = mData->location(filename.str(), target.ref);
			rdf::graph innerMetadata;
			media_overlay = cainteoir::createSmilReader(reader, anchor, innerMetadata, std::string());
			mCurrentOverlay = filename;
		}
		else
			return false;
		break;
	}
	return true;
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
