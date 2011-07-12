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
