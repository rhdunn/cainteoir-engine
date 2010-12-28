/* RDF NTriple formatter tests.
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
#include <sstream>
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

template<typename T>
std::string format(const T &value)
{
	std::ostringstream s;
	(*rdf::create_formatter(s, rdf::formatter::ntriple))
		<< value;
	return s.str();
}

template<typename T>
std::string format_ns(const T &value)
{
	std::ostringstream s;
	(*rdf::create_formatter(s, rdf::formatter::ntriple))
		<< rdf::rdf << rdf::rdfs << rdf::xsd
		<< value;
	return s.str();
}

void test_bnode()
{
	equal(format(rdf::bnode("id1234")), "_:id1234");

	equal(format_ns(rdf::bnode("id1234")), "_:id1234");
}

void test_uri()
{
	equal(format(rdf::rdf("type")), "<http://www.w3.org/1999/02/22-rdf-syntax-ns#type>");

	equal(format_ns(rdf::rdf("type")), "<http://www.w3.org/1999/02/22-rdf-syntax-ns#type>");
}

void test_literal()
{
	equal(format(rdf::literal("Nevermore!")), "\"Nevermore!\"");
	equal(format(rdf::literal("Nevermore!", "en")), "\"Nevermore!\"@en");
	equal(format(rdf::literal("Nevermore!", rdf::xsd("string"))), "\"Nevermore!\"^^<http://www.w3.org/2001/XMLSchema#string>");

	equal(format_ns(rdf::literal("Nevermore!")), "\"Nevermore!\"");
	equal(format_ns(rdf::literal("Nevermore!", "en")), "\"Nevermore!\"@en");
	equal(format_ns(rdf::literal("Nevermore!", rdf::xsd("string"))), "\"Nevermore!\"^^<http://www.w3.org/2001/XMLSchema#string>");
}

void test_statement()
{
	equal(format(rdf::statement(rdf::rdfs("Class"), rdf::rdf("value"), rdf::literal("Class"))),
	      "<http://www.w3.org/2000/01/rdf-schema#Class> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\" .\n");
	equal(format(rdf::statement(rdf::rdfs("Class"), rdf::rdf("value"), rdf::literal("Class", "en-GB"))),
	      "<http://www.w3.org/2000/01/rdf-schema#Class> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\"@en-GB .\n");
	equal(format(rdf::statement(rdf::rdfs("Class"), rdf::rdf("value"), rdf::literal("Class", rdf::xsd("string")))),
	      "<http://www.w3.org/2000/01/rdf-schema#Class> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\"^^<http://www.w3.org/2001/XMLSchema#string> .\n");

	equal(format(rdf::statement(rdf::bnode("a"), rdf::rdf("value"), rdf::literal("Class"))),
	      "_:a <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\" .\n");
	equal(format(rdf::statement(rdf::bnode("a"), rdf::rdf("value"), rdf::literal("Class", "en-GB"))),
	      "_:a <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\"@en-GB .\n");
	equal(format(rdf::statement(rdf::bnode("a"), rdf::rdf("value"), rdf::literal("Class", rdf::xsd("string")))),
	      "_:a <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\"^^<http://www.w3.org/2001/XMLSchema#string> .\n");

	equal(format(rdf::statement(rdf::rdfs("Class"), rdf::rdf("type"), rdf::bnode("tmp"))),
	      "<http://www.w3.org/2000/01/rdf-schema#Class> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> _:tmp .\n");
	equal(format(rdf::statement(rdf::rdfs("Property"), rdf::rdf("type"), rdf::rdfs("Class"))),
	      "<http://www.w3.org/2000/01/rdf-schema#Property> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://www.w3.org/2000/01/rdf-schema#Class> .\n");

	equal(format(rdf::statement(rdf::bnode("a"), rdf::rdf("type"), rdf::bnode("tmp"))),
	      "_:a <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> _:tmp .\n");
	equal(format(rdf::statement(rdf::bnode("a"), rdf::rdf("type"), rdf::rdfs("Class"))),
	      "_:a <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://www.w3.org/2000/01/rdf-schema#Class> .\n");

	equal(format_ns(rdf::statement(rdf::rdfs("Class"), rdf::rdf("value"), rdf::literal("Class"))),
	      "<http://www.w3.org/2000/01/rdf-schema#Class> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\" .\n");
	equal(format_ns(rdf::statement(rdf::rdfs("Class"), rdf::rdf("value"), rdf::literal("Class", "en-GB"))),
	      "<http://www.w3.org/2000/01/rdf-schema#Class> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\"@en-GB .\n");
	equal(format_ns(rdf::statement(rdf::rdfs("Class"), rdf::rdf("value"), rdf::literal("Class", rdf::xsd("string")))),
	      "<http://www.w3.org/2000/01/rdf-schema#Class> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\"^^<http://www.w3.org/2001/XMLSchema#string> .\n");

	equal(format_ns(rdf::statement(rdf::bnode("a"), rdf::rdf("value"), rdf::literal("Class"))),
	      "_:a <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\" .\n");
	equal(format_ns(rdf::statement(rdf::bnode("a"), rdf::rdf("value"), rdf::literal("Class", "en-GB"))),
	      "_:a <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\"@en-GB .\n");
	equal(format_ns(rdf::statement(rdf::bnode("a"), rdf::rdf("value"), rdf::literal("Class", rdf::xsd("string")))),
	      "_:a <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\"^^<http://www.w3.org/2001/XMLSchema#string> .\n");

	equal(format_ns(rdf::statement(rdf::rdfs("Class"), rdf::rdf("type"), rdf::bnode("tmp"))),
	      "<http://www.w3.org/2000/01/rdf-schema#Class> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> _:tmp .\n");
	equal(format_ns(rdf::statement(rdf::rdfs("Property"), rdf::rdf("type"), rdf::rdfs("Class"))),
	      "<http://www.w3.org/2000/01/rdf-schema#Property> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://www.w3.org/2000/01/rdf-schema#Class> .\n");

	equal(format_ns(rdf::statement(rdf::bnode("a"), rdf::rdf("type"), rdf::bnode("tmp"))),
	      "_:a <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> _:tmp .\n");
	equal(format_ns(rdf::statement(rdf::bnode("a"), rdf::rdf("type"), rdf::rdfs("Class"))),
	      "_:a <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://www.w3.org/2000/01/rdf-schema#Class> .\n");
}

void test_model()
{
	rdf::model model;
	model.push_back(rdf::statement(rdf::rdfs("Property"), rdf::rdf("value"), rdf::literal("Property")));
	model.push_back(rdf::statement(rdf::rdfs("Property"), rdf::rdf("value"), rdf::literal("Property", "en")));
	model.push_back(rdf::statement(rdf::rdfs("Property"), rdf::rdf("value"), rdf::literal("Property", rdf::xsd("string"))));
	model.push_back(rdf::statement(rdf::rdfs("Property"), rdf::rdf("type"), rdf::rdfs("Class")));

	equal(format(model),
	      "<http://www.w3.org/2000/01/rdf-schema#Property> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Property\" .\n"
	      "<http://www.w3.org/2000/01/rdf-schema#Property> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Property\"@en .\n"
	      "<http://www.w3.org/2000/01/rdf-schema#Property> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Property\"^^<http://www.w3.org/2001/XMLSchema#string> .\n"
	      "<http://www.w3.org/2000/01/rdf-schema#Property> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://www.w3.org/2000/01/rdf-schema#Class> .\n"
	     );

	equal(format_ns(model),
	      "<http://www.w3.org/2000/01/rdf-schema#Property> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Property\" .\n"
	      "<http://www.w3.org/2000/01/rdf-schema#Property> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Property\"@en .\n"
	      "<http://www.w3.org/2000/01/rdf-schema#Property> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Property\"^^<http://www.w3.org/2001/XMLSchema#string> .\n"
	      "<http://www.w3.org/2000/01/rdf-schema#Property> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://www.w3.org/2000/01/rdf-schema#Class> .\n"
	     );
}

int main(int argc, char ** argv)
{
	test_uri();
	test_literal();
	test_statement();
	test_model();

	return 0;
}
