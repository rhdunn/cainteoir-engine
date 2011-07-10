/* RDF metadata.
 *
 * Copyright (C) 2011 Reece H. Dunn
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

const rdf::ns rdf::smil( "smil", "http://www.w3.org/ns/SMIL#");
const rdf::ns rdf::xhtml("h",    "http://www.w3.org/1999/xhtml#");

const rdf::ns rdf::skos("skos", "http://www.w3.org/2004/02/skos/core#");
const rdf::ns rdf::foaf("foaf", "http://xmlns.com/foaf/0.1/");
const rdf::ns rdf::tts( "tts",  "http://rhdunn.github.com/2010/12/text-to-speech#");

const rdf::resource *rdf::bnode::clone() const
{
	return new bnode(*this);
}

rdf::uri::uri(const std::string &aNS, const std::string &aRef)
	: ns(aNS)
	, ref(aRef)
{
	auto last = --ns.end();
	if (!ref.empty() && *last != '#' && *last != '/')
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

const rdf::resource *rdf::uri::clone() const
{
	return new uri(*this);
}

const rdf::uri rdf::href(const std::string &aHref)
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

rdf::namespaces::namespaces()
{
	mNamespaces["http"] = "http:";
}

rdf::namespaces &rdf::namespaces::set_base(const std::string &aBase)
{
	mBaseUri = aBase;
	return *this;
}

rdf::namespaces &rdf::namespaces::add_namespace(const std::string &aPrefix, const std::string &aHref)
{
	if (*(--aHref.end()) != '#' && *(--aHref.end()) != '/')
		mNamespaces[aPrefix] = aHref + '#';
	else
		mNamespaces[aPrefix] = aHref;
	return *this;
}

std::tr1::shared_ptr<const rdf::resource>
rdf::namespaces::operator()(const std::string &aCurie) const
{
	std::string uri;

	std::string::size_type index = aCurie.find(':');
	if (index != std::string::npos)
	{
		std::string prefix = aCurie.substr(0, index);
		std::string ref = aCurie.substr(index+1);

		if (prefix == "_")
			return std::tr1::shared_ptr<const rdf::resource>(new rdf::bnode(ref));

		auto ns = mNamespaces.find(prefix);
		if (ns == mNamespaces.end())
			return std::tr1::shared_ptr<const rdf::resource>();

		uri = ns->second + ref;
	}
	else
		uri = mBaseUri + aCurie;
	return std::tr1::shared_ptr<const rdf::resource>(new rdf::uri(href(uri)));
}

const rdf::node *rdf::literal::clone() const
{
	return new literal(*this);
}

const rdf::bnode rdf::graph::genid()
{
	std::ostringstream id;
	id << "genid" << nextid;
	++nextid;
	return rdf::bnode(id.str());
}

bool rdf::graph::contains(const ns &uri) const
{
	return namespaces.find(uri.href) != namespaces.end();
}

void rdf::graph::push_back(const std::tr1::shared_ptr<const triple> &s)
{
	{
		const rdf::uri *uri = rdf::query::subject(s);
		if (uri)
			namespaces.insert(uri->ns);
	}
	namespaces.insert(s->predicate.ns);
	{
		const rdf::uri *uri = rdf::query::object(s);
		if (uri)
			namespaces.insert(uri->ns);

		const rdf::literal *literal = rdf::query::object(s);
		if (literal && !literal->type.ns.empty())
			namespaces.insert(literal->type.ns);
	}
	rdf::subgraph::push_back(s);
}
