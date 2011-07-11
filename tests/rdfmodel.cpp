/* RDF statement tests.
 *
 * Copyright (C) 2010-2011 Reece H. Dunn
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

REGISTER_TESTSUITE("RDF Model");

void test_bnode(const rdf::node &node, const std::string &id)
{
	const rdf::bnode *bnode = dynamic_cast<const rdf::bnode *>(&node);
	if (assert(bnode))
	{
		equal(bnode->id, id);
	}
}

TEST_CASE("rdf::bnode")
{
	test_bnode(rdf::bnode("a"), "a");
	test_bnode(rdf::bnode("temp"), "temp");
}

void test_uri(const rdf::node &node, const std::string &value, const std::string &ns, const std::string &ref)
{
	const rdf::uri *uri = dynamic_cast<const rdf::uri *>(&node);
	if (assert(uri))
	{
		equal(uri->str(), value);
		equal(uri->ns,    ns);
		equal(uri->ref,   ref);
	}
}

TEST_CASE("rdf::uri")
{
	test_uri(rdf::uri(std::string(), std::string()), "", "", "");
	test_uri(rdf::uri(std::string(), "test"), "#test", "#", "test");

	test_uri(rdf::uri("http://www.w3.org/2001/XMLSchema", std::string()), "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/2001/XMLSchema", "");
	test_uri(rdf::uri("http://www.w3.org/2001/XMLSchema/", std::string()), "http://www.w3.org/2001/XMLSchema/", "http://www.w3.org/2001/XMLSchema/", "");
	test_uri(rdf::uri("http://www.w3.org/2001/XMLSchema#", std::string()), "http://www.w3.org/2001/XMLSchema#", "http://www.w3.org/2001/XMLSchema#", "");

	test_uri(rdf::uri("http://www.w3.org/2001/XMLSchema", "string"), "http://www.w3.org/2001/XMLSchema#string", "http://www.w3.org/2001/XMLSchema#", "string");
	test_uri(rdf::uri("http://www.w3.org/2001/XMLSchema/", "string"), "http://www.w3.org/2001/XMLSchema/string", "http://www.w3.org/2001/XMLSchema/", "string");
	test_uri(rdf::uri("http://www.w3.org/2001/XMLSchema#", "string"), "http://www.w3.org/2001/XMLSchema#string", "http://www.w3.org/2001/XMLSchema#", "string");
	test_uri(rdf::uri("http://www.w3.org/2001/XMLSchema/#", "string"), "http://www.w3.org/2001/XMLSchema/#string", "http://www.w3.org/2001/XMLSchema/#", "string");
}

TEST_CASE("RDF namespaces")
{
	test_uri(rdf::rdf("type"), "http://www.w3.org/1999/02/22-rdf-syntax-ns#type", "http://www.w3.org/1999/02/22-rdf-syntax-ns#", "type");
	test_uri(rdf::rdfa("Error"), "http://www.w3.org/ns/rdfa#Error", "http://www.w3.org/ns/rdfa#", "Error");
	test_uri(rdf::rdfs("Class"), "http://www.w3.org/2000/01/rdf-schema#Class", "http://www.w3.org/2000/01/rdf-schema#", "Class");
	test_uri(rdf::xsd("string"), "http://www.w3.org/2001/XMLSchema#string", "http://www.w3.org/2001/XMLSchema#", "string");
	test_uri(rdf::xml("lang"), "http://www.w3.org/XML/1998/namespace#lang", "http://www.w3.org/XML/1998/namespace#", "lang");
	test_uri(rdf::owl("Ontology"), "http://www.w3.org/2002/07/owl#Ontology", "http://www.w3.org/2002/07/owl#", "Ontology");

	test_uri(rdf::dc("title"), "http://purl.org/dc/elements/1.1/title", "http://purl.org/dc/elements/1.1/", "title");
	test_uri(rdf::dcterms("title"), "http://purl.org/dc/terms/title", "http://purl.org/dc/terms/", "title");
	test_uri(rdf::dcam("memberOf"), "http://purl.org/dc/dcam/memberOf", "http://purl.org/dc/dcam/", "memberOf");

	test_uri(rdf::dtb("uid"), "http://www.daisy.org/z3986/2005/dtbook/uid", "http://www.daisy.org/z3986/2005/dtbook/", "uid");
	test_uri(rdf::ncx("meta"), "http://www.daisy.org/z3986/2005/ncx/meta", "http://www.daisy.org/z3986/2005/ncx/", "meta");

	test_uri(rdf::epub("switch"), "http://www.idpf.org/2007/ops#switch", "http://www.idpf.org/2007/ops#", "switch");
	test_uri(rdf::opf("package"), "http://www.idpf.org/2007/opf#package", "http://www.idpf.org/2007/opf#", "package");
	test_uri(rdf::ocf("container"), "urn:oasis:names:tc:opendocument:xmlns:container#container", "urn:oasis:names:tc:opendocument:xmlns:container#", "container");
	test_uri(rdf::pkg("role"), "http://www.idpf.org/epub/vocab/package/#role", "http://www.idpf.org/epub/vocab/package/#", "role");
	test_uri(rdf::media("duration"), "http://www.idpf.org/epub/vocab/overlays/#duration", "http://www.idpf.org/epub/vocab/overlays/#", "duration");

	test_uri(rdf::smil("smil"), "http://www.w3.org/ns/SMIL#smil", "http://www.w3.org/ns/SMIL#", "smil");
	test_uri(rdf::xhtml("head"), "http://www.w3.org/1999/xhtml#head", "http://www.w3.org/1999/xhtml#", "head");

	test_uri(rdf::skos("Concept"), "http://www.w3.org/2004/02/skos/core#Concept", "http://www.w3.org/2004/02/skos/core#", "Concept");
	test_uri(rdf::foaf("Person"), "http://xmlns.com/foaf/0.1/Person", "http://xmlns.com/foaf/0.1/", "Person");
	test_uri(rdf::tts("Engine"), "http://rhdunn.github.com/2010/12/text-to-speech#Engine", "http://rhdunn.github.com/2010/12/text-to-speech#", "Engine");
}

TEST_CASE("rdf::href")
{
	test_uri(rdf::href("http://www.example.com/"), "http://www.example.com/", "http://www.example.com/", "");
	test_uri(rdf::href("http://www.example.com/value"), "http://www.example.com/value", "http://www.example.com/", "value");

	test_uri(rdf::href("http://www.example.com/abc/"), "http://www.example.com/abc/", "http://www.example.com/abc/", "");
	test_uri(rdf::href("http://www.example.com/abc/value"), "http://www.example.com/abc/value", "http://www.example.com/abc/", "value");

	test_uri(rdf::href("http://www.example.com/def#"), "http://www.example.com/def#", "http://www.example.com/def#", "");
	test_uri(rdf::href("http://www.example.com/def#value"), "http://www.example.com/def#value", "http://www.example.com/def#", "value");

	test_uri(rdf::href("http://www.example.com/def/#"), "http://www.example.com/def/#", "http://www.example.com/def/#", "");
	test_uri(rdf::href("http://www.example.com/def/#value"), "http://www.example.com/def/#value", "http://www.example.com/def/#", "value");
}

void test_bnode(const std::tr1::shared_ptr<const rdf::resource> &node, const std::string &id)
{
	test_bnode(*node, id);
}

void test_uri(const std::tr1::shared_ptr<const rdf::resource> &node, const std::string &value, const std::string &ns, const std::string &ref)
{
	test_uri(*node, value, ns, ref);
}

TEST_CASE("rdf::namespaces -- empty")
{
	rdf::namespaces test;

	test_uri(test(""), "", "", "");

	assert(!test("dc:title").get());
}

TEST_CASE("rdf::namespaces -- base uri")
{
	rdf::namespaces test;
	test.set_base("http://www.example.org/base");

	test_uri(test("test"), "http://www.example.org/basetest", "http://www.example.org/", "basetest");
	test_uri(test("/test"), "http://www.example.org/base/test", "http://www.example.org/base/", "test");
	test_uri(test("#test"), "http://www.example.org/base#test", "http://www.example.org/base#", "test");
}

TEST_CASE("rdf::namespaces -- uri schemes")
{
	rdf::namespaces test;
	test.set_base("http://www.example.org/base");

	test_uri(test("http://www.example.org/test/"), "http://www.example.org/test/", "http://www.example.org/test/", "");
	test_uri(test("http://www.example.org/test/value"), "http://www.example.org/test/value", "http://www.example.org/test/", "value");
	test_uri(test("http://www.example.org/test#"), "http://www.example.org/test#", "http://www.example.org/test#", "");
	test_uri(test("http://www.example.org/test#value"), "http://www.example.org/test#value", "http://www.example.org/test#", "value");
	test_uri(test("http://www.example.org/test/#"), "http://www.example.org/test/#", "http://www.example.org/test/#", "");
	test_uri(test("http://www.example.org/test/#value"), "http://www.example.org/test/#value", "http://www.example.org/test/#", "value");

	test_uri(test("https://www.example.org/test/"), "https://www.example.org/test/", "https://www.example.org/test/", "");
	test_uri(test("https://www.example.org/test/value"), "https://www.example.org/test/value", "https://www.example.org/test/", "value");
	test_uri(test("https://www.example.org/test#"), "https://www.example.org/test#", "https://www.example.org/test#", "");
	test_uri(test("https://www.example.org/test#value"), "https://www.example.org/test#value", "https://www.example.org/test#", "value");
	test_uri(test("https://www.example.org/test/#"), "https://www.example.org/test/#", "https://www.example.org/test/#", "");
	test_uri(test("https://www.example.org/test/#value"), "https://www.example.org/test/#value", "https://www.example.org/test/#", "value");

	test_uri(test("mailto:abc@example.com"), "mailto:abc@example.com", "mailto:abc@example.com", "");

	test_uri(test("file:///home/test/"), "file:///home/test/", "file:///home/test/", "");
	test_uri(test("file:///home/test/value"), "file:///home/test/value", "file:///home/test/", "value");
	test_uri(test("file:///home/test#"), "file:///home/test#", "file:///home/test#", "");
	test_uri(test("file:///home/test#value"), "file:///home/test#value", "file:///home/test#", "value");
	test_uri(test("file:///home/test/#"), "file:///home/test/#", "file:///home/test/#", "");
	test_uri(test("file:///home/test/#value"), "file:///home/test/#value", "file:///home/test/#", "value");
}

TEST_CASE("rdf::namespaces -- BNode CURIEs")
{
	rdf::namespaces test;

	test.set_base("http://www.example.org/base");
	test_bnode(test("_:test"), "test");
	test_bnode(test("_:joe"), "joe");
	test_bnode(test("_:sue"), "sue");

	test.set_base("http://www.example.org/base/");
	test_bnode(test("_:test"), "test");
	test_bnode(test("_:joe"), "joe");
	test_bnode(test("_:sue"), "sue");

	test.set_base("http://www.example.org/base#");
	test_bnode(test("_:test"), "test");
	test_bnode(test("_:joe"), "joe");
	test_bnode(test("_:sue"), "sue");

	test.set_base("http://www.example.org/base/#");
	test_bnode(test("_:test"), "test");
	test_bnode(test("_:joe"), "joe");
	test_bnode(test("_:sue"), "sue");
}

TEST_CASE("rdf::namespaces -- add_namespaces(prefix, href)")
{
	rdf::namespaces test;
	test.set_base("http://www.example.org/base/");

	assert(!test("dc:title").get());
	assert(!test("xsd:string").get());
	assert(!test("xml:lang").get());
	assert(!test("dct:title").get());

	test.add_namespace("dc", "http://purl.org/dc/elements/1.1/");
	test.add_namespace("xsd", "http://www.w3.org/2001/XMLSchema");
	test.add_namespace("xml", "http://www.w3.org/XML/1998/namespace#");

	test_uri(test("dc:title"), "http://purl.org/dc/elements/1.1/title", "http://purl.org/dc/elements/1.1/", "title");
	test_uri(test("xsd:string"), "http://www.w3.org/2001/XMLSchema#string", "http://www.w3.org/2001/XMLSchema#", "string");
	test_uri(test("xml:lang"), "http://www.w3.org/XML/1998/namespace#lang", "http://www.w3.org/XML/1998/namespace#", "lang");
	assert(!test("dct:title").get());
}

TEST_CASE("rdf::namespaces -- add_namespaces(ns)")
{
	rdf::namespaces test;
	test.set_base("http://www.example.org/base/");

	assert(!test("dc:title").get());
	assert(!test("xsd:string").get());
	assert(!test("xml:lang").get());
	assert(!test("dct:title").get());

	test.add_namespace(rdf::dc);
	test.add_namespace(rdf::xsd);
	test.add_namespace(rdf::xml);

	test_uri(test("dc:title"), "http://purl.org/dc/elements/1.1/title", "http://purl.org/dc/elements/1.1/", "title");
	test_uri(test("xsd:string"), "http://www.w3.org/2001/XMLSchema#string", "http://www.w3.org/2001/XMLSchema#", "string");
	test_uri(test("xml:lang"), "http://www.w3.org/XML/1998/namespace#lang", "http://www.w3.org/XML/1998/namespace#", "lang");
	assert(!test("dct:title").get());
}

TEST_CASE("rdf::namespaces -- namespaces << ns")
{
	rdf::namespaces test;
	test.set_base("http://www.example.org/base/");

	assert(!test("dc:title").get());
	assert(!test("xsd:string").get());
	assert(!test("xml:lang").get());
	assert(!test("dct:title").get());

	test << rdf::dc << rdf::xsd << rdf::xml;

	test_uri(test("dc:title"), "http://purl.org/dc/elements/1.1/title", "http://purl.org/dc/elements/1.1/", "title");
	test_uri(test("xsd:string"), "http://www.w3.org/2001/XMLSchema#string", "http://www.w3.org/2001/XMLSchema#", "string");
	test_uri(test("xml:lang"), "http://www.w3.org/XML/1998/namespace#lang", "http://www.w3.org/XML/1998/namespace#", "lang");
	assert(!test("dct:title").get());
}

TEST_CASE("rdf::namespaces -- add_prefix")
{
	rdf::namespaces test;
	test.set_base("http://www.example.org/base/");

	assert(!test("dc:title").get());
	assert(!test("xsd:string").get());
	assert(!test("xml:lang").get());
	assert(!test("dct:title").get());

	test.add_prefix("dc: http://purl.org/dc/elements/1.1/");

	test_uri(test("dc:title"), "http://purl.org/dc/elements/1.1/title", "http://purl.org/dc/elements/1.1/", "title");
	assert(!test("xsd:string").get());
	assert(!test("xml:lang").get());
	assert(!test("dct:title").get());

	test.add_prefix("xsd: http://www.w3.org/2001/XMLSchema# xml: http://www.w3.org/XML/1998/namespace#");

	test_uri(test("dc:title"), "http://purl.org/dc/elements/1.1/title", "http://purl.org/dc/elements/1.1/", "title");
	test_uri(test("xsd:string"), "http://www.w3.org/2001/XMLSchema#string", "http://www.w3.org/2001/XMLSchema#", "string");
	test_uri(test("xml:lang"), "http://www.w3.org/XML/1998/namespace#lang", "http://www.w3.org/XML/1998/namespace#", "lang");
	assert(!test("dct:title").get());
}

void test_literal(const rdf::node &node, const std::string value, const std::string &language, const rdf::uri &uri)
{
	const rdf::literal *literal = dynamic_cast<const rdf::literal *>(&node);
	if (assert(literal))
	{
		equal(literal->value,      value);
		equal(literal->language,   language);
		equal(literal->type.ns,    uri.ns);
		equal(literal->type.ref,   uri.ref);
	}
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

TEST_CASE("rdf::literal")
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
void test_statement(const std::tr1::shared_ptr<const rdf::triple> &s, const Subject &subject, const rdf::uri &predicate, const Object &object)
{
	test_item(*s->subject, subject);
	test_item(s->predicate, predicate);
	test_item(*s->object, object);
}

TEST_CASE("rdf::statement")
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

TEST_CASE("rdf::graph")
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

void test_model_namespace(const std::tr1::shared_ptr<const rdf::triple> &s, const rdf::ns &ns)
{
	rdf::graph model;
	assert(!model.contains(ns));
	assert(!model.contains(rdf::tts));

	model.push_back(s);
	assert(model.contains(ns));
	assert(!model.contains(rdf::tts));
}

TEST_CASE("rdf::graph -- RDF namespaces")
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

TEST_CASE("rdf::graph -- genid()")
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
