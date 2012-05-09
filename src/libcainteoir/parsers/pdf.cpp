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

struct pdf_document_reader : public cainteoir::document_reader
{
	pdf_document_reader(std::shared_ptr<cainteoir::buffer> &aData, const rdf::uri &aSubject, rdf::graph &aPrimaryMetadata, const std::string &aTitle);

	~pdf_document_reader();

	bool read();

	PopplerDocument *mDoc;
};

pdf_document_reader::pdf_document_reader(std::shared_ptr<cainteoir::buffer> &aData, const rdf::uri &aSubject, rdf::graph &aPrimaryMetadata, const std::string &aTitle)
{
	g_type_init();

	mDoc = poppler_document_new_from_data((char *)aData->begin(), aData->size(), nullptr, nullptr);
	if (!mDoc)
		throw std::runtime_error("error parsing PDF document");

	add_metadata(aPrimaryMetadata, aSubject, rdf::dc("title"),     poppler_document_get_title(mDoc));
	add_metadata(aPrimaryMetadata, aSubject, rdf::dc("creator"),   poppler_document_get_author(mDoc));
	add_metadata(aPrimaryMetadata, aSubject, rdf::dc("subject"),   poppler_document_get_subject(mDoc));

	aPrimaryMetadata.statement(aSubject, rdf::tts("mimetype"), rdf::literal("application/pdf"));
}

pdf_document_reader::~pdf_document_reader()
{
	g_object_unref(mDoc);
}

bool pdf_document_reader::read()
{
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
