#<cldoc:cainteoir::xml::lookup_entity>

Find the predefined entity in the entity table.
@entities The entity table to use to resolve the entity.
@data     The name of the entity without the surruonding & and ; characters.

@return The UTF-8 encoded characters represented by the named entity, or null if it does not exist.

#<cldoc:cainteoir::xml::xmlns::dc>

Dublin Core Elements namespace.

#<cldoc:cainteoir::xml::xmlns::dcam>

Dublin Core Abstract Model namespace.

#<cldoc:cainteoir::xml::xmlns::dct>

Dublin Core Terms namespace.

#<cldoc:cainteoir::xml::xmlns::dtb>

Daisy Talking Book namespace.

#<cldoc:cainteoir::xml::xmlns::epub>

ePub (Open Package Specification) namespace.

#<cldoc:cainteoir::xml::xmlns::foaf>

Friend of a Friend namespace.

#<cldoc:cainteoir::xml::xmlns::html40>

XML-based Hyper-Text Markup Language 4.0 (MS Word HTML) namespace.

#<cldoc:cainteoir::xml::xmlns::media>

ePub Media Overlays namespace.

#<cldoc:cainteoir::xml::xmlns::ncx>

Navigation Control File namespace.

#<cldoc:cainteoir::xml::xmlns::ocf>

Open Container Format namespace.

#<cldoc:cainteoir::xml::xmlns::opf>

Open Packaging Format namespace.

#<cldoc:cainteoir::xml::xmlns::owl>

Ontology Web Language namespace.

#<cldoc:cainteoir::xml::xmlns::pkg>

ePub Package namespace.

#<cldoc:cainteoir::xml::xmlns::rdf>

Resource Description Framework namespace.

#<cldoc:cainteoir::xml::xmlns::rdfa>

RDF/attributes namespace.

#<cldoc:cainteoir::xml::xmlns::rdfs>

RDF Schema namespace.

#<cldoc:cainteoir::xml::xmlns::skos>

Simple Knowledge Organization System namespace.

#<cldoc:cainteoir::xml::xmlns::smil>

Synchronized Multimedia Integration Language namespace.

#<cldoc:cainteoir::xml::xmlns::ssml>

Speech Synthesis Markup Language namespace.

#<cldoc:cainteoir::xml::xmlns::tts>

Cainteoir Text-to-Speech namespace.

#<cldoc:cainteoir::xml::xmlns::xhtml>

XML-based Hyper-Text Markup Language namespace.

#<cldoc:cainteoir::xml::xmlns::xml>

eXtensible Markup Language namespace.

#<cldoc:cainteoir::xml::xmlns::xsd>

XML Schema namespace.

#<cldoc:cainteoir::xml::namespaces>

Manage a set of XML namespaces.

#<cldoc:cainteoir::xml::namespaces::namespaces>

Initialize a new namespace manager object.

#<cldoc:cainteoir::xml::namespaces::add_namespace>

Add a namespace to the current scope.
@aPrefix The prefix of the namespace.
@aHref   The URI of the namespace.

@return This namespace manager object (to support method chaining).

#<cldoc:cainteoir::xml::namespaces::add_namespace>

Add a namespace to the current scope.
@aPrefix The prefix of the namespace.
@aHref   The URI of the namespace.

@return This namespace manager object (to support method chaining).

#<cldoc:cainteoir::xml::namespaces::add_namespace>

Add a namespace to the current scope.
@aNS The namespace to add.

@return This namespace manager object (to support method chaining).

#<cldoc:cainteoir::xml::namespaces::push_block>

Add a new scope block.

#<cldoc:cainteoir::xml::namespaces::pop_block>

Remove a new scope block.

All namespaces associated with the top-most scope block are removed.

#<cldoc:cainteoir::xml::namespaces::lookup>

Resolve the namespace prefix to a URI.
@aPrefix The namespace prefix to resolve.

@return The URI associated with the specified namespace prefix.

#<cldoc:cainteoir::xml::namespaces::lookup>

Resolve the namespace prefix to a URI.
@aPrefix The namespace prefix to resolve.

@return The URI associated with the specified namespace prefix.

#<cldoc:cainteoir::xml::unknown_context>

The specified element/attribute was not found.

#<cldoc:cainteoir::xml::base_attr>

The `@xml:base` attribute.

#<cldoc:cainteoir::xml::id_attr>

The `@xml:id` attribute.

#<cldoc:cainteoir::xml::lang_attr>

The `@xml:lang` attribute.

#<cldoc:cainteoir::xml::space_attr>

The `@xml:space` attribute.

#<cldoc:cainteoir::xml::attrs>

Attributes in the XML namespace.

#<cldoc:cainteoir::xml::context>

Manage looking up XML elements and attributes.

#<cldoc:cainteoir::xml::context::context>

Create an empty context manager object.

#<cldoc:cainteoir::xml::context::context>

Create a context manager object.
@aNS     The namespace to associate the entries with.
@entries The elements or attributes associated with the namespace.
@match   The comparison method to use (case sensitive or insensitive).

#<cldoc:cainteoir::xml::context::context>

Create a context manager object.
@aNS     The namespace to associate the entries with.
@entries The elements or attributes associated with the namespace.
@match   The comparison method to use (case sensitive or insensitive).

#<cldoc:cainteoir::xml::context::set>

Set the context entries to use for the specified namespace.
@aNS     The namespace to associate the entries with.
@entries The elements or attributes associated with the namespace.
@match   The comparison method to use (case sensitive or insensitive).

#<cldoc:cainteoir::xml::context::set>

Set the context entries to use for the specified namespace.
@aNS     The namespace to associate the entries with.
@entries The elements or attributes associated with the namespace.
@match   The comparison method to use (case sensitive or insensitive).

#<cldoc:cainteoir::xml::context::lookup>

Find the entry for the specified element or attribute.
@aNS   The namespace of the element/attribute.
@aNode The element/attribute name.

@return The entry associated with the element/attribute.

#<cldoc:cainteoir::xml::reader>

Parse an XML document.

#<cldoc:cainteoir::xml::reader::reader>

Create a new XML document reader.
@aData               The XML document contents.
@aDefaultEncoding    The character encoding to use by default.
@aPredefinedEntities The DTD entities to use by default.

#<cldoc:cainteoir::xml::reader::set_predefined_entities>

Set the DTD entities to use by default.
@aPredefinedEntities The DTD entities to use by default.

#<cldoc:cainteoir::xml::reader::read>

Read the next node in the XML document.

@return `true` if the next node was read, `false` if there are no more nodes to read.

#<cldoc:cainteoir::xml::reader::nodeValue>

Get the content of the current node.

@return The content of the current node.

#<cldoc:cainteoir::xml::reader::nodeName>

Get the name of the current node.

@return The name of the current node.

#<cldoc:cainteoir::xml::reader::nodePrefix>

Get the namespace prefix of the current node.

@return The namespace prefix of the current node.

#<cldoc:cainteoir::xml::reader::namespaceUri>

Get the namespace URI of the current node.

@return The namespace URI of the current node.

#<cldoc:cainteoir::xml::reader::nodeType>

Get the type of the current node.

@return The type of the current node.

#<cldoc:cainteoir::xml::reader::isPlainText>

Is the document plain text?

@return `true` if the document is plain text, `false` otherwise.

#<cldoc:cainteoir::xml::reader::context>

Get the context entry for the current node.

@return The context entry for the current node.

#<cldoc:cainteoir::xml::reader::set_nodes>

Set the context entries for resolving the namespace's elements.
@aNS     The namespace to associate the entries with.
@entries The elements associated with the namespace.
@match   The comparison method to use (case sensitive or insensitive).

#<cldoc:cainteoir::xml::reader::set_nodes>

Set the context entries for resolving the namespace's elements.
@aNS     The namespace to associate the entries with.
@entries The elements associated with the namespace.
@match   The comparison method to use (case sensitive or insensitive).

#<cldoc:cainteoir::xml::reader::set_attrs>

Set the context entries for resolving the namespace's attributes.
@aNS     The namespace to associate the entries with.
@entries The attributes associated with the namespace.
@match   The comparison method to use (case sensitive or insensitive).

#<cldoc:cainteoir::xml::reader::set_attrs>

Set the context entries for resolving the namespace's attributes.
@aNS     The namespace to associate the entries with.
@entries The attributes associated with the namespace.
@match   The comparison method to use (case sensitive or insensitive).

#<cldoc:cainteoir::xml::reader::set_encoding>

Change the character encoding used by the parser.
@encoding The character encoding to change to.

@return `true` if the character encoding was changed, `false` otherwise.

#<cldoc:cainteoir::xml::reader::current>

Get the current position in the XML document.

@return The current position in the XML document.

#<cldoc:cainteoir::xml::reader::set_begin_tag_type>

Set the way begin tags are interpreted.
@aType The way begin tags are to be interpreted.

#<cldoc:cainteoir::xml::ns>

Manages an XML namespace.

#<cldoc:cainteoir::xml::ns::prefix>

The default prefix for the namespace.

#<cldoc:cainteoir::xml::ns::href>

The URI of the namespace.

#<cldoc:cainteoir::xml::ns::ns>

Create a new XML namespace object.
@aPrefix The prefix to use for the namespace.
@aHref   The URI of the namespace.

#<cldoc:cainteoir::xml::xml_entities>

XML 1.0 entities.

#<cldoc:cainteoir::xml::html_entities>

HTML 5.0 entities.

#<cldoc:cainteoir::xml::context::entry>

Identifies an XML element or attribute.

#<cldoc:cainteoir::xml::context::entry::begin_tag_type>

Denotes whether the element is self-closing or not.

#<cldoc:cainteoir::xml::context::entry::styles>

The CSS styles associated with the element.

#<cldoc:cainteoir::xml::context::entry_ref>

Associates an element/attribute name with its entry object.

#<cldoc:cainteoir::xml::context::entry_ref::name>

The name of the element/attribute.

#<cldoc:cainteoir::xml::context::entry_ref::data>

The entry data associated with the element/attribute.

#<cldoc:cainteoir::xml::reader::node_type>

Specifies the type of the XML node.

#<cldoc:cainteoir::xml::reader::beginTagNode>

An start of an element.

#<cldoc:cainteoir::xml::reader::endTagNode>

An end of an element.

#<cldoc:cainteoir::xml::reader::beginProcessingInstructionNode>

The start of a processing instruction.

#<cldoc:cainteoir::xml::reader::endProcessingInstructionNode>

The end of a processing instruction.

#<cldoc:cainteoir::xml::reader::commentNode>

A comment.

#<cldoc:cainteoir::xml::reader::cdataNode>

A CDATA section.

#<cldoc:cainteoir::xml::reader::textNode>

A block of text.

#<cldoc:cainteoir::xml::reader::error>

An error occurred parsing the XML; the XML may be malformed.

#<cldoc:cainteoir::xml::reader::doctypeNode>

A DOCTYPE.

#<cldoc:cainteoir::xml::reader::attribute>

An attribute on an element or processing instruction.

#<cldoc:cainteoir::xml::reader::endOfData>

The document has no more XML data.

#<cldoc:cainteoir::xml::reader::dtdEntity>

An ENTITY reference declaration.

#<cldoc:cainteoir::xml::begin_tag_type>

Specifies how begin tags are interpreted.

#<cldoc:cainteoir::xml::begin_tag_type::open>

The begin tag opens an element block.

#<cldoc:cainteoir::xml::begin_tag_type::open_close>

The begin tag creates a self-contained element block.

This makes `<node>` behave the same way as `<node/>`, allowing
support for void elements as per HTML§12.1.2.

#<cldoc:cainteoir::createXmlReader>

Create a new XML document reader.
@aData            The XML document contents.
@aDefaultEncoding The character encoding to use by default.

This function moves the reader to the first open tag element, so an XML document
consumer does not have to do this itself.

#<cldoc:cainteoir::xml::node_type_name>

Get the display name for the node type.
@aType The XML node type.

@return The display name for the node type.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
