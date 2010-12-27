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
std::string ntriple(const T &value)
{
	std::ostringstream s;
	*rdf::ntriple_formatter(s) << value;
	return s.str();
}

void test_uri()
{
	equal(ntriple(rdf::rdf("type")), "<http://www.w3.org/1999/02/22-rdf-syntax-ns#type>");
}

void test_literal()
{
	equal(ntriple(rdf::literal("Nevermore!")), "\"Nevermore!\"");
	equal(ntriple(rdf::literal("Nevermore!", "en")), "\"Nevermore!\"@en");
	equal(ntriple(rdf::literal("Nevermore!", rdf::xsd("string"))), "\"Nevermore!\"^^<http://www.w3.org/2001/XMLSchema#string>");
	equal(ntriple(rdf::literal("Nevermore!", "en", rdf::xsd("string"))), "\"Nevermore!\"@en^^<http://www.w3.org/2001/XMLSchema#string>");
}

void test_statement()
{
	equal(ntriple(rdf::statement(rdf::rdfs("Class"), rdf::rdf("value"), rdf::literal("Class"))),
	      "<http://www.w3.org/2000/01/rdf-schema#Class> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\" .\n");
	equal(ntriple(rdf::statement(rdf::rdfs("Class"), rdf::rdf("value"), rdf::literal("Class", "en-GB"))),
	      "<http://www.w3.org/2000/01/rdf-schema#Class> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\"@en-GB .\n");
	equal(ntriple(rdf::statement(rdf::rdfs("Class"), rdf::rdf("value"), rdf::literal("Class", rdf::xsd("string")))),
	      "<http://www.w3.org/2000/01/rdf-schema#Class> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\"^^<http://www.w3.org/2001/XMLSchema#string> .\n");
	equal(ntriple(rdf::statement(rdf::rdfs("Class"), rdf::rdf("value"), rdf::literal("Class", "en", rdf::xsd("string")))),
	      "<http://www.w3.org/2000/01/rdf-schema#Class> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\"@en^^<http://www.w3.org/2001/XMLSchema#string> .\n");

	equal(ntriple(rdf::statement(rdf::rdfs("Property"), rdf::rdf("type"), rdf::rdfs("Class"))),
	      "<http://www.w3.org/2000/01/rdf-schema#Property> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://www.w3.org/2000/01/rdf-schema#Class> .\n");
}

void test_model()
{
	rdf::model model;
	model.push_back(rdf::statement(rdf::rdfs("Property"), rdf::rdf("value"), rdf::literal("Property")));
	model.push_back(rdf::statement(rdf::rdfs("Property"), rdf::rdf("value"), rdf::literal("Property", "en")));
	model.push_back(rdf::statement(rdf::rdfs("Property"), rdf::rdf("value"), rdf::literal("Property", rdf::xsd("string"))));
	model.push_back(rdf::statement(rdf::rdfs("Property"), rdf::rdf("value"), rdf::literal("Property", "en", rdf::xsd("string"))));
	model.push_back(rdf::statement(rdf::rdfs("Property"), rdf::rdf("type"), rdf::rdfs("Class")));

	equal(ntriple(model),
	      "<http://www.w3.org/2000/01/rdf-schema#Property> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Property\" .\n"
	      "<http://www.w3.org/2000/01/rdf-schema#Property> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Property\"@en .\n"
	      "<http://www.w3.org/2000/01/rdf-schema#Property> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Property\"^^<http://www.w3.org/2001/XMLSchema#string> .\n"
	      "<http://www.w3.org/2000/01/rdf-schema#Property> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Property\"@en^^<http://www.w3.org/2001/XMLSchema#string> .\n"
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
