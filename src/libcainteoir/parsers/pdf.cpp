/* Portable Document Format Reader.
 *
 * Copyright (C) 2012-2014 Reece H. Dunn
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
#include "compatibility.hpp"

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

static void walk_index(PopplerIndexIter *iter, std::list<PopplerAction *> &actions)
{
	do
	{
		PopplerAction *action = poppler_index_iter_get_action(iter);
		if (action->type == POPPLER_ACTION_GOTO_DEST)
			actions.push_back(action);
		else
			poppler_action_free(action);

		PopplerIndexIter *child = poppler_index_iter_get_child(iter);
		if (child)
			walk_index(child, actions);
		poppler_index_iter_free(child);
	} while (poppler_index_iter_next(iter));
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
		state_toc,
		state_page,
	};

	pdf_document_reader(std::shared_ptr<cainteoir::buffer> &aData, const rdf::uri &aSubject, rdf::graph &aPrimaryMetadata, const std::string &aTitle);

	~pdf_document_reader();

	bool read(rdf::graph *aMetadata);

	std::shared_ptr<cainteoir::buffer> mData;
	PopplerDocument *mDoc;

	std::list<PopplerAction *> mIndex;
	std::list<PopplerAction *>::iterator mCurrentIndex;
	int mNumPages;
	int mCurrentPage;

	rdf::uri mSubject;
	rdf::uri mCurrentReference;
	state mState;
	std::string mTitle;
};

pdf_document_reader::pdf_document_reader(std::shared_ptr<cainteoir::buffer> &aData, const rdf::uri &aSubject, rdf::graph &aPrimaryMetadata, const std::string &aTitle)
	: mData(aData)
	, mSubject(aSubject)
	, mState(state_title)
	, mTitle(aTitle)
{
	#if !GLIB_CHECK_VERSION(2, 36, 0)
		g_type_init();
	#endif

	mDoc = poppler_document_new_from_data((char *)aData->begin(), aData->size(), nullptr, nullptr);
	if (!mDoc)
		throw std::runtime_error("error parsing PDF document");

	mNumPages    = poppler_document_get_n_pages(mDoc);
	mCurrentPage = 0;

	PopplerIndexIter *index = poppler_index_iter_new(mDoc);
	if (index)
		walk_index(index, mIndex);
	poppler_index_iter_free(index);

	mCurrentIndex = mIndex.empty() ? mIndex.end() : mIndex.begin();

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
	for (auto &action : mIndex)
		poppler_action_free(action);

	g_object_unref(mDoc);
}

bool pdf_document_reader::read(rdf::graph *aMetadata)
{
	while (mCurrentPage < mNumPages) switch (mState)
	{
	case state_title:
		if (aMetadata)
		{
			const rdf::uri listing = aMetadata->genid();
			aMetadata->statement(mSubject, rdf::ref("listing"), listing);

			mCurrentReference = aMetadata->genid();
			aMetadata->statement(listing, rdf::rdf("type"), rdf::ref("Listing"));
			aMetadata->statement(listing, rdf::ref("type"), rdf::epv("toc"));
			aMetadata->statement(listing, rdf::ref("entries"), mCurrentReference);

			const rdf::uri entry = aMetadata->genid();
			aMetadata->statement(mCurrentReference, rdf::rdf("first"), entry);

			aMetadata->statement(entry, rdf::rdf("type"), rdf::ref("Entry"));
			aMetadata->statement(entry, rdf::ref("level"), rdf::literal(0, rdf::xsd("integer")));
			aMetadata->statement(entry, rdf::ref("target"), mSubject);
			aMetadata->statement(entry, rdf::dc("title"), rdf::literal(mTitle));
		}
		mState = mIndex.empty() ? state_page : state_toc;
		break;
	case state_toc:
		if (aMetadata)
		{
			const rdf::uri next = aMetadata->genid();
			aMetadata->statement(mCurrentReference, rdf::rdf("rest"), next);
			mCurrentReference = next;

			const rdf::uri entry = aMetadata->genid();
			aMetadata->statement(mCurrentReference, rdf::rdf("first"), entry);

			gchar *title = (*mCurrentIndex)->goto_dest.title;
			int page = (*mCurrentIndex)->goto_dest.dest->page_num;

			char pagenum[100];
			int len = snprintf(pagenum, sizeof(pagenum), "page%05d", page);
			pagenum[len] = '\0';

			aMetadata->statement(entry, rdf::rdf("type"), rdf::ref("Entry"));
			aMetadata->statement(entry, rdf::ref("level"), rdf::literal(1, rdf::xsd("integer")));
			aMetadata->statement(entry, rdf::ref("target"), rdf::uri(mSubject.str(), pagenum));
			aMetadata->statement(entry, rdf::dc("title"), rdf::literal(cainteoir::normalize(std::make_shared<cainteoir::buffer>(title))->str()));
		}
		if (++mCurrentIndex == mIndex.end())
			mState = state_page;
		break;
	case state_page:
		{
			PopplerPage *page = poppler_document_get_page(mDoc, mCurrentPage++);

			char pagenum[100];
			int len = snprintf(pagenum, sizeof(pagenum), "page%05d", mCurrentPage);
			pagenum[len] = '\0';

			type    = events::text | events::anchor;
			content = std::make_shared<glib_buffer>(poppler_page_get_text(page));
			anchor  = rdf::uri(mSubject.str(), pagenum);
			mState  = state_page;

			g_object_unref(page);
		}
		return true;
	}
	if (aMetadata)
		aMetadata->statement(mCurrentReference, rdf::rdf("rest"), rdf::rdf("nil"));
	return false;
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
