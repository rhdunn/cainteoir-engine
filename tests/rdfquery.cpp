/* RDF query tests.
 *
 * Copyright (C) 2011-2012 Reece H. Dunn
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

#include <cainteoir/metadata.hpp>
#include <cainteoir/document.hpp>
#include <cainteoir/stopwatch.hpp>
#include <iostream>
#include <cassert>
#include <cstdlib>

#include "tester.hpp"

namespace rdf = cainteoir::rdf;
namespace rql = cainteoir::rdf::query;

REGISTER_TESTSUITE("RDF Query");

void match_(const rdf::uri &a, const rdf::uri &b, const char *ref, const char *file, int lineno)
{
	assert_location(a.str() == b.str(), file, lineno);
	assert_location(a.ns    == b.ns,    file, lineno);
	assert_location(a.ref   == b.ref,   file, lineno);
}

void match_(const rdf::literal &a, const rdf::literal &b, const char *ref, const char *file, int lineno)
{
	assert_location(a.value    == b.value,    file, lineno);
	assert_location(a.language == b.language, file, lineno);
	assert_location(a.type.ns  == b.type.ns,  file, lineno);
	assert_location(a.type.ref == b.type.ref, file, lineno);
}

#define match(a, b) match_(a, b, #a " == " #b, __FILE__, __LINE__)

bool select_all(const std::shared_ptr<const rdf::triple> &aStatement)
{
	return true;
}

bool select_none(const std::shared_ptr<const rdf::triple> &aStatement)
{
	return false;
}

TEST_CASE("rql::subject")
{
	rdf::graph g;

	assert(g.statement(rdf::rdf("Property"), rdf::rdf("type"), rdf::rdf("Class")));
	match(rql::subject(g.back()), rdf::rdf("Property"));

	assert(g.statement(rdf::bnode("prop"), rdf::rdf("type"), rdf::rdf("Class")));
	match(rql::subject(g.back()), rdf::bnode("prop"));
}

TEST_CASE("rql::predicate")
{
	rdf::graph g;

	assert(g.statement(rdf::rdf("Property"), rdf::rdf("type"), rdf::rdf("Class")));
	match(rql::predicate(g.back()), rdf::rdf("type"));
}

TEST_CASE("rql::object")
{
	rdf::graph g;

	assert(g.statement(rdf::rdf("Property"), rdf::rdf("type"), rdf::rdf("Class")));
	match(rql::object(g.back()), rdf::rdf("Class"));

	assert(g.statement(rdf::rdf("Property"), rdf::rdf("type"), rdf::bnode("class")));
	match(rql::object(g.back()), rdf::bnode("class"));

	assert(g.statement(rdf::rdf("Property"), rdf::rdf("label"), rdf::literal("Property")));
	match(rql::object(g.back()), rdf::uri(std::string(), std::string()));

	assert(g.statement(rdf::rdf("Property"), rdf::rdf("label"), rdf::literal("Property", "en")));
	match(rql::object(g.back()), rdf::uri(std::string(), std::string()));

	assert(g.statement(rdf::rdf("Property"), rdf::rdf("label"), rdf::literal("Property", rdf::xsd("string"))));
	match(rql::object(g.back()), rdf::uri(std::string(), std::string()));
}

TEST_CASE("rql::value")
{
	rdf::graph g;

	assert(g.statement(rdf::rdf("Property"), rdf::rdf("type"), rdf::rdf("Class")));
	assert(rql::value(g.back()) == "");

	assert(g.statement(rdf::rdf("Property"), rdf::rdf("type"), rdf::bnode("class")));
	assert(rql::value(g.back()) == "");

	assert(g.statement(rdf::rdf("Property"), rdf::rdf("label"), rdf::literal("Property")));
	assert(rql::value(g.back()) == "Property");

	assert(g.statement(rdf::rdf("Property"), rdf::rdf("label"), rdf::literal("Property", "en")));
	assert(rql::value(g.back()) == "Property");

	assert(g.statement(rdf::rdf("Property"), rdf::rdf("label"), rdf::literal("Property", rdf::xsd("string"))));
	assert(rql::value(g.back()) == "Property");
}

TEST_CASE("rql::matches")
{
	rdf::graph g;
	assert(g.statement(rdf::dc("title"), rdf::rdf("type"), rdf::rdf("Property")));

	assert(rql::matches(rql::subject,   rdf::dc("title"))(g.back()));
	assert(rql::matches(rql::predicate, rdf::rdf("type"))(g.back()));
	assert(rql::matches(rql::object,    rdf::rdf("Property"))(g.back()));

	assert(!rql::matches(rql::subject,   rdf::dc("creator"))(g.back()));
	assert(!rql::matches(rql::predicate, rdf::rdf("label"))(g.back()));
	assert(!rql::matches(rql::object,    rdf::rdf("Class"))(g.back()));

	// expression templates:

	assert((rql::subject   == rdf::dc("title"))(g.back()));
	assert((rql::predicate == rdf::rdf("type"))(g.back()));
	assert((rql::object    == rdf::rdf("Property"))(g.back()));

	assert(!(rql::subject   == rdf::dc("creator"))(g.back()));
	assert(!(rql::predicate == rdf::rdf("label"))(g.back()));
	assert(!(rql::object    == rdf::rdf("Class"))(g.back()));
}

TEST_CASE("rql::both")
{
	rdf::graph g;
	assert(g.statement(rdf::dc("title"), rdf::rdf("type"), rdf::rdf("Property")));

	assert(rql::both(rql::matches(rql::subject,   rdf::dc("title")),
	                 rql::matches(rql::predicate, rdf::rdf("type")))(g.back()));
	assert(rql::both(rql::matches(rql::predicate, rdf::rdf("type")),
	                 rql::matches(rql::subject,   rdf::dc("title")))(g.back()));

	assert(!rql::both(rql::matches(rql::subject,   rdf::dcterms("title")),
	                  rql::matches(rql::predicate, rdf::rdf("type")))(g.back()));
	assert(!rql::both(rql::matches(rql::subject,   rdf::dc("title")),
	                  rql::matches(rql::predicate, rdf::rdf("range")))(g.back()));

	// expression templates:

	assert((rql::subject   == rdf::dc("title") && rql::predicate == rdf::rdf("type"))(g.back()));
	assert((rql::predicate == rdf::rdf("type") && rql::subject   == rdf::dc("title"))(g.back()));

	assert(!(rql::subject == rdf::dcterms("title") && rql::predicate == rdf::rdf("type"))(g.back()));
	assert(!(rql::subject == rdf::dc("title")      && rql::predicate == rdf::rdf("range"))(g.back()));
}

TEST_CASE("rql::select(graph, selector, value)")
{
	rdf::graph dcterms;
	auto reader = cainteoir::createDocumentReader("src/schema/dcterms.rdf", dcterms, std::string());
	assert(dcterms.size() == 867);

	rql::results all = rql::select(dcterms, select_all);
	assert(all.size() == 867);
	assert(!all.empty());

	rql::results none = rql::select(dcterms, select_none);
	assert(none.size() == 0);
	assert(none.empty());

	rql::results a = rql::select(dcterms, rql::subject == rdf::dcterms("title"));
	assert(a.size() == 9);
	assert(!a.empty());

	match(rql::subject(a.front()), rdf::dcterms("title"));
	match(rql::predicate(a.front()), rdf::rdfs("label"));
	match(rql::object(a.front()), rdf::uri(std::string(), std::string()));
	assert(rql::value(a.front()) == "Title");

	match(rql::subject(a.back()), rdf::dcterms("title"));
	match(rql::predicate(a.back()), rdf::rdfs("subPropertyOf"));
	match(rql::object(a.back()), rdf::dc("title"));
	assert(rql::value(a.back()) == std::string());

	rql::results b = rql::select(dcterms, rql::predicate == rdf::dcterms("issued"));
	assert(b.size() == 98);
	assert(!b.empty());

	rql::results c = rql::select(dcterms, rql::object == rdf::rdf("Property"));
	assert(c.size() == 55);
	assert(!c.empty());

	rql::results d = rql::select(dcterms, rql::object == rdf::rdf("Class"));
	assert(d.size() == 0);
	assert(d.empty());

	rql::results e = rql::select(a, rql::predicate == rdf::rdf("type"));
	assert(e.size() == 1);
	assert(!e.empty());

	match(rql::subject(e.front()), rdf::dcterms("title"));
	match(rql::predicate(e.front()), rdf::rdf("type"));
	match(rql::object(e.front()), rdf::rdf("Property"));
	assert(rql::value(e.front()) == std::string());
}

TEST_CASE("rql::contains")
{
	rdf::graph dcterms;
	auto reader = cainteoir::createDocumentReader("src/schema/dcterms.rdf", dcterms, std::string());
	assert(dcterms.size() == 867);

	rql::results a = rql::select(dcterms, rql::subject == rdf::dcterms("title"));
	assert(a.size() == 9);
	assert(!a.empty());

	assert(rql::contains(dcterms,  rql::subject == rdf::dcterms("title")));
	assert(!rql::contains(dcterms, rql::subject == rdf::dc("title")));

	assert(rql::contains(a,  rql::subject == rdf::dcterms("title")));
	assert(!rql::contains(a, rql::subject == rdf::dcterms("creator")));
}

TEST_CASE("rql::select_value")
{
	rdf::graph dcterms;
	auto reader = cainteoir::createDocumentReader("src/schema/dcterms.rdf", dcterms, std::string());
	assert(dcterms.size() == 867);

	rql::results a = rql::select(dcterms, rql::subject == rdf::dcterms("title"));
	assert(a.size() == 9);
	assert(!a.empty());

	assert(rql::select_value<std::string>(a, rql::predicate == rdf::rdfs("label"))
	       == "Title");

	assert(rql::select_value<std::string>(a, rql::predicate == rdf::skos("prefLabel"))
	       == "");

	assert(rql::select_value<std::string>(dcterms,
	       rql::subject == rdf::dcterms("creator") && rql::predicate == rdf::rdfs("label"))
	       == "Creator");

	assert(rql::select_value<std::string>(dcterms,
	       rql::subject == rdf::dcterms("creator") && rql::predicate == rdf::skos("prefLabel"))
	       == "");
}

TEST_CASE("rdf::graph::foreach")
{
	rdf::graph g;
	auto reader = cainteoir::createDocumentReader("tests/rdfxml/syntax/example19.rdf", g, std::string());
	assert(g.size() == 8);

	rdf::ns ex(   "ex",    "http://example.org/");
	rdf::ns stuff("stuff", "http://example.org/stuff/1.0/");

	std::vector<rdf::uri> fruit;
	g.foreach(ex("basket"), stuff("hasFruit"), [&fruit](const std::shared_ptr<const rdf::triple> &subject)
	{
		fruit.push_back(rql::object(subject));
	});

	assert(fruit.size() == 3);
	if (fruit.size() >= 1) match(fruit[0], ex("banana"));
	if (fruit.size() >= 2) match(fruit[1], ex("apple"));
	if (fruit.size() >= 3) match(fruit[2], ex("pear"));
}

TEST_CASE("performance")
{
	// Load Time Performance

	cainteoir::stopwatch load_time;
	rdf::graph data;
	auto reader = cainteoir::createDocumentReader("data/languages.rdf.gz", data, std::string());

	printf("... ... load time: %G (%d triples)\n", load_time.elapsed(), data.size());
	assert(!data.empty());

	// Select Performance

	cainteoir::stopwatch select1_time;
	rql::results languages = rql::select(data,
	                                     rql::predicate == rdf::rdf("type") &&
	                                     rql::object    == rdf::iana("Language"));

	printf("... ... select(_, predicate, object) as languages time: %G (%d triples)\n", select1_time.elapsed(), languages.size());
	assert(!languages.empty());

	// Select Performance : Subject
	//
	// This tests the performance of selecting the triples for a large number
	// of subjects. This is a typical use case:
	//    1.  Perform an rql::select to find a set of subjects.
	//    2.  Perform an rql::select on each subject.
	//
	// This highlights that doing step (2) is slow for large graphs. Intuitively
	// this makes sense, as it has O(N^2) complexity when the graph is stored
	// as a set of triples.

	cainteoir::stopwatch select2_time;
	int labels = 0;
	for (auto &lang : languages)
	{
		rql::results statements = rql::select(data, rql::subject == rql::subject(lang));
		auto id   = rql::select_value<std::string>(statements, rql::predicate == rdf::iana("code"));
		auto name = rql::select_value<std::string>(statements, rql::predicate == rdf::iana("label"));
	}

	printf("... ... select(subject, _, _) on each language time: %G\n", select2_time.elapsed());
}
