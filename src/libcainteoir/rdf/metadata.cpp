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
namespace rql = cainteoir::rdf::query;

/** @struct cainteoir::rdf::resource
  * @brief  Manages an RDF resource.
  */

/** @fn    cainteoir::rdf::resource::~resource
  * @brief Clean up the resource.
  */

/** @fn    std::shared_ptr<const cainteoir::rdf::resource> cainteoir::rdf::resource::clone()
  * @brief Create a new instance of this resource object.
  *
  * @return The new copy of the resource.
  */

/** @struct cainteoir::rdf::uri
  * @brief  Represents an RDF URI resource.
  *
  * In RDF, a URI is used to locate items that can be described. This includes
  * subjects (describing information about the item), predicates (describing
  * the type of connection) and objects (connecting two resources together,
  * e.g. linking an author to a novel).
  */

/** @var   std::string cainteoir::rdf::uri::ns
  * @brief The namespace to which the URI resource belongs.
  */

/** @var   std::string cainteoir::rdf::uri::ref
  * @brief The URI reference.
  */

/** @brief Create a new URI object.
  *
  * @param[in] aNS  The namespace of the URI.
  * @param[in] aRef The reference of the URI.
  */
rdf::uri::uri(const std::string &aNS, const std::string &aRef)
	: ns(aNS)
	, ref(aRef)
{
	auto last = --ns.end();
	if (!ns.empty() && !ref.empty() && *last != '#' && *last != '/' && *last != ':')
		ns.push_back('#');
}

/** @brief Is the URI empty?
  *
  * @retval true  If the namespace and reference of the URI are empty.
  * @retval false If the namespace and reference of the URI are not empty.
  */
bool rdf::uri::empty() const
{
	return ns.empty() && ref.empty();
}

/** @brief Convert the URI to a string.
  *
  * @return The combined namespace and reference forming the URI.
  */
std::string rdf::uri::str() const
{
	if (ref.empty())
		return ns;

	return ns + ref;
}

// documentation inherited from rdf::resource
std::shared_ptr<const rdf::resource> rdf::uri::clone() const
{
	return std::make_shared<uri>(*this);
}

/** @fn    bool cainteoir::rdf::operator==(const cainteoir::rdf::uri &a, const cainteoir::rdf::uri &b)
  * @brief Compare two URIs for equlity.
  *
  * @param[in] a The first URI to compare.
  * @param[in] b The second URI to compare.
  *
  * @retval true  If the two URIs are equal.
  * @retval false If the two URIs are not equal.
  */

/** @fn    bool cainteoir::rdf::operator!=(const cainteoir::rdf::uri &a, const cainteoir::rdf::uri &b)
  * @brief Compare two URIs for inequlity.
  *
  * @param[in] a The first URI to compare.
  * @param[in] b The second URI to compare.
  *
  * @retval true  If the two URIs are not equal.
  * @retval false If the two URIs are equal.
  */

/** @struct cainteoir::rdf::ns
  * @brief  Manages RDF namespaces used for creating RDF URIs and CURIEs.
  */

/** @fn    cainteoir::rdf::ns::ns(const std::string &aPrefix, const std::string &aHref)
  * @brief Create an RDF namespace object.
  *
  * @param[in] aPrefix The short name by which this namespace is to be known.
  * @param[in] aHref   The base URI of the namespace.
  */

/** @fn    cainteoir::rdf::uri cainteoir::rdf::ns::operator()(const std::string &aRef) const
  * @brief Create a URI in the namespace.
  *
  * @param aRef The URI reference relative to the namespace.
  */

/** @fn    bool cainteoir::rdf::operator==(const std::string &a, const cainteoir::rdf::ns &b)
  * @brief Check if a URI is referenced by a namespace.
  *
  * @param[in] a The URI to check for.
  * @param[in] b The namespace to compare against.
  *
  * @retval true  If the namespace is for the specified URI.
  * @retval false If the namespace is not for the specified URI.
  *
  * This comparison will ignore the prefix of the namespace.
  */

/** @fn    bool cainteoir::rdf::operator==(const cainteoir::rdf::ns &a, const std::string &b)
  * @brief Check if a URI is referenced by a namespace.
  *
  * @param[in] a The namespace to compare against.
  * @param[in] b The URI to check for.
  *
  * @retval true  If the namespace is for the specified URI.
  * @retval false If the namespace is not for the specified URI.
  *
  * This comparison will ignore the prefix of the namespace.
  */

/** @brief RDF blank node.
  */
const rdf::ns rdf::bnode("_", "_:");

/** @brief RDF syntax namespace.
  */
const rdf::ns rdf::rdf("rdf", "http://www.w3.org/1999/02/22-rdf-syntax-ns#");

/** @brief RDF attributes (RDFa) namespace.
  */
const rdf::ns rdf::rdfa("rdfa", "http://www.w3.org/ns/rdfa#");

/** @brief RDF schema namespace.
  */
const rdf::ns rdf::rdfs("rdfs", "http://www.w3.org/2000/01/rdf-schema#");

/** @brief XMLSchema namespace.
  */
const rdf::ns rdf::xsd("xsd", "http://www.w3.org/2001/XMLSchema#");

/** @brief XML namespace.
  */
const rdf::ns rdf::xml("xml", "http://www.w3.org/XML/1998/namespace#");

/** @brief OWL Ontology namespace.
  */
const rdf::ns rdf::owl("owl", "http://www.w3.org/2002/07/owl#");

/** @brief Dublin Core: Elements namespace.
  */
const rdf::ns rdf::dc("dc", "http://purl.org/dc/elements/1.1/");

/** @brief Dublin Core: Terms namespace.
  */
const rdf::ns rdf::dcterms("dct", "http://purl.org/dc/terms/");

/** @brief DCMI Abstract Model namespace.
  */
const rdf::ns rdf::dcam("dcam", "http://purl.org/dc/dcam/");

/** @brief Daisy Talking Book (DTB) namespace.
  */
const rdf::ns rdf::dtb("dtb", "http://www.daisy.org/z3986/2005/dtbook/");

/** @brief Navigation Control File (NCX) namespace.
  */
const rdf::ns rdf::ncx("ncx", "http://www.daisy.org/z3986/2005/ncx/");

/** @brief ePub 3.0 (OPS) namespace.
  */
const rdf::ns rdf::epub("epub", "http://www.idpf.org/2007/ops#");

/** @brief Open Container Format (OCF) namespace.
  */
const rdf::ns rdf::opf("opf", "http://www.idpf.org/2007/opf#");

/** @brief Open Publication Format (OPF) namespace.
  */
const rdf::ns rdf::ocf("ocf", "urn:oasis:names:tc:opendocument:xmlns:container#");

/** @brief ePub 3.0 package vocabulary namespace.
  */
const rdf::ns rdf::pkg("pkg", "http://www.idpf.org/epub/vocab/package/#");

/** @brief ePub 3.0 media overlay vocabulary namespace.
  */
const rdf::ns rdf::media("media", "http://www.idpf.org/epub/vocab/overlays/#");

/** @brief ONIX namespace.
  */
const rdf::ns rdf::onix("onix", "http://www.editeur.org/ONIX/book/codelists/current.html#");

/** @brief MARC namespace.
  */
const rdf::ns rdf::marc("marc", "http://id.loc.gov/vocabulary/");

/** @brief Speech Synthesis Markup Language (SSML) namespace.
  */
const rdf::ns rdf::ssml("ssml", "http://www.w3.org/2001/10/synthesis#");

/** @brief SMIL namespace.
  */
const rdf::ns rdf::smil("smil", "http://www.w3.org/ns/SMIL#");

/** @brief XML-based HyperText Markup Language (XHTML) namespace.
  */
const rdf::ns rdf::xhtml("h", "http://www.w3.org/1999/xhtml#");

/** @brief SKOS namespace.
  */
const rdf::ns rdf::skos("skos", "http://www.w3.org/2004/02/skos/core#");

/** @brief Friend of a Friend (FOAF) namespace.
  */
const rdf::ns rdf::foaf("foaf", "http://xmlns.com/foaf/0.1/");

/** @brief Cainteoir Text-to-Speech RDF namespace.
  */
const rdf::ns rdf::tts("tts", "http://rhdunn.github.com/2010/12/text-to-speech#");

/** @brief IANA Language Subtag Repository RDF schema namespace.
  */
const rdf::ns rdf::iana("iana", "http://rhdunn.github.com/cainteoir/schema/iana#");

/** @brief IANA Language Subtag Repository RDF namespace.
  */
const rdf::ns rdf::subtag("subtag", "http://rhdunn.github.com/cainteoir/data/iana/subtags#");

/** @class cainteoir::rdf::literal
  * @brief Manages an RDF literal resource.
  *
  * An RDF literal is a string of characters that may also have an associated
  * language or datatype. A literal can only be used as the object of an RDF
  * triple.
  */

/** @var   cainteoir::rdf::literal::value
  * @brief The content of the literal.
  */

/** @var   cainteoir::rdf::literal::language
  * @brief The language the literal is written in.
  */

/** @var   cainteoir::rdf::literal::type
  * @brief The datatype of the literal.
  */

/** @fn    cainteoir::rdf::literal::literal()
  * @brief Create an empty literal resource.
  */

/** @fn    cainteoir::rdf::literal::literal(const T &aValue)
  * @brief Create a value literal resource.
  *
  * @param[in] aValue The value to store in the literal.
  */

/** @fn    cainteoir::rdf::literal::literal(const T &aValue, const std::string &aLanguage)
  * @brief Create a language literal resource.
  *
  * @param[in] aValue    The value to store in the literal.
  * @param[in] aLanguage The language @a aValue is written in.
  */

/** @fn    cainteoir::rdf::literal::literal(const T &aValue, const cainteoir::rdf::uri &aType)
  * @brief Create a datatype literal resource.
  *
  * @param[in] aValue The value to store in the literal.
  * @param[in] aType  The datatype of @a aValue.
  */

/** @fn    cainteoir::rdf::literal &cainteoir::rdf::literal::operator=(const T &aValue)
  * @brief Assign a new value to the literal resource.
  *
  * @param[in] aValue The value to store in the literal.
  *
  * @return The new literal object.
  *
  * If the literal has a language or datatype set, this will preserve those values.
  */

/** @fn    T cainteoir::rdf::literal::as()
  * @brief Convert the value of the literal to the specified type.
  *
  * @return The value of the literal.
  */

// documentation inherited from rdf::resource
std::shared_ptr<const rdf::resource> rdf::literal::clone() const
{
	return std::make_shared<literal>(*this);
}

/** @fn    bool cainteoir::rdf::operator==(const cainteoir::rdf::literal &lhs, const cainteoir::rdf::literal &rhs)
  * @brief Compare two literals for equlity.
  *
  * @param[in] lhs The first literal to compare.
  * @param[in] rhs The second literal to compare.
  *
  * @retval true  If the two literals are equal.
  * @retval false If the two literals are not equal.
  */

/** @struct cainteoir::rdf::triple
  * @brief  An RDF triple.
  *
  * A triple in RDF is a (subject, predicate, object) statement that describes
  * a relation in the RDF graph.
  */

/** @var   const cainteoir::rdf::uri cainteoir::rdf::triple::subject
  * @brief The URI this statement is about.
  */

/** @var   const cainteoir::rdf::uri cainteoir::rdf::triple::predicate
  * @brief The URI describing the relationship expressed by this statement.
  */

/** @var   const std::shared_ptr<const cainteoir::rdf::resource> cainteoir::rdf::triple::object
  * @brief The URI or literal which is the value of this statement.
  */

/** @fn    cainteoir::rdf::triple::triple(const cainteoir::rdf::uri &aSubject, const cainteoir::rdf::uri &aPredicate, cont std::shared_ptr<const cainteoir::rdf::uri> &aObject)
  * @brief Create an RDF triple.
  *
  * @param[in] aSubject   The subject of the triple.
  * @param[in] aPredicate The predicate of the triple.
  * @param[in] aObject    The object of the triple.
  *
  * A triple in RDF is a (subject, predicate, object) statement that describes
  * a relation in the RDF graph.
  */

/** @brief Extract the subject of the RDF triple.
  */
const rql::detail::subject_t rql::subject;

/** @brief Extract the predicate of the RDF triple.
  */
const rql::detail::predicate_t rql::predicate;

/** @brief Extract the object of the RDF triple.
  */
const rql::detail::object_t rql::object;

/** @fn    const std::string &cainteoir::rdf::query::value(const std::shared_ptr<const rdf::triple> &statement)
  * @brief Extract the object literal's value of the RDF triple.
  *
  * @param[in] statement The RDF triple.
  *
  * @return The object literal's value. If the object is a URI resource, an empty string is returned.
  */

/** @typedef std::list<std::shared_ptr<const cainteoir::rdf::triple> > cainteoir::rdf::query::results
  * @brief   The set of RDF triples matching a select query.
  */

/** @struct cainteoir::rdf::triplestore
  * @brief  Hold a set of RDF triples in an RDF graph.
  */

/** @fn    void cainteoir::rdf::triplestore::push_back(const_reference item)
  * @brief Add an RDF triple to the triplestore.
  *
  * @param[in] item The RDF triple to add to the triplestore.
  */

/** @fn    cainteoir::rdf::query::results &cainteoir::rdf::triplestore::subject(const cainteoir::rdf::uri &s) const
  * @brief Locate the RDF triples for the specified subject.
  *
  * @param[in] s The subject of the RDF triples to select.
  *
  * @return The RDF triples matching the subject.
  *
  * This method provides an optimized path for select(subject, _, _) queries to
  * avoid O(N^2) performance when those queries are used in a loop over a set of
  * subjects.
  */

/** @struct cainteoir::rdf::graph
  * @brief  Manages an RDF graph.
  */

/** @typedef cainteoir::rdf::triplestore::size_type cainteoir::rdf::graph::size_type
  * @brief   The type used to hold size values.
  */

/** @typedef cainteoir::rdf::triplestore::const_iterator cainteoir::rdf::graph::const_iterator
  * @brief   An iterator over RDF triples.
  */

/** @typedef cainteoir::rdf::triplestore::const_reference cainteoir::rdf::graph::const_reference
  * @brief   A reference to an RDF triple.
  */

/** @brief Create a new RDF graph object.
  */
rdf::graph::graph() : nextid(1)
{
	add_namespace("http",   "http:");
	add_namespace("https",  "https:");
	add_namespace("mailto", "mailto:");
	add_namespace("file",   "file:");
}

/** @fn    cainteoir::rdf::graph::size_type cainteoir::rdf::graph::size() const
  * @brief Get the number of triples in the graph.
  *
  * @return The number of triples in the graph.
  */

/** @fn    bool cainteoir::rdf::graph::empty() const
  * @brief Is the graph empty?
  *
  * @retval true  If the graph does not contain any triple statements.
  * @retval false If the graph does contain triple statements.
  */

/** @fn    cainteoir::rdf::graph::const_iterator cainteoir::rdf::graph::begin() const
  * @brief Get the iterator to the first triple in the graph's triplestore.
  *
  * @return The iterator to the first triple in the graph's triplestore.
  */

/** @fn    cainteoir::rdf::graph::const_iterator cainteoir::rdf::graph::end() const
  * @brief Get the iterator to the last triple in the graph's triplestore.
  *
  * @return The iterator to the last triple in the graph's triplestore.
  */

/** @fn    cainteoir::rdf::graph::const_reference cainteoir::rdf::graph::front() const
  * @brief Get the first triple in the graph's triplestore.
  *
  * @return The first triple in the graph's triplestore.
  */

/** @fn    cainteoir::rdf::graph::const_reference cainteoir::rdf::graph::back() const
  * @brief Get the last triple in the graph's triplestore.
  *
  * @return The last triple in the graph's triplestore.
  */

/** @fn    cainteoir::rdf::query::results &cainteoir::rdf::graph::subject(const cainteoir::rdf::uri &s) const
  * @brief Locate the RDF triples for the specified subject.
  *
  * @param[in] s The subject of the RDF triples to select.
  *
  * @return The RDF triples matching the subject.
  *
  * This method provides an optimized path for select(subject, _, _) queries to
  * avoid O(N^2) performance when those queries are used in a loop over a set of
  * subjects.
  */

/** @brief Does the graph contain a URI in the specified namespace?
  *
  * @param[in] uri The namespace to check for.
  *
  * @retval true  If the graph contains a URI in the specified namespace.
  * @retval false If the graph does not contain a URI in the specified namespace.
  */
bool rdf::graph::contains(const ns &uri) const
{
	return namespaces.find(uri.href) != namespaces.end();
}

/** @brief Set the base URI for the graph.
  *
  * @param[in] aBase The base URI of the graph.
  *
  * @return This graph object (to support method chaining).
  */
rdf::graph &rdf::graph::set_base(const std::string &aBase)
{
	mBaseUri = aBase;
	return *this;
}

/** @brief Add a namespace to the graph.
  *
  * @param[in] aPrefix The name of the namespace URI.
  * @param[in] aHref   The namespace URI.
  *
  * @return This graph object (to support method chaining).
  *
  * This is used for generating \@prefix entries in turtle/n3 documents.
  */
rdf::graph &rdf::graph::add_namespace(const std::string &aPrefix, const std::string &aHref)
{
	char end = *(--aHref.end());
	if (end != '#' && end != '/' && end != ':')
		namespaces::add_namespace(aPrefix, aHref + '#');
	else
		namespaces::add_namespace(aPrefix, aHref);
	return *this;
}

/** @fn    cainteoir::rdf::graph &cainteoir::rdf::graph::add_namespace(const cainteoir::rdf::ns &ns)
  * @brief Add a namespace to the graph.
  *
  * @param[in] ns The namespace URI.
  *
  * @return This graph object (to support method chaining).
  *
  * This is used for generating \@prefix entries in turtle/n3 documents.
  */

/** @fn    cainteoir::rdf::graph &cainteoir::rdf::graph::operator<<(const cainteoir::rdf::ns &ns)
  * @brief Add a namespace to the graph.
  *
  * @param[in] ns The namespace URI.
  *
  * @return This graph to support method chaining.
  *
  * This is used for generating \@prefix entries in turtle/n3 documents.
  */

/** @brief Add namespaces in an RDFa \@prefix attribute.
  *
  * @param[in] aPrefix The RDFa prefix string containing the namespaces to add.
  *
  * @return This graph object (to support method chaining).
  *
  * This is used for generating \@prefix entries in turtle/n3 documents.
  */
rdf::graph &rdf::graph::add_prefix(const std::string &aPrefix)
{
	std::string prefix;
	std::string href;

	std::istringstream ss(aPrefix);
	while (ss >> prefix >> href)
		add_namespace(prefix.substr(0, prefix.find(':')), href);

	return *this;
}

/** @brief Convert a href into a URI object.
  *
  * @param[in] aHref The href to convert.
  *
  * @return The href as a URI object.
  */
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

/** @brief Generate a new blank node.
  *
  * @return A blank node that is unique for this graph.
  */
const rdf::uri rdf::graph::genid()
{
	std::ostringstream id;
	id << "genid" << nextid;
	++nextid;
	return bnode(id.str());
}

/** @brief Convert a CURIE into a URI object.
  *
  * @param[in] aCurie The CURIE to convert.
  *
  * @return The CURIE as a URI object.
  */
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

/** @brief Add an RDF statement to the graph.
  *
  * @param[in] aSubject   The subject of the statement.
  * @param[in] aPredicate The predicate of the statement.
  * @param[in] aObject    The object of the statement.
  *
  * @retval true  If the statement was added to the graph.
  * @retval false If the statement was not added to the graph.
  *
  * This enforces the RDF requirement that a predicate cannot be a blank node.
  */
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

/** @brief Add an RDF statement to the graph.
  *
  * @param[in] aSubject   The subject of the statement.
  * @param[in] aPredicate The predicate of the statement.
  * @param[in] aObject    The object of the statement.
  *
  * @retval true  If the statement was added to the graph.
  * @retval false If the statement was not added to the graph.
  *
  * This enforces the RDF requirement that a predicate cannot be a blank node.
  */
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

/** @fn    cainteoir::rdf::query::detail::matches_t<Selector, Value> cainteoir::rdf::query::matches(const Selector &aSelector, const Value &aValue)
  * @brief Match statements whos selector matches the value.
  *
  * @param aSelector The selector to extract data from the statement.
  * @param aValue    The value to match against.
  *
  * @return A function object for matching triples where @a aSelector returns @a aValue.
  */

/** @fn    cainteoir::rdf::query::detail::both_t<Selector1, Selector2> cainteoir::rdf::query::both(const Selector1 &a, const Selector2 &b)
  * @brief Match statements that match both selectors.
  *
  * @param a The first selector to match a statement against.
  * @param b The second selector to match a statement against.
  *
  * @return A function object for matching triples that match both @a a and @a b selectors.
  */

/** @fn    cainteoir::rdf::query::results cainteoir::rdf::query::select(const TripleStore &metadata, const Selector &selector)
  * @brief Select statements matching the selector.
  *
  * @param metadata The subgraph to select statements from.
  * @param selector The selector used to choose statements in the graph.
  *
  * @return A subgraph containing all matching statements.
  */

/** @fn    bool cainteoir::rdf::query::contains(const TripleStore &metadata, const Selector &selector)
  * @brief Check if the graph contains any of the specified statements.
  *
  * @param metadata The subgraph to check.
  * @param selector The selector used to choose statements in the graph.
  *
  * @retval true  If the graph contains a statement matching the selector.
  * @retval false If the graph does not contain a statement matching the selector.
  */

/** @fn    T cainteoir::rdf::query::select_value(const TripleStore &metadata, const Selector &selector)
  * @brief Select a value matching the selector.
  *
  * @param metadata The subgraph to select statements from.
  * @param selector The selector used to choose statements in the graph.
  *
  * @return The first literal value matching the selector.
  */
