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

void test_bnode(const rdf::node &node, const std::string &id)
{
	const rdf::bnode *bnode = dynamic_cast<const rdf::bnode *>(&node);
	assert(bnode);

	equal(bnode->id, id);
}

void test_bnode()
{
	test_bnode(rdf::bnode("a"), "a");
	test_bnode(rdf::bnode("temp"), "temp");
}

void test_uri(const rdf::node &node, const std::string &value, const std::string &ns, const std::string &ref)
{
	const rdf::uri *uri = dynamic_cast<const rdf::uri *>(&node);
	assert(uri);

	equal(uri->str(), value);
	equal(uri->ns,    ns);
	equal(uri->ref,   ref);
}

void test_uri()
{
	test_uri(rdf::uri(std::string(), std::string()), "", "", "");
	test_uri(rdf::uri(std::string(), "test"), "#test", "", "test");

	test_uri(rdf::uri("http://www.w3.org/2001/XMLSchema", std::string()), "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/2001/XMLSchema", "");
	test_uri(rdf::uri("http://www.w3.org/2001/XMLSchema/", std::string()), "http://www.w3.org/2001/XMLSchema/", "http://www.w3.org/2001/XMLSchema/", "");
	test_uri(rdf::uri("http://www.w3.org/2001/XMLSchema#", std::string()), "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/2001/XMLSchema", "");

	test_uri(rdf::uri("http://www.w3.org/2001/XMLSchema", "string"), "http://www.w3.org/2001/XMLSchema#string", "http://www.w3.org/2001/XMLSchema", "string");
	test_uri(rdf::uri("http://www.w3.org/2001/XMLSchema/", "string"), "http://www.w3.org/2001/XMLSchema/string", "http://www.w3.org/2001/XMLSchema/", "string");
	test_uri(rdf::uri("http://www.w3.org/2001/XMLSchema#", "string"), "http://www.w3.org/2001/XMLSchema#string", "http://www.w3.org/2001/XMLSchema", "string");
	test_uri(rdf::uri("http://www.w3.org/2001/XMLSchema/#", "string"), "http://www.w3.org/2001/XMLSchema/#string", "http://www.w3.org/2001/XMLSchema/", "string");
}

void test_namespaces()
{
	test_uri(rdf::rdf("type"), "http://www.w3.org/1999/02/22-rdf-syntax-ns#type", "http://www.w3.org/1999/02/22-rdf-syntax-ns", "type");
	test_uri(rdf::rdfs("Class"), "http://www.w3.org/2000/01/rdf-schema#Class", "http://www.w3.org/2000/01/rdf-schema", "Class");
	test_uri(rdf::xsd("string"), "http://www.w3.org/2001/XMLSchema#string", "http://www.w3.org/2001/XMLSchema", "string");
	test_uri(rdf::xml("lang"), "http://www.w3.org/XML/1998/namespace#lang", "http://www.w3.org/XML/1998/namespace", "lang");
	test_uri(rdf::owl("Ontology"), "http://www.w3.org/2002/07/owl#Ontology", "http://www.w3.org/2002/07/owl", "Ontology");

	test_uri(rdf::dc("title"), "http://purl.org/dc/elements/1.1/title", "http://purl.org/dc/elements/1.1/", "title");
	test_uri(rdf::dcterms("title"), "http://purl.org/dc/terms/title", "http://purl.org/dc/terms/", "title");
	test_uri(rdf::dcam("memberOf"), "http://purl.org/dc/dcam/memberOf", "http://purl.org/dc/dcam/", "memberOf");

	test_uri(rdf::opf("package"), "http://www.idpf.org/2007/opf#package", "http://www.idpf.org/2007/opf", "package");
	test_uri(rdf::ocf("container"), "urn:oasis:names:tc:opendocument:xmlns:container#container", "urn:oasis:names:tc:opendocument:xmlns:container", "container");
	test_uri(rdf::smil("smil"), "http://www.w3.org/ns/SMIL#smil", "http://www.w3.org/ns/SMIL", "smil");
	test_uri(rdf::xhtml("head"), "http://www.w3.org/1999/xhtml#head", "http://www.w3.org/1999/xhtml", "head");

	test_uri(rdf::skos("Concept"), "http://www.w3.org/2004/02/skos/core#Concept", "http://www.w3.org/2004/02/skos/core", "Concept");
	test_uri(rdf::foaf("Person"), "http://xmlns.com/foaf/0.1/Person", "http://xmlns.com/foaf/0.1/", "Person");
	test_uri(rdf::tts("Engine"), "http://rhdunn.github.com/2010/12/text-to-speech#Engine", "http://rhdunn.github.com/2010/12/text-to-speech", "Engine");
}

void test_href()
{
	test_uri(rdf::href("http://www.example.com/"), "http://www.example.com/", "http://www.example.com/", "");
	test_uri(rdf::href("http://www.example.com/value"), "http://www.example.com/value", "http://www.example.com/", "value");

	test_uri(rdf::href("http://www.example.com/abc/"), "http://www.example.com/abc/", "http://www.example.com/abc/", "");
	test_uri(rdf::href("http://www.example.com/abc/value"), "http://www.example.com/abc/value", "http://www.example.com/abc/", "value");

	test_uri(rdf::href("http://www.example.com/def#"), "http://www.example.com/def", "http://www.example.com/def", "");
	test_uri(rdf::href("http://www.example.com/def#value"), "http://www.example.com/def#value", "http://www.example.com/def", "value");

	test_uri(rdf::href("http://www.example.com/def/#"), "http://www.example.com/def/", "http://www.example.com/def/", "");
	test_uri(rdf::href("http://www.example.com/def/#value"), "http://www.example.com/def/#value", "http://www.example.com/def/", "value");
}

void test_literal(const rdf::node &node, const std::string value, const std::string &language, const rdf::uri &uri)
{
	const rdf::literal *literal = dynamic_cast<const rdf::literal *>(&node);
	assert(literal);

	equal(literal->value,      value);
	equal(literal->language,   language);
	equal(literal->type.ns,    uri.ns);
	equal(literal->type.ref,   uri.ref);
}

template<typename T>
void test_literal(const T &value, const std::string &expected, const rdf::uri &uri)
{
	const rdf::uri nulluri = rdf::uri(std::string(), std::string());

	test_literal(rdf::literal(value), expected, std::string(), nulluri);
	test_literal(rdf::literal(value, "en"), expected, "en", nulluri);
	test_literal(rdf::literal(value, "en-GB"), expected, "en-GB", nulluri);
	test_literal(rdf::literal(value, uri), expected, std::string(), uri);
}

void test_literal()
{
	test_literal("This is a test.", "This is a test.", rdf::xsd("string"));
	test_literal("27", "27", rdf::xsd("int"));

	test_literal(std::string("This is a test."), "This is a test.", rdf::xsd("string"));
	test_literal(27, "27", rdf::xsd("int"));
	test_literal(27l, "27", rdf::xsd("int"));
	test_literal(27u, "27", rdf::xsd("int"));
	test_literal(3.2, "3.2", rdf::xsd("string"));
}

void test_item(const rdf::node &a, const rdf::bnode &b)
{
	test_bnode(a, b.id);
}

void test_item(const rdf::node &a, const rdf::uri &b)
{
	test_uri(a, b.str(), b.ns, b.ref);
}

void test_item(const rdf::node &a, const rdf::literal &b)
{
	test_literal(a, b.value, b.language, b.type);
}

template<typename Subject, typename Object>
void test_statement(const rdf::statement &s, const Subject &subject, const rdf::uri &predicate, const Object &object)
{
	test_item(*s.subject, subject);
	test_item(s.predicate, predicate);
	test_item(*s.object, object);
}

void test_statement()
{
	test_statement(rdf::statement(rdf::dc("date"), rdf::rdf("type"), rdf::bnode("tmp")),
	               rdf::dc("date"), rdf::rdf("type"), rdf::bnode("tmp"));

	test_statement(rdf::statement(rdf::dc("date"), rdf::rdf("type"), rdf::xsd("date")),
	               rdf::dc("date"), rdf::rdf("type"), rdf::xsd("date"));

	test_statement(rdf::statement(rdf::rdf("value"), rdf::dc("title"), rdf::literal("value", "en")),
	               rdf::rdf("value"), rdf::dc("title"), rdf::literal("value", "en"));

	test_statement(rdf::statement(rdf::rdf("value"), rdf::dc("title"), rdf::literal("value", rdf::xsd("string"))),
	               rdf::rdf("value"), rdf::dc("title"), rdf::literal("value", rdf::xsd("string")));

	test_statement(rdf::statement(rdf::bnode("a"), rdf::rdf("type"), rdf::bnode("tmp")),
	               rdf::bnode("a"), rdf::rdf("type"), rdf::bnode("tmp"));

	test_statement(rdf::statement(rdf::bnode("b"), rdf::rdf("type"), rdf::xsd("date")),
	               rdf::bnode("b"), rdf::rdf("type"), rdf::xsd("date"));

	test_statement(rdf::statement(rdf::bnode("c"), rdf::dc("title"), rdf::literal("value", "en")),
	               rdf::bnode("c"), rdf::dc("title"), rdf::literal("value", "en"));

	test_statement(rdf::statement(rdf::bnode("d"), rdf::dc("title"), rdf::literal("value", rdf::xsd("string"))),
	               rdf::bnode("d"), rdf::dc("title"), rdf::literal("value", rdf::xsd("string")));
}

void test_model()
{
	rdf::graph model;
	assert(model.empty());
	equal(model.size(), 0);
	assert(model.begin() == model.end());

	model.push_back(rdf::statement(rdf::rdf("value"), rdf::rdf("type"), rdf::rdfs("Property")));
	assert(!model.empty());
	equal(model.size(), 1);
	assert(model.begin() != model.end());

	test_statement(model.front(), rdf::rdf("value"), rdf::rdf("type"), rdf::rdfs("Property"));
	test_statement(model.back(), rdf::rdf("value"), rdf::rdf("type"), rdf::rdfs("Property"));

	model.push_back(rdf::statement(rdf::rdf("value"), rdf::rdf("value"), rdf::literal("value", "en-GB")));
	assert(!model.empty());
	equal(model.size(), 2);
	assert(model.begin() != model.end());

	test_statement(model.front(), rdf::rdf("value"), rdf::rdf("type"), rdf::rdfs("Property"));
	test_statement(model.back(), rdf::rdf("value"), rdf::rdf("value"), rdf::literal("value", "en-GB"));
}

void test_model_namespace(const rdf::statement &s, const rdf::ns &ns)
{
	rdf::graph model;
	assert(!model.contains(ns));
	assert(!model.contains(rdf::tts));

	model.push_back(s);
	assert(model.contains(ns));
	assert(!model.contains(rdf::tts));
}

void test_model_namespaces()
{
	test_model_namespace(rdf::statement(rdf::dc("title"), rdf::rdf("type"), rdf::rdfs("Property")), rdf::dc);
	test_model_namespace(rdf::statement(rdf::dc("title"), rdf::rdf("type"), rdf::rdfs("Property")), rdf::rdf);
	test_model_namespace(rdf::statement(rdf::dc("title"), rdf::rdf("type"), rdf::rdfs("Property")), rdf::rdfs);

	test_model_namespace(rdf::statement(rdf::rdfs("Property"), rdf::rdf("value"), rdf::literal("Property")), rdf::rdf);
	test_model_namespace(rdf::statement(rdf::rdfs("Property"), rdf::rdf("value"), rdf::literal("Property")), rdf::rdfs);

	test_model_namespace(rdf::statement(rdf::rdfs("Property"), rdf::rdf("value"), rdf::literal("Property", rdf::xsd("string"))), rdf::rdf);
	test_model_namespace(rdf::statement(rdf::rdfs("Property"), rdf::rdf("value"), rdf::literal("Property", rdf::xsd("string"))), rdf::rdfs);
	test_model_namespace(rdf::statement(rdf::rdfs("Property"), rdf::rdf("value"), rdf::literal("Property", rdf::xsd("string"))), rdf::xsd);

	test_model_namespace(rdf::statement(rdf::rdfs("Property"), rdf::dc("creator"), rdf::bnode("a")), rdf::dc);
	test_model_namespace(rdf::statement(rdf::rdfs("Property"), rdf::dc("creator"), rdf::bnode("a")), rdf::rdfs);

	test_model_namespace(rdf::statement(rdf::bnode("a"), rdf::rdf("value"),  rdf::literal("test")), rdf::rdf);

	test_model_namespace(rdf::statement(rdf::bnode("a"), rdf::rdf("value"),  rdf::literal("test", rdf::xsd("string"))), rdf::rdf);
	test_model_namespace(rdf::statement(rdf::bnode("a"), rdf::rdf("value"),  rdf::literal("test", rdf::xsd("string"))), rdf::xsd);

	test_model_namespace(rdf::statement(rdf::bnode("a"), rdf::rdf("type"),   rdf::rdfs("Class")), rdf::rdf);
	test_model_namespace(rdf::statement(rdf::bnode("a"), rdf::rdf("type"),   rdf::rdfs("Class")), rdf::rdfs);

	test_model_namespace(rdf::statement(rdf::bnode("a"), rdf::dc("creator"), rdf::bnode("Class")), rdf::dc);
}

void test_genid()
{
	rdf::graph model1;
	rdf::graph model2;

	test_bnode(model1.genid(), "genid1");
	test_bnode(model1.genid(), "genid2");
	test_bnode(model1.genid(), "genid3");
	test_bnode(model2.genid(), "genid1");
	test_bnode(model2.genid(), "genid2");
	test_bnode(model1.genid(), "genid4");
	test_bnode(model1.genid(), "genid5");
	test_bnode(model2.genid(), "genid3");
	test_bnode(model2.genid(), "genid4");
}

int main(int argc, char ** argv)
{
	test_bnode();
	test_uri();
	test_namespaces();
	test_href();
	test_literal();
	test_statement();
	test_model();
	test_model_namespaces();
	test_genid();

	return 0;
}
