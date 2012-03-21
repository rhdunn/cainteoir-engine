/* RDF metadata.
 *
 * Copyright (C) 2011-2012 Reece H. Dunn
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

namespace rdf = cainteoir::rdf;

const rdf::ns rdf::bnode("_", "_:");

const rdf::ns rdf::rdf( "rdf",  "http://www.w3.org/1999/02/22-rdf-syntax-ns#");
const rdf::ns rdf::rdfa("rdfa", "http://www.w3.org/ns/rdfa#");
const rdf::ns rdf::rdfs("rdfs", "http://www.w3.org/2000/01/rdf-schema#");
const rdf::ns rdf::xsd( "xsd",  "http://www.w3.org/2001/XMLSchema#");
const rdf::ns rdf::xml( "xml",  "http://www.w3.org/XML/1998/namespace#");
const rdf::ns rdf::owl( "owl",  "http://www.w3.org/2002/07/owl#");

const rdf::ns rdf::dc(     "dc",   "http://purl.org/dc/elements/1.1/");
const rdf::ns rdf::dcterms("dct",  "http://purl.org/dc/terms/");
const rdf::ns rdf::dcam(   "dcam", "http://purl.org/dc/dcam/");

const rdf::ns rdf::dtb("dtb", "http://www.daisy.org/z3986/2005/dtbook/");
const rdf::ns rdf::ncx("ncx", "http://www.daisy.org/z3986/2005/ncx/");

const rdf::ns rdf::epub( "epub",  "http://www.idpf.org/2007/ops#");
const rdf::ns rdf::opf(  "opf",   "http://www.idpf.org/2007/opf#");
const rdf::ns rdf::ocf(  "ocf",   "urn:oasis:names:tc:opendocument:xmlns:container#");
const rdf::ns rdf::pkg(  "pkg",   "http://www.idpf.org/epub/vocab/package/#");
const rdf::ns rdf::media("media", "http://www.idpf.org/epub/vocab/overlays/#");

const rdf::ns rdf::ssml( "ssml", "http://www.w3.org/2001/10/synthesis#");
const rdf::ns rdf::smil( "smil", "http://www.w3.org/ns/SMIL#");
const rdf::ns rdf::xhtml("h",    "http://www.w3.org/1999/xhtml#");

const rdf::ns rdf::skos("skos", "http://www.w3.org/2004/02/skos/core#");
const rdf::ns rdf::foaf("foaf", "http://xmlns.com/foaf/0.1/");
const rdf::ns rdf::tts( "tts",  "http://rhdunn.github.com/2010/12/text-to-speech#");

bool rdf::resource::operator==(const resource &rhs) const
{
	{
		const rdf::uri *a = dynamic_cast<const rdf::uri *>(value);
		const rdf::uri *b = dynamic_cast<const rdf::uri *>(rhs.value);
		if (a && b)
			return a == b || *a == *b;
	}

	{
		const rdf::literal *a = dynamic_cast<const rdf::literal *>(value);
		const rdf::literal *b = dynamic_cast<const rdf::literal *>(rhs.value);
		if (a && b)
			return a == b || *a == *b;
	}

	return value == rhs.value;
}

rdf::uri::uri(const std::string &aNS, const std::string &aRef)
	: ns(aNS)
	, ref(aRef)
{
	auto last = --ns.end();
	if (!ns.empty() && !ref.empty() && *last != '#' && *last != '/' && *last != ':')
		ns.push_back('#');
}

bool rdf::uri::empty() const
{
	return ns.empty() && ref.empty();
}

std::string rdf::uri::str() const
{
	if (ref.empty())
		return ns;

	return ns + ref;
}

std::shared_ptr<const cainteoir::xml::resource> rdf::uri::clone() const
{
	return std::make_shared<uri>(*this);
}

const rdf::uri rdf::graph::href(const std::string &aHref)
{
	std::string::size_type index;

	index = aHref.rfind('#');
	if (index == aHref.size()-1)
		return uri(aHref, std::string());
	else if (index != std::string::npos)
		return uri(aHref.substr(0, index+1), aHref.substr(index+1));

	index = aHref.rfind('/');
	if (index != std::string::npos && index != aHref.size()-1)
		return uri(aHref.substr(0, index+1), aHref.substr(index+1));

	return uri(aHref, std::string());
}

rdf::graph &rdf::graph::set_base(const std::string &aBase)
{
	mBaseUri = aBase;
	return *this;
}

rdf::graph &rdf::graph::add_namespace(const std::string &aPrefix, const std::string &aHref)
{
	char end = *(--aHref.end());
	if (end != '#' && end != '/' && end != ':')
		namespaces::add_namespace(aPrefix, aHref + '#');
	else
		namespaces::add_namespace(aPrefix, aHref);
	return *this;
}

rdf::graph &rdf::graph::add_prefix(const std::string &aPrefix)
{
	std::string prefix;
	std::string href;

	std::istringstream ss(aPrefix);
	while (ss >> prefix >> href)
		add_namespace(prefix.substr(0, prefix.find(':')), href);

	return *this;
}

std::shared_ptr<const rdf::uri>
rdf::graph::curie(const std::string &aCurie)
{
	std::string uri;

	std::string::size_type index = aCurie.find(':');
	if (index != std::string::npos)
	{
		std::string prefix = aCurie.substr(0, index);
		std::string ref = aCurie.substr(index+1);

		if (prefix == "_")
			return std::make_shared<rdf::uri>("_:", ref);

		std::string ns = lookup(prefix);
		if (ns.empty())
			return std::shared_ptr<const rdf::uri>();

		uri = ns + ref;
	}
	else
		uri = mBaseUri + aCurie;
	return std::make_shared<rdf::uri>(href(uri));
}

std::shared_ptr<const cainteoir::xml::resource> rdf::literal::clone() const
{
	return std::make_shared<literal>(*this);
}

rdf::graph::graph() : nextid(1)
{
	add_namespace("http",   "http:");
	add_namespace("https",  "https:");
	add_namespace("mailto", "mailto:");
	add_namespace("file",   "file:");
}

const rdf::uri rdf::graph::genid()
{
	std::ostringstream id;
	id << "genid" << nextid;
	++nextid;
	return bnode(id.str());
}

bool rdf::graph::contains(const ns &uri) const
{
	return namespaces.find(uri.href) != namespaces.end();
}

bool rdf::graph::statement(const rdf::uri &aSubject, const rdf::uri &aPredicate, const rdf::uri &aObject)
{
	if (aPredicate.ns == rdf::bnode.href)
		return false;

	if (!aSubject.ns.empty())
		namespaces.insert(aSubject.ns);

	namespaces.insert(aPredicate.ns);

	if (!aObject.ns.empty())
		namespaces.insert(aObject.ns);

	push_back(std::make_shared<triple>(aSubject.clone(), aPredicate, aObject.clone()));
	return true;
}

bool rdf::graph::statement(const rdf::uri &aSubject, const rdf::uri &aPredicate, const rdf::literal &aObject)
{
	if (aPredicate.ns == rdf::bnode.href)
		return false;

	if (!aSubject.ns.empty())
		namespaces.insert(aSubject.ns);

	namespaces.insert(aPredicate.ns);

	if (!aObject.type.ns.empty())
		namespaces.insert(aObject.type.ns);

	push_back(std::make_shared<triple>(aSubject.clone(), aPredicate, aObject.clone()));
	return true;
}
