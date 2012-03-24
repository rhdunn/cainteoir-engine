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
#include <sstream>
#include <cstdlib>

#include "tester.hpp"

namespace rdf = cainteoir::rdf;

REGISTER_TESTSUITE("RDF NTriple Formatter");

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
		<< rdf::rdf << rdf::rdfs << rdf::xsd << rdf::dc << rdf::opf
		<< value;
	return s.str();
}

TEST_CASE("rdf::bnode")
{
	assert(format(rdf::bnode("id1234"))    == "_:id1234");
	assert(format_ns(rdf::bnode("id1234")) == "_:id1234");
}

TEST_CASE("rdf::uri")
{
	assert(format(rdf::rdf("type"))    == "<http://www.w3.org/1999/02/22-rdf-syntax-ns#type>");
	assert(format_ns(rdf::rdf("type")) == "<http://www.w3.org/1999/02/22-rdf-syntax-ns#type>");
}

TEST_CASE("rdf::literal")
{
	assert(format(rdf::literal("Nevermore!")) == "\"Nevermore!\"");
	assert(format(rdf::literal("Nevermore!", "en")) == "\"Nevermore!\"@en");
	assert(format(rdf::literal("Nevermore!", rdf::xsd("string"))) == "\"Nevermore!\"^^<http://www.w3.org/2001/XMLSchema#string>");

	assert(format(rdf::literal("Quoth the raven: \"Nevermore!\"")) == "\"Quoth the raven: \\\"Nevermore!\\\"\"");
	assert(format(rdf::literal("Quoth the raven: \"Nevermore!\"", "en")) == "\"Quoth the raven: \\\"Nevermore!\\\"\"@en");
	assert(format(rdf::literal("Quoth the raven: \"Nevermore!\"", rdf::xsd("string"))) == "\"Quoth the raven: \\\"Nevermore!\\\"\"^^<http://www.w3.org/2001/XMLSchema#string>");

	assert(format_ns(rdf::literal("Nevermore!")) == "\"Nevermore!\"");
	assert(format_ns(rdf::literal("Nevermore!", "en")) == "\"Nevermore!\"@en");
	assert(format_ns(rdf::literal("Nevermore!", rdf::xsd("string"))) == "\"Nevermore!\"^^<http://www.w3.org/2001/XMLSchema#string>");

	assert(format_ns(rdf::literal("Quoth the raven: \"Nevermore!\"")) == "\"Quoth the raven: \\\"Nevermore!\\\"\"");
	assert(format_ns(rdf::literal("Quoth the raven: \"Nevermore!\"", "en")) == "\"Quoth the raven: \\\"Nevermore!\\\"\"@en");
	assert(format_ns(rdf::literal("Quoth the raven: \"Nevermore!\"", rdf::xsd("string"))) == "\"Quoth the raven: \\\"Nevermore!\\\"\"^^<http://www.w3.org/2001/XMLSchema#string>");
}

TEST_CASE("rdf::statement")
{
	rdf::graph g;

	assert(g.statement(rdf::rdfs("Class"), rdf::rdf("value"), rdf::literal("Class")));
	assert(format(g.back())    == "<http://www.w3.org/2000/01/rdf-schema#Class> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\" .\n");
	assert(format_ns(g.back()) == "<http://www.w3.org/2000/01/rdf-schema#Class> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\" .\n");

	assert(g.statement(rdf::rdfs("Class"), rdf::rdf("value"), rdf::literal("Class", "en-GB")));
	assert(format(g.back())    == "<http://www.w3.org/2000/01/rdf-schema#Class> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\"@en-GB .\n");
	assert(format_ns(g.back()) == "<http://www.w3.org/2000/01/rdf-schema#Class> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\"@en-GB .\n");

	assert(g.statement(rdf::rdfs("Class"), rdf::rdf("value"), rdf::literal("Class", rdf::xsd("string"))));
	assert(format(g.back())    == "<http://www.w3.org/2000/01/rdf-schema#Class> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\"^^<http://www.w3.org/2001/XMLSchema#string> .\n");
	assert(format_ns(g.back()) == "<http://www.w3.org/2000/01/rdf-schema#Class> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\"^^<http://www.w3.org/2001/XMLSchema#string> .\n");

	assert(g.statement(rdf::bnode("a"), rdf::rdf("value"), rdf::literal("Class")));
	assert(format(g.back())    == "_:a <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\" .\n");
	assert(format_ns(g.back()) == "_:a <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\" .\n");

	assert(g.statement(rdf::bnode("a"), rdf::rdf("value"), rdf::literal("Class", "en-GB")));
	assert(format(g.back())    == "_:a <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\"@en-GB .\n");
	assert(format_ns(g.back()) == "_:a <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\"@en-GB .\n");

	assert(g.statement(rdf::bnode("a"), rdf::rdf("value"), rdf::literal("Class", rdf::xsd("string"))));
	assert(format(g.back())    == "_:a <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\"^^<http://www.w3.org/2001/XMLSchema#string> .\n");
	assert(format_ns(g.back()) == "_:a <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Class\"^^<http://www.w3.org/2001/XMLSchema#string> .\n");

	assert(g.statement(rdf::rdfs("Class"), rdf::rdf("type"), rdf::bnode("tmp")));
	assert(format(g.back())    == "<http://www.w3.org/2000/01/rdf-schema#Class> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> _:tmp .\n");
	assert(format_ns(g.back()) == "<http://www.w3.org/2000/01/rdf-schema#Class> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> _:tmp .\n");

	assert(g.statement(rdf::rdfs("Property"), rdf::rdf("type"), rdf::rdfs("Class")));
	assert(format(g.back())    == "<http://www.w3.org/2000/01/rdf-schema#Property> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://www.w3.org/2000/01/rdf-schema#Class> .\n");
	assert(format_ns(g.back()) == "<http://www.w3.org/2000/01/rdf-schema#Property> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://www.w3.org/2000/01/rdf-schema#Class> .\n");

	assert(g.statement(rdf::bnode("a"), rdf::rdf("type"), rdf::bnode("tmp")));
	assert(format(g.back())    == "_:a <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> _:tmp .\n");
	assert(format_ns(g.back()) == "_:a <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> _:tmp .\n");

	assert(g.statement(rdf::bnode("a"), rdf::rdf("type"), rdf::rdfs("Class")));
	assert(format(g.back())    == "_:a <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://www.w3.org/2000/01/rdf-schema#Class> .\n");
	assert(format_ns(g.back()) == "_:a <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://www.w3.org/2000/01/rdf-schema#Class> .\n");
}

TEST_CASE("rdf::graph")
{
	rdf::graph model;
	model.statement(rdf::rdfs("Property"), rdf::rdf("value"), rdf::literal("Property"));
	model.statement(rdf::rdfs("Property"), rdf::rdf("value"), rdf::literal("Property", "en"));
	model.statement(rdf::rdfs("Property"), rdf::rdf("value"), rdf::literal("Property", rdf::xsd("string")));
	model.statement(rdf::rdfs("Property"), rdf::rdf("type"), rdf::rdfs("Class"));

	assert(format(model) ==
	      "<http://www.w3.org/2000/01/rdf-schema#Property> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Property\" .\n"
	      "<http://www.w3.org/2000/01/rdf-schema#Property> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Property\"@en .\n"
	      "<http://www.w3.org/2000/01/rdf-schema#Property> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Property\"^^<http://www.w3.org/2001/XMLSchema#string> .\n"
	      "<http://www.w3.org/2000/01/rdf-schema#Property> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://www.w3.org/2000/01/rdf-schema#Class> .\n"
	     );

	assert(format_ns(model) ==
	      "<http://www.w3.org/2000/01/rdf-schema#Property> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Property\" .\n"
	      "<http://www.w3.org/2000/01/rdf-schema#Property> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Property\"@en .\n"
	      "<http://www.w3.org/2000/01/rdf-schema#Property> <http://www.w3.org/1999/02/22-rdf-syntax-ns#value> \"Property\"^^<http://www.w3.org/2001/XMLSchema#string> .\n"
	      "<http://www.w3.org/2000/01/rdf-schema#Property> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://www.w3.org/2000/01/rdf-schema#Class> .\n"
	     );
}
