/* RDF statement tests.
 *
 * Copyright (C) 2010 Reece H. Dunn
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

namespace rdf = cainteoir::rdf;

template<typename T1, typename T2>
void equal_(const char *fn, const char *ref, const T1 &a, const T2 &b, int lineno)
{
	if (a != b)
	{
		std::cout << fn << " @ line " << lineno << " : " << ref << " -- types are not equal" << std::endl
		          << "   expected: " << b << std::endl
		          << "   actual:   " << a << std::endl
		          ;
		exit(1);
	}
}

#define equal(a, b) equal_(__FUNCTION__, #a, a, b, __LINE__)

void test_uri(const rdf::node &node, const std::string &value, const std::string &ns, const std::string &ref)
{
	const rdf::uri *uri = dynamic_cast<const rdf::uri *>(&node);
	assert(uri);

	equal(uri->value, value);
	equal(uri->ns,    ns);
	equal(uri->ref,   ref);
}

void test_uri()
{
	test_uri(rdf::rdf("type"), "http://www.w3.org/1999/02/22-rdf-syntax-ns#type", "http://www.w3.org/1999/02/22-rdf-syntax-ns#", "type");
	test_uri(rdf::rdfs("Class"), "http://www.w3.org/2000/01/rdf-schema#Class", "http://www.w3.org/2000/01/rdf-schema#", "Class");
	test_uri(rdf::xsd("string"), "http://www.w3.org/2001/XMLSchema#string", "http://www.w3.org/2001/XMLSchema#", "string");
	test_uri(rdf::dc("title"), "http://purl.org/dc/elements/1.1/title", "http://purl.org/dc/elements/1.1/", "title");
	test_uri(rdf::dcterms("title"), "http://purl.org/dc/terms/title", "http://purl.org/dc/terms/", "title");
}

void test_literal(const rdf::node &node, const std::string value, const std::string &language, const rdf::uri &uri)
{
	const rdf::literal *literal = dynamic_cast<const rdf::literal *>(&node);
	assert(literal);

	equal(literal->value,      value);
	equal(literal->language,   language);
	equal(literal->type.value, uri.value);
	equal(literal->type.ns,    uri.ns);
	equal(literal->type.ref,   uri.ref);
}

void test_literal()
{
	const rdf::uri nulluri = rdf::uri(std::string(), std::string());

	test_literal(rdf::literal("This is a test."), "This is a test.", std::string(), nulluri);
	test_literal(rdf::literal("This is a test.", "en"), "This is a test.", "en", nulluri);
	test_literal(rdf::literal("This is a test.", "en-GB"), "This is a test.", "en-GB", nulluri);
	test_literal(rdf::literal("This is a test.", "en-GB", rdf::xsd("string")), "This is a test.", std::string(), rdf::xsd("string"));
	test_literal(rdf::literal("This is a test.", rdf::xsd("string")), "This is a test.", std::string(), rdf::xsd("string"));
}

void test_statement(const rdf::statement &s, const rdf::uri &subject, const rdf::uri &predicate, const rdf::literal &object)
{
	test_uri(s.subject, subject.value, subject.ns, subject.ref);
	test_uri(s.predicate, predicate.value, predicate.ns, predicate.ref);
	test_literal(*s.object, object.value, object.language, object.type);
}

void test_statement(const rdf::statement &s, const rdf::uri &subject, const rdf::uri &predicate, const rdf::uri &object)
{
	test_uri(s.subject, subject.value, subject.ns, subject.ref);
	test_uri(s.predicate, predicate.value, predicate.ns, predicate.ref);
	test_uri(*s.object, object.value, object.ns, object.ref);
}

void test_statement()
{
	test_statement(rdf::statement(rdf::rdf("value"), rdf::dc("title"), rdf::literal("value", "en")),
	               rdf::rdf("value"), rdf::dc("title"), rdf::literal("value", "en"));
	test_statement(rdf::statement(rdf::dc("date"), rdf::rdf("type"), rdf::xsd("date")),
	               rdf::dc("date"), rdf::rdf("type"), rdf::xsd("date"));
}

void test_model()
{
	rdf::model model;
	assert(model.empty());
	equal(model.size(), 0);
	assert(model.begin() == model.end());

	model.push_back(rdf::statement(rdf::rdf("value"), rdf::rdf("type"), rdf::rdfs("Property")));
	assert(!model.empty());
	equal(model.size(), 1);
	assert(model.begin() != model.end());

	test_statement(model.front(), rdf::rdf("value"), rdf::rdf("type"), rdf::rdfs("Property"));
	test_statement(model.back(), rdf::rdf("value"), rdf::rdf("type"), rdf::rdfs("Property"));

	model.push_back(rdf::statement(rdf::rdf("value"), rdf::rdf("value"), rdf::literal("value", "en-GB", rdf::xsd("string"))));
	assert(!model.empty());
	equal(model.size(), 2);
	assert(model.begin() != model.end());

	test_statement(model.front(), rdf::rdf("value"), rdf::rdf("type"), rdf::rdfs("Property"));
	test_statement(model.back(), rdf::rdf("value"), rdf::rdf("value"), rdf::literal("value", "en-GB", rdf::xsd("string")));
}

int main(int argc, char ** argv)
{
	test_uri();
	test_literal();
	test_statement();
	test_model();

	return 0;
}
