/* Plain Text Document Reader.
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

#include "parsers.hpp"

struct plaintext_document_reader : public cainteoir::document_reader
{
	plaintext_document_reader(std::shared_ptr<cainteoir::buffer> &aData)
		: mData(aData)
		, mHaveRead(false)
	{
	}

	bool read();

	event_type type() const;

	std::shared_ptr<cainteoir::buffer> text() const;

	std::shared_ptr<cainteoir::buffer> mData;
	bool mHaveRead;
};

bool plaintext_document_reader::read()
{
	if (mHaveRead) return false;

	mHaveRead = true;
	return !mData->empty();
}

cainteoir::document_reader::event_type plaintext_document_reader::type() const
{
	return text_event;
}

std::shared_ptr<cainteoir::buffer> plaintext_document_reader::text() const
{
	return mData;
}

std::shared_ptr<cainteoir::document_reader>
cainteoir::createPlainTextReader(std::shared_ptr<buffer> &aData,
                                 const rdf::uri &aSubject,
                                 rdf::graph &aPrimaryMetadata)
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
	return std::make_shared<plaintext_document_reader>(aData);
}
