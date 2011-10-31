/* Mime Type Support.
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

#include <cainteoir/mimetype.hpp>

bool cainteoir::mime::matchlet::match(const std::tr1::shared_ptr<cainteoir::buffer> &data) const
{
	const char *begin = data->begin() + offset;
	for (const char *start = begin; start != begin + range + 1; ++start)
	{
		if (start + pattern_length >= data->end())
			return false; // out of range!

		if (!strncmp(start, pattern, pattern_length))
			return true;
	}
	return false;
}

bool cainteoir::mime::magic::match(const std::tr1::shared_ptr<cainteoir::buffer> &data) const
{
	for (const cainteoir::mime::matchlet *matchlet = matchlets; matchlet != matchlets + length; ++matchlet)
	{
		if (!matchlet->match(data))
			return false;
	}

	return true;
}

bool cainteoir::mime::mimetype::match(const std::tr1::shared_ptr<cainteoir::buffer> &data) const
{
	for (const cainteoir::mime::magic *matchlet = magic; matchlet != magic + magic_length; ++matchlet)
	{
		if (matchlet->match(data))
			return true;
	}

	return false;
}

bool cainteoir::mime::mimetype::match(const std::string &uri, const std::string &name) const
{
	if (xmlns)
	{
		if (!(uri == xmlns || uri == std::string(xmlns) + "#"))
			return false;
	}
	return localname == name;
}

void cainteoir::mime::mimetype::metadata(rdf::graph &aGraph, const std::string &baseuri, const rdf::uri &type) const
{
	rdf::uri ref = rdf::uri(baseuri, name);
	aGraph.statement(ref, rdf::rdf("type"), type);
	aGraph.statement(ref, rdf::tts("name"), rdf::literal(name));
	aGraph.statement(ref, rdf::dc("title"), rdf::literal(label));
	aGraph.statement(ref, rdf::dc("description"), rdf::literal(label));
	aGraph.statement(ref, rdf::tts("mimetype"), rdf::literal(mimetype));
	if (aliases)
	{
		for (const char **alias = aliases; *alias; ++alias)
			aGraph.statement(ref, rdf::tts("mimetype"), rdf::literal(*alias));
	}
	for (const char **glob = globs; *glob; ++glob)
		aGraph.statement(ref, rdf::tts("extension"), rdf::literal(*glob));
}
