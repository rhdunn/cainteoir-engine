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

void test_statement(const rdf::uri &subject, const rdf::uri &predicate, const rdf::literal &object)
{
	const rdf::statement s(subject, predicate, object);

	test_uri(s.subject, subject.value, subject.ns, subject.ref);
	test_uri(s.predicate, predicate.value, predicate.ns, predicate.ref);
	test_literal(*s.object, object.value, object.language, object.type);
}

void test_statement(const rdf::uri &subject, const rdf::uri &predicate, const rdf::uri &object)
{
	const rdf::statement s(subject, predicate, object);

	test_uri(s.subject, subject.value, subject.ns, subject.ref);
	test_uri(s.predicate, predicate.value, predicate.ns, predicate.ref);
	test_uri(*s.object, object.value, object.ns, object.ref);
}

int main(int argc, char ** argv)
{
	test_statement(rdf::rdf("value"), rdf::dc("title"), rdf::literal("value", "en"));
	test_statement(rdf::dc("date"), rdf::rdf("type"), rdf::xsd("date"));

	return 0;
}
