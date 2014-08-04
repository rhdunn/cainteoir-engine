#<cldoc:cainteoir::rdf::resource>

Manages an RDF resource.

#<cldoc:cainteoir::rdf::resource::~resource>

Clean up the resource.

#<cldoc:cainteoir::rdf::resource::clone>

Create a new instance of this resource object.

@return The new copy of the resource.

#<cldoc:cainteoir::rdf::uri>

Represents an RDF URI resource.

In RDF, a URI is used to locate items that can be described. This includes
subjects (describing information about the item), predicates (describing
the type of connection) and objects (connecting two resources together,
e.g. linking an author to a novel).

#<cldoc:cainteoir::rdf::uri::ns>

The namespace to which the URI resource belongs.

#<cldoc:cainteoir::rdf::uri::ref>

The URI reference.

#<cldoc:cainteoir::rdf::uri::uri>

Create a new URI object.
@aNS  The namespace of the URI.
@aRef The reference of the URI.

#<cldoc:cainteoir::rdf::uri::empty>

Is the URI empty?

@return `true` if the namespace and reference of the URI are empty, `false` otherwise.

#<cldoc:cainteoir::rdf::uri::str>

Convert the URI to a string.

@return The combined namespace and reference forming the URI.

#<cldoc:cainteoir::rdf::operator==>

Compare two URIs for equlity.
@a The first URI to compare.
@b The second URI to compare.

@return `true` if the two URIs are equal, `false` otherwise.

#<cldoc:cainteoir::rdf::operator!=>

Compare two URIs for inequlity.
@a The first URI to compare.
@b The second URI to compare.

@return `true` if the two URIs are not equal, `false` otherwise.

#<cldoc:cainteoir::rdf::ns>

Manages RDF namespaces used for creating RDF URIs and CURIEs.

#<cldoc:cainteoir::rdf::ns::ns>

Create an RDF namespace object.
@aPrefix The short name by which this namespace is to be known.
@aHref   The base URI of the namespace.

#<cldoc:cainteoir::rdf::ns::operator()>

Create a URI in the namespace.
@aRef The URI reference relative to the namespace.

#<cldoc:cainteoir::rdf::operator==>

Check if a URI is referenced by a namespace.
@a The URI to check for.
@b The namespace to compare against.

This comparison will ignore the prefix of the namespace.

@return `true` if the namespace is for the specified URI, `false` otherwise.

#<cldoc:cainteoir::rdf::operator==>

Check if a URI is referenced by a namespace.
@a The namespace to compare against.
@b The URI to check for.

This comparison will ignore the prefix of the namespace.

@return `true` if the namespace is for the specified URI, `false` otherwise.

#<cldoc:cainteoir::rdf::bnode>

RDF blank node.

#<cldoc:cainteoir::rdf::dc>

Dublin Core: Elements namespace.

#<cldoc:cainteoir::rdf::dcam>

DCMI Abstract Model namespace.

#<cldoc:cainteoir::rdf::dcterms>

Dublin Core: Terms namespace.

#<cldoc:cainteoir::rdf::dtb>

Daisy Talking Book (DTB) namespace.

#<cldoc:cainteoir::rdf::epub>

ePub 3.0 (OPS) namespace.

#<cldoc:cainteoir::rdf::epv>

ePub 3.0 vocabulary for epub:type relations.

#<cldoc:cainteoir::rdf::foaf>

Friend of a Friend (FOAF) namespace.

#<cldoc:cainteoir::rdf::iana>

IANA Language Subtag Repository RDF schema namespace.

#<cldoc:cainteoir::rdf::marc>

MARC namespace.

#<cldoc:cainteoir::rdf::media>

ePub 3.0 media overlay vocabulary namespace.

#<cldoc:cainteoir::rdf::ncx>

Navigation Control File (NCX) namespace.

#<cldoc:cainteoir::rdf::ocf>

Open Publication Format (OPF) namespace.

#<cldoc:cainteoir::rdf::onix>

ONIX namespace.

#<cldoc:cainteoir::rdf::opf>

Open Container Format (OCF) namespace.

#<cldoc:cainteoir::rdf::owl>

OWL Ontology namespace.

#<cldoc:cainteoir::rdf::pkg>

ePub 3.0 package vocabulary namespace.

#<cldoc:cainteoir::rdf::rdf>

RDF syntax namespace.

#<cldoc:cainteoir::rdf::rdfa>

RDF attributes (RDFa) namespace.

#<cldoc:cainteoir::rdf::rdfs>

RDF schema namespace.

#<cldoc:cainteoir::rdf::ref>

Metadata for describing Document References such as table of contents.

#<cldoc:cainteoir::rdf::skos>

SKOS namespace.

#<cldoc:cainteoir::rdf::smil>

SMIL namespace.

#<cldoc:cainteoir::rdf::ssml>

Speech Synthesis Markup Language (SSML) namespace.

#<cldoc:cainteoir::rdf::subtag>

IANA Language Subtag Repository RDF namespace.

#<cldoc:cainteoir::rdf::tts>

Cainteoir Text-to-Speech RDF namespace.

#<cldoc:cainteoir::rdf::xhtml>

XML-based HyperText Markup Language (XHTML) namespace.

#<cldoc:cainteoir::rdf::xml>

XML namespace.

#<cldoc:cainteoir::rdf::xsd>

XMLSchema namespace.

#<cldoc:cainteoir::rdf::literal>

Manages an RDF literal resource.

An RDF literal is a string of characters that may also have an associated
language or datatype. A literal can only be used as the object of an RDF
triple.

#<cldoc:cainteoir::rdf::literal::value>

The content of the literal.

#<cldoc:cainteoir::rdf::literal::language>

The language the literal is written in.

#<cldoc:cainteoir::rdf::literal::type>

The datatype of the literal.

#<cldoc:cainteoir::rdf::literal::literal>

Create an empty literal resource.

#<cldoc:cainteoir::rdf::literal::literal>

Create a value literal resource.
@aValue The value to store in the literal.

#<cldoc:cainteoir::rdf::literal::literal>

Create a language literal resource.
@aValue    The value to store in the literal.
@aLanguage The language `aValue` is written in.

#<cldoc:cainteoir::rdf::literal::literal>

Create a datatype literal resource.
@aValue The value to store in the literal.
@aType  The datatype of `aValue`.

#<cldoc:cainteoir::rdf::literal::operator=>

Assign a new value to the literal resource.
@aValue The value to store in the literal.

If the literal has a language or datatype set, this will preserve those values.

@return The new literal object.

#<cldoc:cainteoir::rdf::literal::as>

Convert the value of the literal to the specified type.

@return The value of the literal.

#<cldoc:cainteoir::rdf::operator==>

Compare two literals for equlity.
@lhs The first literal to compare.
@rhs The second literal to compare.

@return `true` if the two literals are equal, `false` otherwise.

#<cldoc:cainteoir::rdf::triple>

An RDF triple.

A triple in RDF is a `(subject, predicate, object)` statement that describes
a relation in the RDF graph.

#<cldoc:cainteoir::rdf::triple::subject>

The URI this statement is about.

#<cldoc:cainteoir::rdf::triple::predicate>

The URI describing the relationship expressed by this statement.

#<cldoc:cainteoir::rdf::triple::object>

The URI or literal which is the value of this statement.

#<cldoc:cainteoir::rdf::triple::triple>

Create an RDF triple.
@aSubject   The subject of the triple.
@aPredicate The predicate of the triple.
@aObject    The object of the triple.

A triple in RDF is a `(subject, predicate, object)` statement that describes
a relation in the RDF graph.

#<cldoc:cainteoir::rdf::query::subject>

Extract the subject of the RDF triple.

#<cldoc:cainteoir::rdf::query::predicate>

Extract the predicate of the RDF triple.

#<cldoc:cainteoir::rdf::query::object>

Extract the object of the RDF triple.

#<cldoc:cainteoir::rdf::query::literal>

Extract the object literal of the RDF triple.
@statement The RDF triple.

@return The object literal. If the object is a URI resource, an empty literal is returned.

#<cldoc:cainteoir::rdf::query::value>

Extract the object literal's value of the RDF triple.
@statement The RDF triple.

@return The object literal's value. If the object is a URI resource, an empty string is returned.

#<cldoc:cainteoir::rdf::query::results>

The set of RDF triples matching a select query.

#<cldoc:cainteoir::rdf::triplestore>

Hold a set of RDF triples in an RDF graph.

#<cldoc:cainteoir::rdf::triplestore::push_back>

Add an RDF triple to the triplestore.
@item The RDF triple to add to the triplestore.

#<cldoc:cainteoir::rdf::triplestore::subject>

Locate the RDF triples for the specified subject.
@s The subject of the RDF triples to select.

This method provides an optimized path for `select(subject, _, _)` queries to
avoid O(N^2) performance when those queries are used in a loop over a set of
subjects.

@return The RDF triples matching the subject.

#<cldoc:cainteoir::rdf::graph>

Manages an RDF graph.

#<cldoc:cainteoir::rdf::graph::size_type>

The type used to hold size values.

#<cldoc:cainteoir::rdf::graph::const_iterator>

An iterator over RDF triples.

#<cldoc:cainteoir::rdf::graph::const_reference>

A reference to an RDF triple.

#<cldoc:cainteoir::rdf::graph::graph>

Create a new RDF graph object.

#<cldoc:cainteoir::rdf::graph::graph>

Create a new RDF graph object suitable for merging with the parent graph.
@aGraph The graph to share genids with.

The output of the genid function will be unique across this graph and `aGraph`,
allowing the statements of the two graphs to be merged.

This does not copy the statements of `aGraph` into the new graph.

#<cldoc:cainteoir::rdf::graph::size>

Get the number of triples in the graph.

@return The number of triples in the graph.

#<cldoc:cainteoir::rdf::graph::empty>

Is the graph empty?

@return `true` if the graph does not contain any triple statements, `false` otherwise.

#<cldoc:cainteoir::rdf::graph::begin>

Get the iterator to the first triple in the graph's triplestore.

@return The iterator to the first triple in the graph's triplestore.

#<cldoc:cainteoir::rdf::graph::end>

Get the iterator to the last triple in the graph's triplestore.

@return The iterator to the last triple in the graph's triplestore.

#<cldoc:cainteoir::rdf::graph::front>

Get the first triple in the graph's triplestore.

@return The first triple in the graph's triplestore.

#<cldoc:cainteoir::rdf::graph::back>

Get the last triple in the graph's triplestore.

@return The last triple in the graph's triplestore.

#<cldoc:cainteoir::rdf::graph::subject>

Locate the RDF triples for the specified subject.
@s The subject of the RDF triples to select.

This method provides an optimized path for `select(subject, _, _)` queries to
avoid O(N^2) performance when those queries are used in a loop over a set of
subjects.

@return The RDF triples matching the subject.

#<cldoc:cainteoir::rdf::graph::contains>

Does the graph contain a URI in the specified namespace?
@uri The namespace to check for.

@return `true` if the graph contains a URI in the specified namespace, `false` otherwise.

#<cldoc:cainteoir::rdf::graph::set_base>

Set the base URI for the graph.
@aBase The base URI of the graph.

@return This graph object (to support method chaining).

#<cldoc:cainteoir::rdf::graph::get_base>

Get the base URI for the graph.

@return The base URI of the graph.

#<cldoc:cainteoir::rdf::graph::add_namespace>

Add a namespace to the graph.
@aPrefix The name of the namespace URI.
@aHref   The namespace URI.

This is used for generating `@prefix` entries in turtle/n3 documents.

@return This graph object (to support method chaining).

#<cldoc:cainteoir::rdf::graph::add_namespace>

Add a namespace to the graph.
@ns The namespace URI.

This is used for generating `@prefix` entries in turtle/n3 documents.

@return This graph object (to support method chaining).

#<cldoc:cainteoir::rdf::graph::operator<<>

Add a namespace to the graph.
@ns The namespace URI.

This is used for generating `@prefix` entries in turtle/n3 documents.

@return This graph to support method chaining.

#<cldoc:cainteoir::rdf::graph::add_prefix>

Add namespaces in an RDFa `@prefix` attribute.
@aPrefix The RDFa prefix string containing the namespaces to add.

This is used for generating `@prefix` entries in turtle/n3 documents.

@return This graph object (to support method chaining).

#<cldoc:cainteoir::rdf::href>

Convert a href into a URI object.
@aHref The href to convert.

@return The href as a URI object.

#<cldoc:cainteoir::rdf::graph::genid>

Generate a new blank node.

@return A blank node that is unique for this graph.

#<cldoc:cainteoir::rdf::graph::curie>

Convert a CURIE into a URI object.
@aCurie The CURIE to convert.

@return The CURIE as a URI object.

#<cldoc:cainteoir::rdf::graph::curie_list>

Convert a space-separated CURIE-list into a sequence of URI objects.
@aCurieList The space-separated CURIE-list to convert.
@onuri      The callback to be called when a URI is parsed from the CURIE list.

#<cldoc:cainteoir::rdf::query::rdf_list_items>

Iterate over the items of an RDF list.
@aMetadata  The RDF graph containing the list metadata.
@aSubject   The subject of the RDF list to iterate over.
@aPredicate The predicate of the RDF list to iterate over.
@onlistitem The callback function to call on each item in the RDF list.

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

#<cldoc:cainteoir::rdf::graph::statement>

Add an RDF statement to the graph.
@aSubject   The subject of the statement.
@aPredicate The predicate of the statement.
@aObject    The object of the statement.

This enforces the RDF requirement that a predicate cannot be a blank node.

@return `true` if the statement was added to the graph, `false` otherwise.

#<cldoc:cainteoir::rdf::graph::statement>

Add an RDF statement to the graph.
@aSubject   The subject of the statement.
@aPredicate The predicate of the statement.
@aObject    The object of the statement.

This enforces the RDF requirement that a predicate cannot be a blank node.

@return `true` if the statement was added to the graph, `false` otherwise.

#<cldoc:cainteoir::rdf::query::matches>

Match statements whos selector matches the value.
@aSelector The selector to extract data from the statement.
@aValue    The value to match against.

@return A function object for matching triples where `aSelector` returns `aValue`.

#<cldoc:cainteoir::rdf::query::both>

Match statements that match both selectors.
@a The first selector to match a statement against.
@b The second selector to match a statement against.

@return A function object for matching triples that match both `a` and `b` selectors.

#<cldoc:cainteoir::rdf::query::select>

Select statements matching the selector.
@metadata The subgraph to select statements from.
@selector The selector used to choose statements in the graph.

@return A subgraph containing all matching statements.

#<cldoc:cainteoir::rdf::query::contains>

Check if the graph contains any of the specified statements.
@metadata The subgraph to check.
@selector The selector used to choose statements in the graph.

@return `true` if the graph contains a statement matching the selector, `false` otherwise.

#<cldoc:cainteoir::rdf::query::select_value>

Select a value matching the selector.
@metadata The subgraph to select statements from.
@selector The selector used to choose statements in the graph.

@return The first literal value matching the selector.

#<cldoc:cainteoir::rdf::create_formatter>

Create an RDF serializer.
@aStream     The output stream to serialize the RDF data to.
@aFormatType The format to serialize the RDF data as.

@return The formatter object.

#<cldoc:cainteoir::rdf::formatter>

An interface for serializing RDF graphs.

#<cldoc:cainteoir::rdf::formatter::~formatter>

Clean up the formatter object.

#<cldoc:cainteoir::rdf::formatter::operator<<>

Serialize a namespace.
@aNS The namespace to serialize.

@return The formatter object (to support method chaining).

#<cldoc:cainteoir::rdf::formatter::operator<<>

Serialize a URI resource.
@uri The URI resource to serialize.

@return The formatter object (to support method chaining).

#<cldoc:cainteoir::rdf::formatter::operator<<>

Serialize a literal resource.
@literal The literal resource to serialize.

@return The formatter object (to support method chaining).

#<cldoc:cainteoir::rdf::formatter::operator<<>

Serialize an RDF statement.
@statement The statement to serialize.

@return The formatter object (to support method chaining).

#<cldoc:cainteoir::rdf::formatter::operator<<>

Serialize an RDF graph.
@aGraph The RDF graph to serialize.

@return The formatter object (to support method chaining).

#<cldoc:cainteoir::rdf::formatter::format_type>

The format used to serialize the RDF statements as.

#<cldoc:cainteoir::rdf::formatter::ntriple>

N-Triple serialization format.

#<cldoc:cainteoir::rdf::formatter::turtle>

Turtle serialization format.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
