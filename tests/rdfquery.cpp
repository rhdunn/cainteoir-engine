/* RDF query tests.
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

#include <cainteoir/metadata.hpp>
#include <cainteoir/document.hpp>
#include <iostream>
#include <cassert>
#include <cstdlib>

#include "tester.hpp"

namespace rdf = cainteoir::rdf;
namespace rql = cainteoir::rdf::query;

REGISTER_TESTSUITE("RDF Query");

void match_(const rdf::any_type &a, const rdf::bnode &b, const char *fn, const char *ref, int lineno)
{
	const rdf::bnode *bnode = a.as<rdf::bnode>();
	if (assert_(bnode, fn, ref, lineno))
	{
		equal_(bnode->id, b.id, fn, ref, lineno);
	}
}

void match_(const rdf::any_type &a, const rdf::uri &b, const char *fn, const char *ref, int lineno)
{
	const rdf::uri *uri = a.as<rdf::uri>();
	if (assert_(uri, fn, ref, lineno))
	{
		equal_(uri->str(), b.str(), fn, ref, lineno);
		equal_(uri->ns,    b.ns,    fn, ref, lineno);
		equal_(uri->ref,   b.ref,   fn, ref, lineno);
	}
}

void match_(const rdf::any_type &a, const rdf::literal &b, const char *fn, const char *ref, int lineno)
{
	const rdf::literal *literal = a.as<rdf::literal>();
	if (assert_(literal, fn, ref, lineno))
	{
		equal_(literal->value,      b.value,    fn, ref, lineno);
		equal_(literal->language,   b.language, fn, ref, lineno);
		equal_(literal->type.ns,    b.type.ns,  fn, ref, lineno);
		equal_(literal->type.ref,   b.type.ref, fn, ref, lineno);
	}
}

#define match(a, b) match_(a, b, __FUNCTION__, #a " == " #b, __LINE__)

struct rdfdoc : public rdf::graph, public cainteoir::document_events
{
	rdfdoc(const char *filename)
	{
		parseDocument(filename, *this);
	}

	void metadata(const std::tr1::shared_ptr<const rdf::triple> &aStatement)
	{
		push_back(aStatement);
	}

	const rdf::bnode genid()
	{
		return rdf::graph::genid();
	}
};

bool select_all(const std::tr1::shared_ptr<const rdf::triple> &aStatement)
{
	return true;
}

bool select_none(const std::tr1::shared_ptr<const rdf::triple> &aStatement)
{
	return false;
}

TEST_CASE("rql::subject")
{
	match(rql::subject(rdf::statement(rdf::rdf("Property"), rdf::rdf("type"), rdf::rdf("Class"))),
	                   rdf::rdf("Property"));
	match(rql::subject(rdf::statement(rdf::bnode("prop"), rdf::rdf("type"), rdf::rdf("Class"))),
	                   rdf::bnode("prop"));
}

TEST_CASE("rql::predicate")
{
	match(rql::predicate(rdf::statement(rdf::rdf("Property"), rdf::rdf("type"), rdf::rdf("Class"))),
	                     rdf::rdf("type"));
}

TEST_CASE("rql::object")
{
	match(rql::object(rdf::statement(rdf::rdf("Property"), rdf::rdf("type"), rdf::rdf("Class"))),
	                  rdf::rdf("Class"));
	match(rql::object(rdf::statement(rdf::rdf("Property"), rdf::rdf("type"), rdf::bnode("class"))),
	                  rdf::bnode("class"));
	match(rql::object(rdf::statement(rdf::rdf("Property"), rdf::rdf("label"), rdf::literal("Property"))),
	                  rdf::literal("Property"));
	match(rql::object(rdf::statement(rdf::rdf("Property"), rdf::rdf("label"), rdf::literal("Property", "en"))),
	                  rdf::literal("Property", "en"));
	match(rql::object(rdf::statement(rdf::rdf("Property"), rdf::rdf("label"), rdf::literal("Property", rdf::xsd("string")))),
	                  rdf::literal("Property", rdf::xsd("string")));
}

TEST_CASE("rql::value")
{
	equal(rql::value(rdf::statement(rdf::rdf("Property"), rdf::rdf("type"), rdf::rdf("Class"))),
	                 "");
	equal(rql::value(rdf::statement(rdf::rdf("Property"), rdf::rdf("type"), rdf::bnode("class"))),
	                 "");
	equal(rql::value(rdf::statement(rdf::rdf("Property"), rdf::rdf("label"), rdf::literal("Property"))),
	                 "Property");
	equal(rql::value(rdf::statement(rdf::rdf("Property"), rdf::rdf("label"), rdf::literal("Property", "en"))),
	                 "Property");
	equal(rql::value(rdf::statement(rdf::rdf("Property"), rdf::rdf("label"), rdf::literal("Property", rdf::xsd("string")))),
	                 "Property");
}

TEST_CASE("rql::match")
{
	assert(rql::match<rdf::uri>(rql::subject, rdf::dc("title"))(rdf::statement(rdf::dc("title"), rdf::rdf("type"), rdf::rdf("Property"))));
	assert(rql::match<rdf::uri>(rql::predicate, rdf::rdf("type"))(rdf::statement(rdf::dc("title"), rdf::rdf("type"), rdf::rdf("Property"))));
	assert(rql::match<rdf::uri>(rql::object, rdf::rdf("Property"))(rdf::statement(rdf::dc("title"), rdf::rdf("type"), rdf::rdf("Property"))));

	assert(!rql::match<rdf::uri>(rql::subject, rdf::dc("creator"))(rdf::statement(rdf::dc("title"), rdf::rdf("type"), rdf::rdf("Property"))));
	assert(!rql::match<rdf::uri>(rql::predicate, rdf::rdf("label"))(rdf::statement(rdf::dc("title"), rdf::rdf("type"), rdf::rdf("Property"))));
	assert(!rql::match<rdf::uri>(rql::object, rdf::rdf("Class"))(rdf::statement(rdf::dc("title"), rdf::rdf("type"), rdf::rdf("Property"))));
}

TEST_CASE("rql::select(graph, selector, results)")
{
	rdfdoc dcterms("src/schema/dcterms.rdf");
	equal(dcterms.size(), 867);

	rql::results a;
	rql::select(dcterms, select_all, a);
	equal(a.size(), 867);
	assert(!a.empty());

	rql::results b;
	rql::select(dcterms, select_none, b);
	equal(b.size(), 0);
	assert(b.empty());

	rql::results c;
	rql::select(dcterms, rql::match<rdf::uri>(rql::subject, rdf::dcterms("title")), c);
	equal(c.size(), 9);
	assert(!c.empty());

	match(rql::subject(c.front()), rdf::dcterms("title"));
	match(rql::predicate(c.front()), rdf::rdfs("label"));
	match(rql::object(c.front()), rdf::literal("Title", "en-US"));

	match(rql::subject(c.back()), rdf::dcterms("title"));
	match(rql::predicate(c.back()), rdf::rdfs("subPropertyOf"));
	match(rql::object(c.back()), rdf::dc("title"));
}

TEST_CASE("rql::select(graph, selector, value)")
{
	rdfdoc dcterms("src/schema/dcterms.rdf");
	equal(dcterms.size(), 867);

	rql::results a = rql::select(dcterms, rql::subject, rdf::dcterms("title"));
	equal(a.size(), 9);
	assert(!a.empty());

	match(rql::subject(a.front()), rdf::dcterms("title"));
	match(rql::predicate(a.front()), rdf::rdfs("label"));
	match(rql::object(a.front()), rdf::literal("Title", "en-US"));

	match(rql::subject(a.back()), rdf::dcterms("title"));
	match(rql::predicate(a.back()), rdf::rdfs("subPropertyOf"));
	match(rql::object(a.back()), rdf::dc("title"));

	rql::results b = rql::select(dcterms, rql::predicate, rdf::dcterms("issued"));
	equal(b.size(), 98);
	assert(!b.empty());

	rql::results c = rql::select(dcterms, rql::object, rdf::rdf("Property"));
	equal(c.size(), 55);
	assert(!c.empty());

	rql::results d = rql::select(dcterms, rql::object, rdf::rdf("Class"));
	equal(d.size(), 0);
	assert(d.empty());

	rql::results e = rql::select(a, rql::predicate, rdf::rdf("type"));
	equal(e.size(), 1);
	assert(!e.empty());

	match(rql::subject(e.front()), rdf::dcterms("title"));
	match(rql::predicate(e.front()), rdf::rdf("type"));
	match(rql::object(e.front()), rdf::rdf("Property"));
}

TEST_CASE("rql::contains(graph, selector, value)")
{
	rdfdoc dcterms("src/schema/dcterms.rdf");
	equal(dcterms.size(), 867);

	rql::results a = rql::select(dcterms, rql::subject, rdf::dcterms("title"));
	equal(a.size(), 9);
	assert(!a.empty());

	assert(rql::contains(dcterms, rql::subject, rdf::dcterms("title")));
	assert(!rql::contains(dcterms, rql::subject, rdf::dc("title")));

	assert(rql::contains(a, rql::subject, rdf::dcterms("title")));
	assert(!rql::contains(a, rql::subject, rdf::dcterms("creator")));
}
