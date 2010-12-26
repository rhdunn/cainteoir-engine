/* RDF URI tests.
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

void test_rdfns()
{
	const rdf::uri test = rdf::rdf("type");
	equal(test.value, "http://www.w3.org/1999/02/22-rdf-syntax-ns#type");
	equal(test.ns,    "http://www.w3.org/1999/02/22-rdf-syntax-ns#");
	equal(test.ref,   "type");
}

void test_rdfsns()
{
	const rdf::uri test = rdf::rdfs("Class");
	equal(test.value, "http://www.w3.org/2000/01/rdf-schema#Class");
	equal(test.ns,    "http://www.w3.org/2000/01/rdf-schema#");
	equal(test.ref,   "Class");
}

void test_xsdns()
{
	const rdf::uri test = rdf::xsd("string");
	equal(test.value, "http://www.w3.org/2001/XMLSchema#string");
	equal(test.ns,    "http://www.w3.org/2001/XMLSchema#");
	equal(test.ref,   "string");
}

void test_dcns()
{
	const rdf::uri test = rdf::dc("title");
	equal(test.value, "http://purl.org/dc/elements/1.1/title");
	equal(test.ns,    "http://purl.org/dc/elements/1.1/");
	equal(test.ref,   "title");
}

void test_dctermsns()
{
	const rdf::uri test = rdf::dc("title");
	equal(test.value, "http://purl.org/dc/terms/title");
	equal(test.ns,    "http://purl.org/dc/terms/");
	equal(test.ref,   "title");
}

int main(int argc, char ** argv)
{
	test_rdfns();
	test_rdfsns();
	test_xsdns();
	test_dcns();

	return 0;
}
