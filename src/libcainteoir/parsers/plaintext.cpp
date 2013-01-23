/* Plain Text Document Reader.
 *
 * Copyright (C) 2012-2013 Reece H. Dunn
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

	bool read();

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

bool plaintext_document_reader::read()
{
	switch (mState)
	{
	case state_title:
		type   = events::toc_entry | events::anchor;
		styles = &cainteoir::heading0;
		text   = cainteoir::make_buffer(mTitle);
		anchor = mSubject;
		mState = mData->empty() ? state_eof : state_text;
		break;
	case state_text:
		type   = events::text;
		text   = mData;
		anchor = rdf::uri();
		mState = state_eof;
		break;
	case state_eof:
		type = 0;
		text.reset();
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
