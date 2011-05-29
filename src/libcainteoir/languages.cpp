/* Cainteoir Engine.
 *
 * Copyright (C) 2010-2011 Reece H. Dunn
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

#include <cainteoir/languages.hpp>
#include <cainteoir/document.hpp>

namespace rdf = cainteoir::rdf;
namespace rql = cainteoir::rdf::query;

struct language_rdf
	: public cainteoir::document_events
	, public rdf::graph
{
	void metadata(const rdf::statement &aStatement)
	{
		push_back(aStatement);
	}

	const rdf::bnode genid()
	{
		return rdf::graph::genid();
	}

	void text(std::tr1::shared_ptr<cainteoir::buffer> aText)
	{
	}
};

cainteoir::languages::languages(const char * locale)
{
	language_rdf data;
	try
	{
		const char * filename = DATADIR "/" PACKAGE "/languages.rdf";
		printf("loading language data from %s\n", filename);

		cainteoir::parseDocument(filename, data);
	}
	catch (const std::exception & e)
	{
		printf("error: %s\n", e.what());
	}

	rql::results languages = rql::select(
		rql::select(data, rql::predicate, rdf::rdf("type")),
		rql::object, rdf::skos("Concept"));

	foreach_iter(lang, languages)
	{
		const rdf::uri *uri = rql::subject(*lang);
		if (uri)
		{
			rql::results statements = rql::select(data, rql::subject, *uri);
			std::string name;

			foreach_iter(statement, statements)
			{
				if (rql::predicate(*statement) == rdf::skos("prefLabel"))
					name = rql::value(*statement);
			}

			foreach_iter(statement, statements)
			{
				if (rql::predicate(*statement) == rdf::skos("altLabel"))
					m_languages[ rql::value(*statement) ] = name;
			}
		}
	}
}

std::string cainteoir::languages::operator()(const std::string & langid)
{
	std::string & lang = m_languages[langid];
	if (lang.empty())
		lang = langid;
	return lang;
}
