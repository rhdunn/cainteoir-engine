/* RDF metadata.
 *
 * Copyright (C) 2011-2014 Reece H. Dunn
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
namespace rql = cainteoir::rdf::query;

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

std::shared_ptr<const rdf::resource> rdf::uri::clone() const
{
	return std::make_shared<uri>(*this);
}

const rdf::ns rdf::bnode(  "_",      "_:");
const rdf::ns rdf::dc(     "dc",     "http://purl.org/dc/elements/1.1/");
const rdf::ns rdf::dcam(   "dcam",   "http://purl.org/dc/dcam/");
const rdf::ns rdf::dcterms("dct",    "http://purl.org/dc/terms/");
const rdf::ns rdf::dtb(    "dtb",    "http://www.daisy.org/z3986/2005/dtbook/");
const rdf::ns rdf::epub(   "epub",   "http://www.idpf.org/2007/ops#");
const rdf::ns rdf::epv(    "epv",    "http://www.idpf.org/epub/vocab/structure/#");
const rdf::ns rdf::foaf(   "foaf",   "http://xmlns.com/foaf/0.1/");
const rdf::ns rdf::iana(   "iana",   "http://reecedunn.co.uk/schema/2013/iana#");
const rdf::ns rdf::marc(   "marc",   "http://id.loc.gov/vocabulary/");
const rdf::ns rdf::media(  "media",  "http://www.idpf.org/epub/vocab/overlays/#");
const rdf::ns rdf::ncx(    "ncx",    "http://www.daisy.org/z3986/2005/ncx/");
const rdf::ns rdf::ocf(    "ocf",    "urn:oasis:names:tc:opendocument:xmlns:container#");
const rdf::ns rdf::onix(   "onix",   "http://www.editeur.org/ONIX/book/codelists/current.html#");
const rdf::ns rdf::opf(    "opf",    "http://www.idpf.org/2007/opf#");
const rdf::ns rdf::owl(    "owl",    "http://www.w3.org/2002/07/owl#");
const rdf::ns rdf::pkg(    "pkg",    "http://www.idpf.org/epub/vocab/package/#");
const rdf::ns rdf::rdf(    "rdf",    "http://www.w3.org/1999/02/22-rdf-syntax-ns#");
const rdf::ns rdf::rdfa(   "rdfa",   "http://www.w3.org/ns/rdfa#");
const rdf::ns rdf::rdfs(   "rdfs",   "http://www.w3.org/2000/01/rdf-schema#");
const rdf::ns rdf::ref(    "ref",    "http://reecedunn.co.uk/schema/2014/ref#");
const rdf::ns rdf::skos(   "skos",   "http://www.w3.org/2004/02/skos/core#");
const rdf::ns rdf::smil(   "smil",   "http://www.w3.org/ns/SMIL#");
const rdf::ns rdf::ssml(   "ssml",   "http://www.w3.org/2001/10/synthesis#");
const rdf::ns rdf::subtag( "subtag", "http://rhdunn.github.com/cainteoir/data/iana/subtags#");
const rdf::ns rdf::tts(    "tts",    "http://reecedunn.co.uk/schema/2014/tts#");
const rdf::ns rdf::xhtml(  "h",      "http://www.w3.org/1999/xhtml#");
const rdf::ns rdf::xml(    "xml",    "http://www.w3.org/XML/1998/namespace#");
const rdf::ns rdf::xsd(    "xsd",    "http://www.w3.org/2001/XMLSchema#");

std::shared_ptr<const rdf::resource> rdf::literal::clone() const
{
	return std::make_shared<literal>(*this);
}

const rql::detail::subject_t   rql::subject;
const rql::detail::predicate_t rql::predicate;
const rql::detail::object_t    rql::object;

rdf::graph::graph() : mContext(std::make_shared<context>())
{
	add_namespace("http",   "http:");
	add_namespace("https",  "https:");
	add_namespace("mailto", "mailto:");
	add_namespace("file",   "file:");
}

rdf::graph::graph(rdf::graph &aGraph) : mContext(aGraph.mContext)
{
	add_namespace("http",   "http:");
	add_namespace("https",  "https:");
	add_namespace("mailto", "mailto:");
	add_namespace("file",   "file:");
}

bool rdf::graph::contains(const ns &uri) const
{
	return namespaces.find(uri.href) != namespaces.end();
}

rdf::graph &rdf::graph::set_base(const std::string &aBase)
{
	mContext->baseUri = aBase;
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

const rdf::uri rdf::graph::genid()
{
	std::ostringstream id;
	id << "genid" << mContext->nextId;
	++mContext->nextId;
	return bnode(id.str());
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
		uri = mContext->baseUri + aCurie;
	return std::make_shared<rdf::uri>(href(uri));
}

void rdf::graph::curie_list(const std::string &aCurieList,
                            const std::function<void (const rdf::uri &aUri)> &onuri)
{
	std::string rel;
	std::istringstream ss(aCurieList);
	while (ss >> rel)
	{
		std::shared_ptr<const rdf::uri> uri = curie(rel);
		if (uri.get() && !uri->ns.empty())
			onuri(*uri);
	}
}

void cainteoir::rdf::query::rdf_list_items(const rdf::graph &aMetadata,
                                           const rdf::uri &aSubject,
                                           const rdf::uri &aPredicate,
                                           const std::function<void (const std::shared_ptr<const triple> &aStatement)> &onlistitem)
{
	auto start = rql::select(aMetadata, rql::subject == aSubject && rql::predicate == aPredicate);
	if (start.empty()) return;

	const rdf::uri *item = &rql::object(start.front());
	while (item)
	{
		auto first = rql::select(aMetadata, rql::subject == *item && rql::predicate == rdf::rdf("first"));
		if (!first.empty())
			onlistitem(first.front());

		auto rest = rql::select(aMetadata, rql::subject == *item && rql::predicate == rdf::rdf("rest"));
		if (!rest.empty())
		{
			item = &rql::object(rest.front());
			if (*item == rdf::rdf("nil"))
				item = nullptr;
		}
		else
			item = nullptr;
	}
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

	triples.push_back(std::make_shared<triple>(aSubject, aPredicate, aObject.clone()));
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

	triples.push_back(std::make_shared<triple>(aSubject, aPredicate, aObject.clone()));
	return true;
}
