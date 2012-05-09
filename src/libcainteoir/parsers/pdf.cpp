/* Portable Document Format Reader.
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

#include "config.h"

#include "parsers.hpp"

#ifdef HAVE_POPPLER

#include "poppler/glib/poppler-document.h"
#include <stdexcept>

namespace rdf    = cainteoir::rdf;
namespace events = cainteoir::events;

static void add_metadata(rdf::graph &aMetadata, const rdf::uri &aSubject, const rdf::uri &aPredicate, gchar *aObject)
{
	if (!aObject) return;

	aMetadata.statement(aSubject, aPredicate, rdf::literal(aObject));
	g_free(aObject);
}

static void add_metadata(rdf::graph &aMetadata, const rdf::uri &aSubject, const rdf::uri &aPredicate, const time_t &aObject)
{
	char object[100];
	strftime(object, sizeof(object), "%Y-%m-%d", localtime(&aObject));

	aMetadata.statement(aSubject, aPredicate, rdf::literal(object));
}

struct glib_buffer : public cainteoir::buffer
{
	glib_buffer(char *str) : cainteoir::buffer(str)
	{
	}

	~glib_buffer()
	{
		g_free((char *)begin());
	}
};

struct pdf_document_reader : public cainteoir::document_reader
{
	enum state
	{
		state_title,
		state_text,
	};

	pdf_document_reader(std::shared_ptr<cainteoir::buffer> &aData, const rdf::uri &aSubject, rdf::graph &aPrimaryMetadata, const std::string &aTitle);

	~pdf_document_reader();

	bool read();

	PopplerDocument *mDoc;
	int mNumPages;
	int mCurrentPage;

	rdf::uri mSubject;
	state mState;
	std::string mTitle;
};

pdf_document_reader::pdf_document_reader(std::shared_ptr<cainteoir::buffer> &aData, const rdf::uri &aSubject, rdf::graph &aPrimaryMetadata, const std::string &aTitle)
	: mSubject(aSubject)
	, mState(state_title)
	, mTitle(aTitle)
{
	g_type_init();

	mDoc = poppler_document_new_from_data((char *)aData->begin(), aData->size(), nullptr, nullptr);
	if (!mDoc)
		throw std::runtime_error("error parsing PDF document");

	mNumPages    = poppler_document_get_n_pages(mDoc);
	mCurrentPage = 0;

	char *title = poppler_document_get_title(mDoc);
	if (title)
		mTitle = title;

	add_metadata(aPrimaryMetadata, aSubject, rdf::dc("title"),   title);
	add_metadata(aPrimaryMetadata, aSubject, rdf::dc("creator"), poppler_document_get_author(mDoc));
	add_metadata(aPrimaryMetadata, aSubject, rdf::dc("subject"), poppler_document_get_subject(mDoc));

	add_metadata(aPrimaryMetadata, aSubject, rdf::dcterms("created"),  poppler_document_get_creation_date(mDoc));
	add_metadata(aPrimaryMetadata, aSubject, rdf::dcterms("modified"), poppler_document_get_modification_date(mDoc));

	if (mTitle.empty())
	{
		mTitle = mSubject.str();
		std::string::size_type sep = mTitle.rfind('/');
		if (sep != std::string::npos)
			mTitle = mTitle.substr(sep + 1);
	}


	aPrimaryMetadata.statement(aSubject, rdf::tts("mimetype"), rdf::literal("application/pdf"));
}

pdf_document_reader::~pdf_document_reader()
{
	g_object_unref(mDoc);
}

bool pdf_document_reader::read()
{
	if (mCurrentPage >= mNumPages)
		return false;

	switch (mState)
	{
	case state_title:
		type      = events::toc_entry | events::anchor;
		context   = events::heading;
		parameter = 0;
		text      = cainteoir::make_buffer(mTitle);
		anchor    = mSubject;
		mState    = state_text;
		break;
	case state_text:
		{
			PopplerPage *page = poppler_document_get_page(mDoc, mCurrentPage);

			type      = events::text;
			context   = events::span;
			parameter = events::nostyle;
			text      = std::make_shared<glib_buffer>(poppler_page_get_text(page));
			anchor    = rdf::uri();
			mState    = state_text;

			g_object_unref(page);
			++mCurrentPage;
		}
		break;
	}
	return true;
}

std::shared_ptr<cainteoir::document_reader>
cainteoir::createPdfReader(std::shared_ptr<buffer> &aData,
                           const rdf::uri &aSubject,
                           rdf::graph &aPrimaryMetadata,
                           const std::string &aTitle)
{
	return std::make_shared<pdf_document_reader>(aData, aSubject, aPrimaryMetadata, aTitle);
}

#else

std::shared_ptr<cainteoir::document_reader>
cainteoir::createPdfReader(std::shared_ptr<buffer> &aData,
                           const rdf::uri &aSubject,
                           rdf::graph &aPrimaryMetadata,
                           const std::string &aTitle)
{
	return std::shared_ptr<document_reader>();
}

#endif
