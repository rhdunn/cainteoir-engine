/* RichText Document Parser.
 *
 * Copyright (C) 2011 Reece H. Dunn
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
#include <cainteoir/platform.hpp>

namespace rdf = cainteoir::rdf;

inline bool is_control_char(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-';
}

const char * skipRtfBlock(const char * begin, const char * end)
{
	while (begin <= end && *begin != '}')
		++begin;
	return ++begin;
}

const char * parseRtfBlock(const char * begin, const char * end, const rdf::uri &aSubject, cainteoir::document_events &events)
{
	if (*begin != '{')
		throw std::runtime_error(_("unrecognised rich text document format"));

	bool initial_control = true;
	cainteoir::rope data;

	++begin;
	while (begin <= end && *begin != '}')
	{
		if (*begin == '{')
		{
			if (!data.empty())
			{
				events.text(data.buffer());
				data.clear();
			}
			begin = parseRtfBlock(begin, end, aSubject, events);
		}
		else if (*begin == '\\')
		{
			++begin;

			const char * control = begin;
			while (begin <= end && is_control_char(*begin))
				++begin;

			if (!strncmp(control, "par", 3) && !data.empty())
			{
				events.text(data.buffer());
				data.clear();
			}

			if (initial_control && !(!strncmp(control, "rtf1", 4)))
				return skipRtfBlock(begin, end);

			initial_control = false;
		}
		else
		{
			while (begin <= end && (*begin == ' ' || *begin == '\n' || *begin == '\r' || *begin == '\t'))
				++begin;

			const char * text = begin;
			while (begin <= end && *begin != '{' && *begin != '\\' && *begin != '}')
				++begin;

			data.add(std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer(text, begin)));
		}
	}

	if (*begin != '}')
		throw std::runtime_error(_("unrecognised rich text document format"));

	if (!data.empty())
	{
		events.text(data.buffer());
		data.clear();
	}

	return ++begin;
}

void cainteoir::parseRtfDocument(std::tr1::shared_ptr<cainteoir::buffer> aData, const rdf::uri &aSubject, cainteoir::document_events &events)
{
	const char * end = parseRtfBlock(aData->begin(), aData->end(), aSubject, events);
	if ((end + 1) != aData->end())
	{
		fprintf(stderr, "rtf: end=%p; data-end=%p\n", end, aData->end());
		throw std::runtime_error(_("unexpected end of the rich text document"));
	}
}
