/* RDF statement tests.
 *
 * Copyright (C) 2010-2014 Reece H. Dunn
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
namespace xml = cainteoir::xml;

REGISTER_TESTSUITE("RDF Model");

void test_uri(const rdf::resource &node, const std::string &value, const std::string &ns, const std::string &ref)
{
	const rdf::uri *uri = dynamic_cast<const rdf::uri *>(&node);
	if (assert(uri))
	{
		assert(uri->str() == value);
		assert(uri->ns    == ns);
		assert(uri->ref   == ref);
	}
}

TEST_CASE("rdf::bnode")
{
	test_uri(rdf::bnode("a"),    "_:a",    "_:", "a");
	test_uri(rdf::bnode("temp"), "_:temp", "_:", "temp");
}

TEST_CASE("rdf::uri")
{
	test_uri(rdf::uri(std::string(), std::string()), "", "", "");
	test_uri(rdf::uri(std::string(), "test"), "test", "", "test");

	test_uri(rdf::uri("http://www.w3.org/2001/XMLSchema", std::string()), "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/2001/XMLSchema", "");
	test_uri(rdf::uri("http://www.w3.org/2001/XMLSchema/", std::string()), "http://www.w3.org/2001/XMLSchema/", "http://www.w3.org/2001/XMLSchema/", "");
	test_uri(rdf::uri("http://www.w3.org/2001/XMLSchema#", std::string()), "http://www.w3.org/2001/XMLSchema#", "http://www.w3.org/2001/XMLSchema#", "");

	test_uri(rdf::uri("http://www.w3.org/2001/XMLSchema", "string"), "http://www.w3.org/2001/XMLSchema#string", "http://www.w3.org/2001/XMLSchema#", "string");
	test_uri(rdf::uri("http://www.w3.org/2001/XMLSchema/", "string"), "http://www.w3.org/2001/XMLSchema/string", "http://www.w3.org/2001/XMLSchema/", "string");
	test_uri(rdf::uri("http://www.w3.org/2001/XMLSchema#", "string"), "http://www.w3.org/2001/XMLSchema#string", "http://www.w3.org/2001/XMLSchema#", "string");
	test_uri(rdf::uri("http://www.w3.org/2001/XMLSchema/#", "string"), "http://www.w3.org/2001/XMLSchema/#string", "http://www.w3.org/2001/XMLSchema/#", "string");

	test_uri(rdf::uri("http:", "//www.example.org"), "http://www.example.org", "http:", "//www.example.org");
	test_uri(rdf::uri("mailto:", "test@example.com"), "mailto:test@example.com", "mailto:", "test@example.com");
}

TEST_CASE("RDF namespaces")
{
	test_uri(rdf::dc("title"), "http://purl.org/dc/elements/1.1/title", "http://purl.org/dc/elements/1.1/", "title");
	test_uri(rdf::dcam("memberOf"), "http://purl.org/dc/dcam/memberOf", "http://purl.org/dc/dcam/", "memberOf");
	test_uri(rdf::dcterms("title"), "http://purl.org/dc/terms/title", "http://purl.org/dc/terms/", "title");
	test_uri(rdf::dtb("uid"), "http://www.daisy.org/z3986/2005/dtbook/uid", "http://www.daisy.org/z3986/2005/dtbook/", "uid");
	test_uri(rdf::epub("switch"), "http://www.idpf.org/2007/ops#switch", "http://www.idpf.org/2007/ops#", "switch");
	test_uri(rdf::epv("toc"), "http://www.idpf.org/epub/vocab/structure/#toc", "http://www.idpf.org/epub/vocab/structure/#", "toc");
	test_uri(rdf::foaf("Person"), "http://xmlns.com/foaf/0.1/Person", "http://xmlns.com/foaf/0.1/", "Person");
	test_uri(rdf::iana("Language"), "http://reecedunn.co.uk/schema/2013/iana#Language", "http://reecedunn.co.uk/schema/2013/iana#", "Language");
	test_uri(rdf::media("duration"), "http://www.idpf.org/epub/vocab/overlays/#duration", "http://www.idpf.org/epub/vocab/overlays/#", "duration");
	test_uri(rdf::ncx("meta"), "http://www.daisy.org/z3986/2005/ncx/meta", "http://www.daisy.org/z3986/2005/ncx/", "meta");
	test_uri(rdf::ocf("container"), "urn:oasis:names:tc:opendocument:xmlns:container#container", "urn:oasis:names:tc:opendocument:xmlns:container#", "container");
	test_uri(rdf::opf("package"), "http://www.idpf.org/2007/opf#package", "http://www.idpf.org/2007/opf#", "package");
	test_uri(rdf::owl("Ontology"), "http://www.w3.org/2002/07/owl#Ontology", "http://www.w3.org/2002/07/owl#", "Ontology");
	test_uri(rdf::pkg("role"), "http://www.idpf.org/epub/vocab/package/#role", "http://www.idpf.org/epub/vocab/package/#", "role");
	test_uri(rdf::rdf("type"), "http://www.w3.org/1999/02/22-rdf-syntax-ns#type", "http://www.w3.org/1999/02/22-rdf-syntax-ns#", "type");
	test_uri(rdf::rdfa("Error"), "http://www.w3.org/ns/rdfa#Error", "http://www.w3.org/ns/rdfa#", "Error");
	test_uri(rdf::rdfs("Class"), "http://www.w3.org/2000/01/rdf-schema#Class", "http://www.w3.org/2000/01/rdf-schema#", "Class");
	test_uri(rdf::ref("level"), "http://reecedunn.co.uk/schema/2014/ref#level", "http://reecedunn.co.uk/schema/2014/ref#", "level");
	test_uri(rdf::skos("Concept"), "http://www.w3.org/2004/02/skos/core#Concept", "http://www.w3.org/2004/02/skos/core#", "Concept");
	test_uri(rdf::smil("smil"), "http://www.w3.org/ns/SMIL#smil", "http://www.w3.org/ns/SMIL#", "smil");
	test_uri(rdf::ssml("speak"), "http://www.w3.org/2001/10/synthesis#speak", "http://www.w3.org/2001/10/synthesis#", "speak");
	test_uri(rdf::subtag("de"), "http://rhdunn.github.com/cainteoir/data/iana/subtags#de", "http://rhdunn.github.com/cainteoir/data/iana/subtags#", "de");
	test_uri(rdf::tts("Engine"), "http://reecedunn.co.uk/schema/2014/tts#Engine", "http://reecedunn.co.uk/schema/2014/tts#", "Engine");
	test_uri(rdf::xhtml("head"), "http://www.w3.org/1999/xhtml#head", "http://www.w3.org/1999/xhtml#", "head");
	test_uri(rdf::xml("lang"), "http://www.w3.org/XML/1998/namespace#lang", "http://www.w3.org/XML/1998/namespace#", "lang");
	test_uri(rdf::xsd("string"), "http://www.w3.org/2001/XMLSchema#string", "http://www.w3.org/2001/XMLSchema#", "string");
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

void test_uri(const std::shared_ptr<const rdf::resource> &node, const std::string &value, const std::string &ns, const std::string &ref)
{
	test_uri(*node, value, ns, ref);
}

TEST_CASE("rdf::graph -- curie: empty namespaces")
{
	rdf::graph test;

	test_uri(test.curie(""), "", "", "");

	assert(!test.curie("dc:title").get());
}

TEST_CASE("rdf::graph -- curie: base uri")
{
	rdf::graph test;
	test.set_base("http://www.example.org/base");
	assert(test.get_base() == "http://www.example.org/base");

	test_uri(test.curie("test"), "http://www.example.org/basetest", "http://www.example.org/", "basetest");
	test_uri(test.curie("/test"), "http://www.example.org/base/test", "http://www.example.org/base/", "test");
	test_uri(test.curie("#test"), "http://www.example.org/base#test", "http://www.example.org/base#", "test");
}

TEST_CASE("rdf::graph -- curie: base uri -- tied")
{
	rdf::graph g;
	g.set_base("http://www.example.org/base");

	rdf::graph test(g);
	assert(test.get_base() == "http://www.example.org/base");

	test_uri(test.curie("test"), "http://www.example.org/basetest", "http://www.example.org/", "basetest");
	test_uri(test.curie("/test"), "http://www.example.org/base/test", "http://www.example.org/base/", "test");
	test_uri(test.curie("#test"), "http://www.example.org/base#test", "http://www.example.org/base#", "test");
}

TEST_CASE("rdf::graph -- curie: uri schemes")
{
	rdf::graph test;
	test.set_base("http://www.example.org/base");

	test_uri(test.curie("http://www.example.org/test/"), "http://www.example.org/test/", "http://www.example.org/test/", "");
	test_uri(test.curie("http://www.example.org/test/value"), "http://www.example.org/test/value", "http://www.example.org/test/", "value");
	test_uri(test.curie("http://www.example.org/test#"), "http://www.example.org/test#", "http://www.example.org/test#", "");
	test_uri(test.curie("http://www.example.org/test#value"), "http://www.example.org/test#value", "http://www.example.org/test#", "value");
	test_uri(test.curie("http://www.example.org/test/#"), "http://www.example.org/test/#", "http://www.example.org/test/#", "");
	test_uri(test.curie("http://www.example.org/test/#value"), "http://www.example.org/test/#value", "http://www.example.org/test/#", "value");

	test_uri(test.curie("https://www.example.org/test/"), "https://www.example.org/test/", "https://www.example.org/test/", "");
	test_uri(test.curie("https://www.example.org/test/value"), "https://www.example.org/test/value", "https://www.example.org/test/", "value");
	test_uri(test.curie("https://www.example.org/test#"), "https://www.example.org/test#", "https://www.example.org/test#", "");
	test_uri(test.curie("https://www.example.org/test#value"), "https://www.example.org/test#value", "https://www.example.org/test#", "value");
	test_uri(test.curie("https://www.example.org/test/#"), "https://www.example.org/test/#", "https://www.example.org/test/#", "");
	test_uri(test.curie("https://www.example.org/test/#value"), "https://www.example.org/test/#value", "https://www.example.org/test/#", "value");

	test_uri(test.curie("mailto:abc@example.com"), "mailto:abc@example.com", "mailto:abc@example.com", "");

	test_uri(test.curie("file:///home/test/"), "file:///home/test/", "file:///home/test/", "");
	test_uri(test.curie("file:///home/test/value"), "file:///home/test/value", "file:///home/test/", "value");
	test_uri(test.curie("file:///home/test#"), "file:///home/test#", "file:///home/test#", "");
	test_uri(test.curie("file:///home/test#value"), "file:///home/test#value", "file:///home/test#", "value");
	test_uri(test.curie("file:///home/test/#"), "file:///home/test/#", "file:///home/test/#", "");
	test_uri(test.curie("file:///home/test/#value"), "file:///home/test/#value", "file:///home/test/#", "value");
}

TEST_CASE("rdf::graph -- curie: uri schemes -- tied")
{
	rdf::graph g;

	rdf::graph test(g);
	test.set_base("http://www.example.org/base");

	test_uri(test.curie("http://www.example.org/test/"), "http://www.example.org/test/", "http://www.example.org/test/", "");
	test_uri(test.curie("http://www.example.org/test/value"), "http://www.example.org/test/value", "http://www.example.org/test/", "value");
	test_uri(test.curie("http://www.example.org/test#"), "http://www.example.org/test#", "http://www.example.org/test#", "");
	test_uri(test.curie("http://www.example.org/test#value"), "http://www.example.org/test#value", "http://www.example.org/test#", "value");
	test_uri(test.curie("http://www.example.org/test/#"), "http://www.example.org/test/#", "http://www.example.org/test/#", "");
	test_uri(test.curie("http://www.example.org/test/#value"), "http://www.example.org/test/#value", "http://www.example.org/test/#", "value");

	test_uri(test.curie("https://www.example.org/test/"), "https://www.example.org/test/", "https://www.example.org/test/", "");
	test_uri(test.curie("https://www.example.org/test/value"), "https://www.example.org/test/value", "https://www.example.org/test/", "value");
	test_uri(test.curie("https://www.example.org/test#"), "https://www.example.org/test#", "https://www.example.org/test#", "");
	test_uri(test.curie("https://www.example.org/test#value"), "https://www.example.org/test#value", "https://www.example.org/test#", "value");
	test_uri(test.curie("https://www.example.org/test/#"), "https://www.example.org/test/#", "https://www.example.org/test/#", "");
	test_uri(test.curie("https://www.example.org/test/#value"), "https://www.example.org/test/#value", "https://www.example.org/test/#", "value");

	test_uri(test.curie("mailto:abc@example.com"), "mailto:abc@example.com", "mailto:abc@example.com", "");

	test_uri(test.curie("file:///home/test/"), "file:///home/test/", "file:///home/test/", "");
	test_uri(test.curie("file:///home/test/value"), "file:///home/test/value", "file:///home/test/", "value");
	test_uri(test.curie("file:///home/test#"), "file:///home/test#", "file:///home/test#", "");
	test_uri(test.curie("file:///home/test#value"), "file:///home/test#value", "file:///home/test#", "value");
	test_uri(test.curie("file:///home/test/#"), "file:///home/test/#", "file:///home/test/#", "");
	test_uri(test.curie("file:///home/test/#value"), "file:///home/test/#value", "file:///home/test/#", "value");
}

TEST_CASE("rdf::graph -- curie: bnodes")
{
	rdf::graph test;

	test.set_base("http://www.example.org/base");
	test_uri(test.curie("_:test"), "_:test", "_:", "test");
	test_uri(test.curie("_:joe"),  "_:joe",  "_:", "joe");
	test_uri(test.curie("_:sue"),  "_:sue",  "_:", "sue");

	test.set_base("http://www.example.org/base/");
	test_uri(test.curie("_:test"), "_:test", "_:", "test");
	test_uri(test.curie("_:joe"),  "_:joe",  "_:", "joe");
	test_uri(test.curie("_:sue"),  "_:sue",  "_:", "sue");

	test.set_base("http://www.example.org/base#");
	test_uri(test.curie("_:test"), "_:test", "_:", "test");
	test_uri(test.curie("_:joe"),  "_:joe",  "_:", "joe");
	test_uri(test.curie("_:sue"),  "_:sue",  "_:", "sue");

	test.set_base("http://www.example.org/base/#");
	test_uri(test.curie("_:test"), "_:test", "_:", "test");
	test_uri(test.curie("_:joe"),  "_:joe",  "_:", "joe");
	test_uri(test.curie("_:sue"),  "_:sue",  "_:", "sue");
}

TEST_CASE("rdf::graph -- curie: add_namespaces(prefix, href)")
{
	rdf::graph test;
	test.set_base("http://www.example.org/base/");

	assert(!test.curie("dc:title").get());
	assert(!test.curie("xsd:string").get());
	assert(test.curie("xml:lang").get());
	assert(!test.curie("dct:title").get());

	test.add_namespace("dc", "http://purl.org/dc/elements/1.1/");
	test.add_namespace("xsd", "http://www.w3.org/2001/XMLSchema");
	test.add_namespace("xml", "http://www.w3.org/XML/1998/namespace#");

	test_uri(test.curie("dc:title"), "http://purl.org/dc/elements/1.1/title", "http://purl.org/dc/elements/1.1/", "title");
	test_uri(test.curie("xsd:string"), "http://www.w3.org/2001/XMLSchema#string", "http://www.w3.org/2001/XMLSchema#", "string");
	test_uri(test.curie("xml:lang"), "http://www.w3.org/XML/1998/namespace#lang", "http://www.w3.org/XML/1998/namespace#", "lang");
	assert(!test.curie("dct:title").get());
}

TEST_CASE("rdf::graph -- curie: add_namespaces(ns)")
{
	rdf::graph test;
	test.set_base("http://www.example.org/base/");

	assert(!test.curie("dc:title").get());
	assert(!test.curie("xsd:string").get());
	assert(test.curie("xml:lang").get());
	assert(!test.curie("dct:title").get());

	test.add_namespace(rdf::dc);
	test.add_namespace(rdf::xsd);
	test.add_namespace(rdf::xml);

	test_uri(test.curie("dc:title"), "http://purl.org/dc/elements/1.1/title", "http://purl.org/dc/elements/1.1/", "title");
	test_uri(test.curie("xsd:string"), "http://www.w3.org/2001/XMLSchema#string", "http://www.w3.org/2001/XMLSchema#", "string");
	test_uri(test.curie("xml:lang"), "http://www.w3.org/XML/1998/namespace#lang", "http://www.w3.org/XML/1998/namespace#", "lang");
	assert(!test.curie("dct:title").get());
}

TEST_CASE("rdf::graph -- curie: namespaces << ns")
{
	rdf::graph test;
	test.set_base("http://www.example.org/base/");

	assert(!test.curie("dc:title").get());
	assert(!test.curie("xsd:string").get());
	assert(test.curie("xml:lang").get());
	assert(!test.curie("dct:title").get());

	test << rdf::dc << rdf::xsd << rdf::xml;

	test_uri(test.curie("dc:title"), "http://purl.org/dc/elements/1.1/title", "http://purl.org/dc/elements/1.1/", "title");
	test_uri(test.curie("xsd:string"), "http://www.w3.org/2001/XMLSchema#string", "http://www.w3.org/2001/XMLSchema#", "string");
	test_uri(test.curie("xml:lang"), "http://www.w3.org/XML/1998/namespace#lang", "http://www.w3.org/XML/1998/namespace#", "lang");
	assert(!test.curie("dct:title").get());
}

TEST_CASE("rdf::graph -- curie: add_prefix")
{
	rdf::graph test;
	test.set_base("http://www.example.org/base/");

	assert(!test.curie("dc:title").get());
	assert(!test.curie("xsd:string").get());
	assert(test.curie("xml:lang").get());
	assert(!test.curie("dct:title").get());

	test.add_prefix("dc: http://purl.org/dc/elements/1.1/");

	test_uri(test.curie("dc:title"), "http://purl.org/dc/elements/1.1/title", "http://purl.org/dc/elements/1.1/", "title");
	assert(!test.curie("xsd:string").get());
	assert(test.curie("xml:lang").get());
	assert(!test.curie("dct:title").get());

	test.add_prefix("xsd: http://www.w3.org/2001/XMLSchema# xml: http://www.w3.org/XML/1998/namespace#");

	test_uri(test.curie("dc:title"), "http://purl.org/dc/elements/1.1/title", "http://purl.org/dc/elements/1.1/", "title");
	test_uri(test.curie("xsd:string"), "http://www.w3.org/2001/XMLSchema#string", "http://www.w3.org/2001/XMLSchema#", "string");
	test_uri(test.curie("xml:lang"), "http://www.w3.org/XML/1998/namespace#lang", "http://www.w3.org/XML/1998/namespace#", "lang");
	assert(!test.curie("dct:title").get());
}

void test_literal(const rdf::resource &node, const std::string value, const std::string &language, const rdf::uri &uri)
{
	const rdf::literal *literal = dynamic_cast<const rdf::literal *>(&node);
	if (assert(literal))
	{
		assert(literal->value    == value);
		assert(literal->language == language);
		assert(literal->type.ns  == uri.ns);
		assert(literal->type.ref == uri.ref);
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

void test_item(const rdf::resource &a, const rdf::uri &b)
{
	test_uri(a, b.str(), b.ns, b.ref);
}

void test_item(const rdf::resource &a, const rdf::literal &b)
{
	test_literal(a, b.value, b.language, b.type);
}

template<typename Subject, typename Object>
void test_statement(const std::shared_ptr<const rdf::triple> &s, const Subject &subject, const rdf::uri &predicate, const Object &object)
{
	test_item(s->subject,   subject);
	test_item(s->predicate, predicate);
	test_item(*s->object,   object);
}

TEST_CASE("rdf::graph -- statement")
{
	rdf::graph model;
	assert(model.empty());
	assert(model.size()  == 0);
	assert(model.begin() == model.end());

	assert(model.statement(rdf::rdf("value"), rdf::rdf("type"), rdf::rdfs("Property")));
	assert(!model.empty());
	assert(model.size()  == 1);
	assert(model.begin() != model.end());

	test_statement(model.front(), rdf::rdf("value"), rdf::rdf("type"), rdf::rdfs("Property"));
	test_statement(model.back(), rdf::rdf("value"), rdf::rdf("type"), rdf::rdfs("Property"));

	assert(!model.statement(rdf::rdf("value"), rdf::bnode("test"), rdf::rdfs("Property")));
	assert(!model.empty());
	assert(model.size()  == 1);
	assert(model.begin() != model.end());

	assert(model.statement(rdf::rdf("value"), rdf::rdf("value"), rdf::literal("value", "en-GB")));
	assert(!model.empty());
	assert(model.size()  == 2);
	assert(model.begin() != model.end());

	test_statement(model.front(), rdf::rdf("value"), rdf::rdf("type"), rdf::rdfs("Property"));
	test_statement(model.back(), rdf::rdf("value"), rdf::rdf("value"), rdf::literal("value", "en-GB"));
}

TEST_CASE("rdf::graph -- RDF namespaces")
{
	{
		rdf::graph model;
		assert(!model.contains(rdf::dc));
		assert(!model.contains(rdf::rdf));
		assert(!model.contains(rdf::rdfs));
		assert(!model.contains(rdf::xsd));
		assert(!model.contains(rdf::tts));
	}

	{
		rdf::graph model;
		assert(model.statement(rdf::dc("title"), rdf::rdf("type"), rdf::rdfs("Property")));
		assert(model.contains(rdf::dc));
		assert(model.contains(rdf::rdf));
		assert(model.contains(rdf::rdfs));
		assert(!model.contains(rdf::xsd));
		assert(!model.contains(rdf::tts));
	}

	{
		rdf::graph model;
		assert(model.statement(rdf::rdfs("Property"), rdf::rdf("value"), rdf::literal("Property")));
		assert(!model.contains(rdf::dc));
		assert(model.contains(rdf::rdf));
		assert(model.contains(rdf::rdfs));
		assert(!model.contains(rdf::xsd));
		assert(!model.contains(rdf::tts));
	}

	{
		rdf::graph model;
		assert(model.statement(rdf::rdfs("Property"), rdf::rdf("value"), rdf::literal("Property", rdf::xsd("string"))));
		assert(!model.contains(rdf::dc));
		assert(model.contains(rdf::rdf));
		assert(model.contains(rdf::rdfs));
		assert(model.contains(rdf::xsd));
		assert(!model.contains(rdf::tts));
	}

	{
		rdf::graph model;
		assert(model.statement(rdf::rdfs("Property"), rdf::dc("creator"), rdf::bnode("a")));
		assert(model.contains(rdf::dc));
		assert(!model.contains(rdf::rdf));
		assert(model.contains(rdf::rdfs));
		assert(!model.contains(rdf::xsd));
		assert(!model.contains(rdf::tts));
	}

	{
		rdf::graph model;
		assert(model.statement(rdf::bnode("a"), rdf::rdf("value"), rdf::literal("test")));
		assert(!model.contains(rdf::dc));
		assert(model.contains(rdf::rdf));
		assert(!model.contains(rdf::rdfs));
		assert(!model.contains(rdf::xsd));
		assert(!model.contains(rdf::tts));
	}

	{
		rdf::graph model;
		assert(model.statement(rdf::bnode("a"), rdf::rdf("value"), rdf::literal("test", rdf::xsd("string"))));
		assert(!model.contains(rdf::dc));
		assert(model.contains(rdf::rdf));
		assert(!model.contains(rdf::rdfs));
		assert(model.contains(rdf::xsd));
		assert(!model.contains(rdf::tts));
	}

	{
		rdf::graph model;
		assert(model.statement(rdf::bnode("a"), rdf::rdf("type"), rdf::rdfs("Class")));
		assert(!model.contains(rdf::dc));
		assert(model.contains(rdf::rdf));
		assert(model.contains(rdf::rdfs));
		assert(!model.contains(rdf::xsd));
		assert(!model.contains(rdf::tts));
	}

	{
		rdf::graph model;
		assert(model.statement(rdf::bnode("a"), rdf::dc("creator"), rdf::bnode("John")));
		assert(model.contains(rdf::dc));
		assert(!model.contains(rdf::rdf));
		assert(!model.contains(rdf::rdfs));
		assert(!model.contains(rdf::xsd));
		assert(!model.contains(rdf::tts));
	}
}

TEST_CASE("rdf::graph -- genid()")
{
	rdf::graph model1;
	rdf::graph model2;

	test_uri(model1.genid(), "_:genid1", "_:", "genid1");
	test_uri(model1.genid(), "_:genid2", "_:", "genid2");
	test_uri(model1.genid(), "_:genid3", "_:", "genid3");
	test_uri(model2.genid(), "_:genid1", "_:", "genid1");
	test_uri(model2.genid(), "_:genid2", "_:", "genid2");
	test_uri(model1.genid(), "_:genid4", "_:", "genid4");
	test_uri(model1.genid(), "_:genid5", "_:", "genid5");
	test_uri(model2.genid(), "_:genid3", "_:", "genid3");
	test_uri(model2.genid(), "_:genid4", "_:", "genid4");
}

TEST_CASE("rdf::graph -- genid() -- tied")
{
	rdf::graph model1;
	rdf::graph model2(model1);

	test_uri(model1.genid(), "_:genid1", "_:", "genid1");
	test_uri(model1.genid(), "_:genid2", "_:", "genid2");
	test_uri(model1.genid(), "_:genid3", "_:", "genid3");
	test_uri(model2.genid(), "_:genid4", "_:", "genid4");
	test_uri(model2.genid(), "_:genid5", "_:", "genid5");
	test_uri(model1.genid(), "_:genid6", "_:", "genid6");
	test_uri(model1.genid(), "_:genid7", "_:", "genid7");
	test_uri(model2.genid(), "_:genid8", "_:", "genid8");
	test_uri(model2.genid(), "_:genid9", "_:", "genid9");
}
