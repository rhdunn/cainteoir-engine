/* Plain Text Document Reader.
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

#include "parsers.hpp"

namespace rdf    = cainteoir::rdf;
namespace events = cainteoir::events;

struct plaintext_document_reader : public cainteoir::document_reader
{
	enum state
	{
		state_title,
		state_text,
		state_eof,
	};

	plaintext_document_reader(std::shared_ptr<cainteoir::buffer> &aData, const rdf::uri &aSubject, const std::string &aTitle);

	bool read(rdf::graph *aMetadata);

	std::shared_ptr<cainteoir::buffer> mData;
	rdf::uri mSubject;
	state mState;
	std::string mTitle;
};

plaintext_document_reader::plaintext_document_reader(std::shared_ptr<cainteoir::buffer> &aData, const rdf::uri &aSubject, const std::string &aTitle)
	: mData(aData)
	, mSubject(aSubject)
	, mState(state_title)
	, mTitle(aTitle)
{
	if (mTitle.empty())
	{
		mTitle = mSubject.str();
		std::string::size_type sep = mTitle.rfind('/');
		if (sep != std::string::npos)
			mTitle = mTitle.substr(sep + 1);
	}
}

bool plaintext_document_reader::read(rdf::graph *aMetadata)
{
	switch (mState)
	{
	case state_title:
		if (aMetadata)
		{
			const rdf::uri listing = aMetadata->genid();
			aMetadata->statement(mSubject, rdf::ref("listing"), listing);

			const rdf::uri currentReference = aMetadata->genid();
			aMetadata->statement(listing, rdf::rdf("type"), rdf::ref("Listing"));
			aMetadata->statement(listing, rdf::ref("type"), rdf::epv("toc"));
			aMetadata->statement(listing, rdf::ref("entries"), currentReference);

			const rdf::uri entry = aMetadata->genid();
			aMetadata->statement(currentReference, rdf::rdf("first"), entry);

			aMetadata->statement(entry, rdf::rdf("type"), rdf::ref("Entry"));
			aMetadata->statement(entry, rdf::ref("level"), rdf::literal(0, rdf::xsd("integer")));
			aMetadata->statement(entry, rdf::ref("target"), mSubject);
			aMetadata->statement(entry, rdf::dc("title"), rdf::literal(mTitle));

			aMetadata->statement(currentReference, rdf::rdf("rest"), rdf::rdf("nil"));
		}
		type    = events::anchor;
//		styles  = &cainteoir::heading0;
//		content = cainteoir::make_buffer(mTitle);
		anchor  = mSubject;
		mState  = mData->empty() ? state_eof : state_text;
		return true;
	case state_text:
		type    = events::text;
		content = mData;
		anchor  = rdf::uri();
		mState  = state_eof;
		break;
	case state_eof:
		type = 0;
		content.reset();
		return false;
	}
	return true;
}

std::shared_ptr<cainteoir::document_reader>
cainteoir::createPlainTextReader(std::shared_ptr<buffer> &aData,
                                 const rdf::uri &aSubject,
                                 rdf::graph &aPrimaryMetadata,
                                 const std::string &aTitle)
{
	// Octet Stream ...

	const uint8_t *begin = (const uint8_t *)aData->begin();
	const uint8_t *end   = (const uint8_t *)aData->end();
	if (begin + 101 < end) // only check the first 100 bytes
		end = begin + 101;

	while (begin < end)
	{
		if (*begin < 0x20 && !(*begin == '\r' || *begin == '\n' || *begin == '\t' || *begin == '\f'))
		{
			printf("error: control character 0x%02X found ... treating as octet stream.\n", *begin);
			aPrimaryMetadata.statement(aSubject, rdf::tts("mimetype"), rdf::literal("application/octet-stream"));
			return std::shared_ptr<cainteoir::document_reader>();
		}
		++begin;
	}

	// Plain Text ...

	aPrimaryMetadata.statement(aSubject, rdf::tts("mimetype"), rdf::literal("text/plain"));
	return std::make_shared<plaintext_document_reader>(aData, aSubject, aTitle);
}
