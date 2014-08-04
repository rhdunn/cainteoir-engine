# cainteoir::rdf::resource {: .doc }

Manages an RDF resource.

# cainteoir::rdf::resource::~resource {: .doc }

Clean up the resource.

# cainteoir::rdf::resource::clone {: .doc }

Create a new instance of this resource object.

@return
: The new copy of the resource.

# cainteoir::rdf::uri {: .doc }

Represents an RDF URI resource.

In RDF, a URI is used to locate items that can be described. This includes
subjects (describing information about the item), predicates (describing
the type of connection) and objects (connecting two resources together,
e.g. linking an author to a novel).

# cainteoir::rdf::uri::ns {: .doc }

The namespace to which the URI resource belongs.

# cainteoir::rdf::uri::ref {: .doc }

The URI reference.

# cainteoir::rdf::uri::uri {: .doc }

Create a new URI object.

@aNS
: The namespace of the URI.

@aRef
: The reference of the URI.

# cainteoir::rdf::uri::empty {: .doc }

Is the URI empty?

@return
: `true` if the namespace and reference of the URI are empty, `false` otherwise.

# cainteoir::rdf::uri::str {: .doc }

Convert the URI to a string.

@return
: The combined namespace and reference forming the URI.

# cainteoir::rdf::operator== {: .doc }

Compare two URIs for equlity.

@a
: The first URI to compare.

@b
: The second URI to compare.

@return
: `true` if the two URIs are equal, `false` otherwise.

# cainteoir::rdf::operator!= {: .doc }

Compare two URIs for inequlity.

@a
: The first URI to compare.

@b
: The second URI to compare.

@return
: `true` if the two URIs are not equal, `false` otherwise.

# cainteoir::rdf::ns {: .doc }

Manages RDF namespaces used for creating RDF URIs and CURIEs.

# cainteoir::rdf::ns::ns {: .doc }

Create an RDF namespace object.

@aPrefix
: The short name by which this namespace is to be known.

@aHref
: The base URI of the namespace.

# cainteoir::rdf::ns::operator() {: .doc }

Create a URI in the namespace.

@aRef
: The URI reference relative to the namespace.

# cainteoir::rdf::operator== {: .doc }

Check if a URI is referenced by a namespace.

@a
: The URI to check for.

@b
: The namespace to compare against.

This comparison will ignore the prefix of the namespace.

@return
: `true` if the namespace is for the specified URI, `false` otherwise.

# cainteoir::rdf::operator== {: .doc }

Check if a URI is referenced by a namespace.

@a
: The namespace to compare against.

@b
: The URI to check for.

This comparison will ignore the prefix of the namespace.

@return
: `true` if the namespace is for the specified URI, `false` otherwise.

# cainteoir::rdf::bnode {: .doc }

RDF blank node.

# cainteoir::rdf::dc {: .doc }

Dublin Core: Elements namespace.

# cainteoir::rdf::dcam {: .doc }

DCMI Abstract Model namespace.

# cainteoir::rdf::dcterms {: .doc }

Dublin Core: Terms namespace.

# cainteoir::rdf::dtb {: .doc }

Daisy Talking Book (DTB) namespace.

# cainteoir::rdf::epub {: .doc }

ePub 3.0 (OPS) namespace.

# cainteoir::rdf::epv {: .doc }

ePub 3.0 vocabulary for epub:type relations.

# cainteoir::rdf::foaf {: .doc }

Friend of a Friend (FOAF) namespace.

# cainteoir::rdf::iana {: .doc }

IANA Language Subtag Repository RDF schema namespace.

# cainteoir::rdf::marc {: .doc }

MARC namespace.

# cainteoir::rdf::media {: .doc }

ePub 3.0 media overlay vocabulary namespace.

# cainteoir::rdf::ncx {: .doc }

Navigation Control File (NCX) namespace.

# cainteoir::rdf::ocf {: .doc }

Open Publication Format (OPF) namespace.

# cainteoir::rdf::onix {: .doc }

ONIX namespace.

# cainteoir::rdf::opf {: .doc }

Open Container Format (OCF) namespace.

# cainteoir::rdf::owl {: .doc }

OWL Ontology namespace.

# cainteoir::rdf::pkg {: .doc }

ePub 3.0 package vocabulary namespace.

# cainteoir::rdf::rdf {: .doc }

RDF syntax namespace.

# cainteoir::rdf::rdfa {: .doc }

RDF attributes (RDFa) namespace.

# cainteoir::rdf::rdfs {: .doc }

RDF schema namespace.

# cainteoir::rdf::ref {: .doc }

Metadata for describing Document References such as table of contents.

# cainteoir::rdf::skos {: .doc }

SKOS namespace.

# cainteoir::rdf::smil {: .doc }

SMIL namespace.

# cainteoir::rdf::ssml {: .doc }

Speech Synthesis Markup Language (SSML) namespace.

# cainteoir::rdf::subtag {: .doc }

IANA Language Subtag Repository RDF namespace.

# cainteoir::rdf::tts {: .doc }

Cainteoir Text-to-Speech RDF namespace.

# cainteoir::rdf::xhtml {: .doc }

XML-based HyperText Markup Language (XHTML) namespace.

# cainteoir::rdf::xml {: .doc }

XML namespace.

# cainteoir::rdf::xsd {: .doc }

XMLSchema namespace.

# cainteoir::rdf::literal {: .doc }

Manages an RDF literal resource.

An RDF literal is a string of characters that may also have an associated
language or datatype. A literal can only be used as the object of an RDF
triple.

# cainteoir::rdf::literal::value {: .doc }

The content of the literal.

# cainteoir::rdf::literal::language {: .doc }

The language the literal is written in.

# cainteoir::rdf::literal::type {: .doc }

The datatype of the literal.

# cainteoir::rdf::literal::literal {: .doc }

Create an empty literal resource.

# cainteoir::rdf::literal::literal {: .doc }

Create a value literal resource.

@aValue
: The value to store in the literal.

# cainteoir::rdf::literal::literal {: .doc }

Create a language literal resource.

@aValue
: The value to store in the literal.

@aLanguage
: The language `aValue` is written in.

# cainteoir::rdf::literal::literal {: .doc }

Create a datatype literal resource.

@aValue
: The value to store in the literal.

@aType
: The datatype of `aValue`.

# cainteoir::rdf::literal::operator= {: .doc }

Assign a new value to the literal resource.

@aValue
: The value to store in the literal.

If the literal has a language or datatype set, this will preserve those values.

@return
: The new literal object.

# cainteoir::rdf::literal::as {: .doc }

Convert the value of the literal to the specified type.

@return
: The value of the literal.

# cainteoir::rdf::operator== {: .doc }

Compare two literals for equlity.

@lhs
: The first literal to compare.

@rhs
: The second literal to compare.

@return
: `true` if the two literals are equal, `false` otherwise.

# cainteoir::rdf::triple {: .doc }

An RDF triple.

A triple in RDF is a `(subject, predicate, object)` statement that describes
a relation in the RDF graph.

# cainteoir::rdf::triple::subject {: .doc }

The URI this statement is about.

# cainteoir::rdf::triple::predicate {: .doc }

The URI describing the relationship expressed by this statement.

# cainteoir::rdf::triple::object {: .doc }

The URI or literal which is the value of this statement.

# cainteoir::rdf::triple::triple {: .doc }

Create an RDF triple.

@aSubject
: The subject of the triple.

@aPredicate
: The predicate of the triple.

@aObject
: The object of the triple.

A triple in RDF is a `(subject, predicate, object)` statement that describes
a relation in the RDF graph.

# cainteoir::rdf::query::subject {: .doc }

Extract the subject of the RDF triple.

# cainteoir::rdf::query::predicate {: .doc }

Extract the predicate of the RDF triple.

# cainteoir::rdf::query::object {: .doc }

Extract the object of the RDF triple.

# cainteoir::rdf::query::literal {: .doc }

Extract the object literal of the RDF triple.

@statement
: The RDF triple.

@return
: The object literal. If the object is a URI resource, an empty literal is
  returned.

# cainteoir::rdf::query::value {: .doc }

Extract the object literal's value of the RDF triple.

@statement
: The RDF triple.

@return
: The object literal's value. If the object is a URI resource, an empty string
  is returned.

# cainteoir::rdf::query::results {: .doc }

The set of RDF triples matching a select query.

# cainteoir::rdf::triplestore {: .doc }

Hold a set of RDF triples in an RDF graph.

# cainteoir::rdf::triplestore::push_back {: .doc }

Add an RDF triple to the triplestore.

@item
: The RDF triple to add to the triplestore.

# cainteoir::rdf::triplestore::subject {: .doc }

Locate the RDF triples for the specified subject.

@s
: The subject of the RDF triples to select.

This method provides an optimized path for `select(subject, _, _)` queries to
avoid O(N^2) performance when those queries are used in a loop over a set of
subjects.

@return
: The RDF triples matching the subject.

# cainteoir::rdf::graph {: .doc }

Manages an RDF graph.

# cainteoir::rdf::graph::size_type {: .doc }

The type used to hold size values.

# cainteoir::rdf::graph::const_iterator {: .doc }

An iterator over RDF triples.

# cainteoir::rdf::graph::const_reference {: .doc }

A reference to an RDF triple.

# cainteoir::rdf::graph::graph {: .doc }

Create a new RDF graph object.

# cainteoir::rdf::graph::graph {: .doc }

Create a new RDF graph object suitable for merging with the parent graph.

@aGraph
: The graph to share genids with.

The output of the genid function will be unique across this graph and `aGraph`,
allowing the statements of the two graphs to be merged.

This does not copy the statements of `aGraph` into the new graph.

# cainteoir::rdf::graph::size {: .doc }

Get the number of triples in the graph.

@return
: The number of triples in the graph.

# cainteoir::rdf::graph::empty {: .doc }

Is the graph empty?

@return
: `true` if the graph does not contain any triple statements, `false` otherwise.

# cainteoir::rdf::graph::begin {: .doc }

Get the iterator to the first triple in the graph's triplestore.

@return
: The iterator to the first triple in the graph's triplestore.

# cainteoir::rdf::graph::end {: .doc }

Get the iterator to the last triple in the graph's triplestore.

@return
: The iterator to the last triple in the graph's triplestore.

# cainteoir::rdf::graph::front {: .doc }

Get the first triple in the graph's triplestore.

@return
: The first triple in the graph's triplestore.

# cainteoir::rdf::graph::back {: .doc }

Get the last triple in the graph's triplestore.

@return
: The last triple in the graph's triplestore.

# cainteoir::rdf::graph::subject {: .doc }

Locate the RDF triples for the specified subject.

@s
: The subject of the RDF triples to select.

This method provides an optimized path for `select(subject, _, _)` queries to
avoid O(N^2) performance when those queries are used in a loop over a set of
subjects.

@return
: The RDF triples matching the subject.

# cainteoir::rdf::graph::contains {: .doc }

Does the graph contain a URI in the specified namespace?

@uri
: The namespace to check for.

@return
: `true` if the graph contains a URI in the specified namespace, `false` otherwise.

# cainteoir::rdf::graph::set_base {: .doc }

Set the base URI for the graph.

@aBase
: The base URI of the graph.

@return
: This graph object (to support method chaining).

# cainteoir::rdf::graph::get_base {: .doc }

Get the base URI for the graph.

@return
: The base URI of the graph.

# cainteoir::rdf::graph::add_namespace {: .doc }

Add a namespace to the graph.

@aPrefix
: The name of the namespace URI.

@aHref
: The namespace URI.

This is used for generating `@prefix` entries in turtle/n3 documents.

@return
: This graph object (to support method chaining).

# cainteoir::rdf::graph::add_namespace {: .doc }

Add a namespace to the graph.

@ns
: The namespace URI.

This is used for generating `@prefix` entries in turtle/n3 documents.

@return
: This graph object (to support method chaining).

# cainteoir::rdf::graph::operator<< {: .doc }

Add a namespace to the graph.

@ns
: The namespace URI.

This is used for generating `@prefix` entries in turtle/n3 documents.

@return
: This graph to support method chaining.

# cainteoir::rdf::graph::add_prefix {: .doc }

Add namespaces in an RDFa `@prefix` attribute.

@aPrefix
: The RDFa prefix string containing the namespaces to add.

This is used for generating `@prefix` entries in turtle/n3 documents.

@return
: This graph object (to support method chaining).

# cainteoir::rdf::href {: .doc }

Convert a href into a URI object.

@aHref
: The href to convert.

@return
: The href as a URI object.

# cainteoir::rdf::graph::genid {: .doc }

Generate a new blank node.

@return
: A blank node that is unique for this graph.

# cainteoir::rdf::graph::curie {: .doc }

Convert a CURIE into a URI object.

@aCurie
: The CURIE to convert.

@return
: The CURIE as a URI object.

# cainteoir::rdf::graph::curie_list {: .doc }

Convert a space-separated CURIE-list into a sequence of URI objects.

@aCurieList
: The space-separated CURIE-list to convert.

@onuri
: The callback to be called when a URI is parsed from the CURIE list.

# cainteoir::rdf::query::rdf_list_items {: .doc }

Iterate over the items of an RDF list.

@aMetadata
: The RDF graph containing the list metadata.

@aSubject
: The subject of the RDF list to iterate over.

@aPredicate
: The predicate of the RDF list to iterate over.

@onlistitem
: The callback function to call on each item in the RDF list.

An RDF list is a chain of nodes where the rdf:first predicate references the
current value and rdf:rest references the next node in the chain. An rdf:nil
node is used to terminate the list. The RDF/XML, Turtle and other RDF syntaxes
provide a higher level way of specifying RDF lists that hide this detail.

This function provides an easy way for iterating over the items in such an RDF
list. For example:

	cainteoir::rdf::graph g;
	auto reader = cainteoir::createDocumentReader("example19.rdf", g, std::string());

	cainteoir::rdf::ns ex(   "ex",    "http://example.org/");
	cainteoir::rdf::ns stuff("stuff", "http://example.org/stuff/1.0/");

	g.foreach(ex("basket"), stuff("hasFruit"),
	          [](const std::shared_ptr<const cainteoir::rdf::triple> &subject)
	{
		std::cout << cainteoir::rdf::query::object(subject).str() << std::endl;
	});

which, when using example19.rdf from the RDF/XML Syntax Specification, will
print:

	http://example.org/banana
	http://example.org/apple
	http://example.org/pear

# cainteoir::rdf::graph::statement {: .doc }

Add an RDF statement to the graph.

@aSubject
: The subject of the statement.

@aPredicate
: The predicate of the statement.

@aObject
: The object of the statement.

This enforces the RDF requirement that a predicate cannot be a blank node.

@return
: `true` if the statement was added to the graph, `false` otherwise.

# cainteoir::rdf::graph::statement {: .doc }

Add an RDF statement to the graph.

@aSubject
: The subject of the statement.

@aPredicate
: The predicate of the statement.

@aObject
: The object of the statement.

This enforces the RDF requirement that a predicate cannot be a blank node.

@return
: `true` if the statement was added to the graph, `false` otherwise.

# cainteoir::rdf::query::matches {: .doc }

Match statements whos selector matches the value.

@aSelector
: The selector to extract data from the statement.

@aValue
: The value to match against.

@return
: A function object for matching triples where `aSelector` returns `aValue`.

# cainteoir::rdf::query::both {: .doc }

Match statements that match both selectors.

@a
: The first selector to match a statement against.

@b
: The second selector to match a statement against.

@return
: A function object for matching triples that match both `a` and `b` selectors.

# cainteoir::rdf::query::select {: .doc }

Select statements matching the selector.

@metadata
: The subgraph to select statements from.

@selector
: The selector used to choose statements in the graph.

@return
: A subgraph containing all matching statements.

# cainteoir::rdf::query::contains {: .doc }

Check if the graph contains any of the specified statements.

@metadata
: The subgraph to check.

@selector
: The selector used to choose statements in the graph.

@return
: `true` if the graph contains a statement matching the selector, `false` otherwise.

# cainteoir::rdf::query::select_value {: .doc }

Select a value matching the selector.

@metadata
: The subgraph to select statements from.

@selector
: The selector used to choose statements in the graph.

@return
: The first literal value matching the selector.

# cainteoir::rdf::create_formatter {: .doc }

Create an RDF serializer.

@aStream
: The output stream to serialize the RDF data to.

@aFormatType
: The format to serialize the RDF data as.

@return
: The formatter object.

# cainteoir::rdf::formatter {: .doc }

An interface for serializing RDF graphs.

# cainteoir::rdf::formatter::~formatter {: .doc }

Clean up the formatter object.

# cainteoir::rdf::formatter::operator<< {: .doc }

Serialize a namespace.

@aNS
: The namespace to serialize.

@return
: The formatter object (to support method chaining).

# cainteoir::rdf::formatter::operator<< {: .doc }

Serialize a URI resource.

@uri
: The URI resource to serialize.

@return
: The formatter object (to support method chaining).

# cainteoir::rdf::formatter::operator<< {: .doc }

Serialize a literal resource.

@literal
: The literal resource to serialize.

@return
: The formatter object (to support method chaining).

# cainteoir::rdf::formatter::operator<< {: .doc }

Serialize an RDF statement.

@statement
: The statement to serialize.

@return
: The formatter object (to support method chaining).

# cainteoir::rdf::formatter::operator<< {: .doc }

Serialize an RDF graph.

@aGraph
: The RDF graph to serialize.

@return
: The formatter object (to support method chaining).

# cainteoir::rdf::formatter::format_type {: .doc }

The format used to serialize the RDF statements as.

# cainteoir::rdf::formatter::ntriple {: .doc }

N-Triple serialization format.

# cainteoir::rdf::formatter::turtle {: .doc }

Turtle serialization format.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
