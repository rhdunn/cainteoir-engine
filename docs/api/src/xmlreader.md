# cainteoir::xml::lookup_entity
{: .doc }

Find the predefined entity in the entity table.

@entities
: The entity table to use to resolve the entity.

@data
: The name of the entity without the surruonding & and ; characters.

@return
: The UTF-8 encoded characters represented by the named entity, or null if it does not exist.

# cainteoir::xml::xmlns::dc
{: .doc }

Dublin Core Elements namespace.

# cainteoir::xml::xmlns::dcam
{: .doc }

Dublin Core Abstract Model namespace.

# cainteoir::xml::xmlns::dct
{: .doc }

Dublin Core Terms namespace.

# cainteoir::xml::xmlns::dtb
{: .doc }

Daisy Talking Book namespace.

# cainteoir::xml::xmlns::epub
{: .doc }

ePub (Open Package Specification) namespace.

# cainteoir::xml::xmlns::foaf
{: .doc }

Friend of a Friend namespace.

# cainteoir::xml::xmlns::html40
{: .doc }

XML-based Hyper-Text Markup Language 4.0 (MS Word HTML) namespace.

# cainteoir::xml::xmlns::media
{: .doc }

ePub Media Overlays namespace.

# cainteoir::xml::xmlns::ncx
{: .doc }

Navigation Control File namespace.

# cainteoir::xml::xmlns::ocf
{: .doc }

Open Container Format namespace.

# cainteoir::xml::xmlns::opf
{: .doc }

Open Packaging Format namespace.

# cainteoir::xml::xmlns::owl
{: .doc }

Ontology Web Language namespace.

# cainteoir::xml::xmlns::pkg
{: .doc }

ePub Package namespace.

# cainteoir::xml::xmlns::rdf
{: .doc }

Resource Description Framework namespace.

# cainteoir::xml::xmlns::rdfa
{: .doc }

RDF/attributes namespace.

# cainteoir::xml::xmlns::rdfs
{: .doc }

RDF Schema namespace.

# cainteoir::xml::xmlns::skos
{: .doc }

Simple Knowledge Organization System namespace.

# cainteoir::xml::xmlns::smil
{: .doc }

Synchronized Multimedia Integration Language namespace.

# cainteoir::xml::xmlns::ssml
{: .doc }

Speech Synthesis Markup Language namespace.

# cainteoir::xml::xmlns::tts
{: .doc }

Cainteoir Text-to-Speech namespace.

# cainteoir::xml::xmlns::xhtml
{: .doc }

XML-based Hyper-Text Markup Language namespace.

# cainteoir::xml::xmlns::xml
{: .doc }

eXtensible Markup Language namespace.

# cainteoir::xml::xmlns::xsd
{: .doc }

XML Schema namespace.

# cainteoir::xml::namespaces
{: .doc }

Manage a set of XML namespaces.

# cainteoir::xml::namespaces::namespaces
{: .doc }

Initialize a new namespace manager object.

# cainteoir::xml::namespaces::add_namespace
{: .doc }

Add a namespace to the current scope.

@aPrefix
: The prefix of the namespace.

@aHref
: The URI of the namespace.

@return
: This namespace manager object (to support method chaining).

# cainteoir::xml::namespaces::add_namespace
{: .doc }

Add a namespace to the current scope.

@aPrefix
: The prefix of the namespace.

@aHref
: The URI of the namespace.

@return
: This namespace manager object (to support method chaining).

# cainteoir::xml::namespaces::add_namespace
{: .doc }

Add a namespace to the current scope.

@aNS
: The namespace to add.

@return
: This namespace manager object (to support method chaining).

# cainteoir::xml::namespaces::push_block
{: .doc }

Add a new scope block.

# cainteoir::xml::namespaces::pop_block
{: .doc }

Remove a new scope block.

All namespaces associated with the top-most scope block are removed.

# cainteoir::xml::namespaces::lookup
{: .doc }

Resolve the namespace prefix to a URI.

@aPrefix
: The namespace prefix to resolve.

@return
: The URI associated with the specified namespace prefix.

# cainteoir::xml::namespaces::lookup
{: .doc }

Resolve the namespace prefix to a URI.

@aPrefix
: The namespace prefix to resolve.

@return
: The URI associated with the specified namespace prefix.

# cainteoir::xml::unknown_context
{: .doc }

The specified element/attribute was not found.

# cainteoir::xml::base_attr
{: .doc }

The `@xml:base` attribute.

# cainteoir::xml::id_attr
{: .doc }

The `@xml:id` attribute.

# cainteoir::xml::lang_attr
{: .doc }

The `@xml:lang` attribute.

# cainteoir::xml::space_attr
{: .doc }

The `@xml:space` attribute.

# cainteoir::xml::attrs
{: .doc }

Attributes in the XML namespace.

# cainteoir::xml::context
{: .doc }

Manage looking up XML elements and attributes.

# cainteoir::xml::context::context
{: .doc }

Create an empty context manager object.

# cainteoir::xml::context::context
{: .doc }

Create a context manager object.

@aNS
: The namespace to associate the entries with.

@entries
: The elements or attributes associated with the namespace.

@match
: The comparison method to use (case sensitive or insensitive).

# cainteoir::xml::context::context
{: .doc }

Create a context manager object.

@aNS
: The namespace to associate the entries with.

@entries
: The elements or attributes associated with the namespace.

@match
: The comparison method to use (case sensitive or insensitive).

# cainteoir::xml::context::set
{: .doc }

Set the context entries to use for the specified namespace.

@aNS
: The namespace to associate the entries with.

@entries
: The elements or attributes associated with the namespace.

@match
: The comparison method to use (case sensitive or insensitive).

# cainteoir::xml::context::set
{: .doc }

Set the context entries to use for the specified namespace.

@aNS
: The namespace to associate the entries with.

@entries
: The elements or attributes associated with the namespace.

@match
: The comparison method to use (case sensitive or insensitive).

# cainteoir::xml::context::lookup
{: .doc }

Find the entry for the specified element or attribute.

@aNS
: The namespace of the element/attribute.

@aNode
: The element/attribute name.

@return
: The entry associated with the element/attribute.

# cainteoir::xml::reader
{: .doc }

Parse an XML document.

# cainteoir::xml::reader::reader
{: .doc }

Create a new XML document reader.

@aData
: The XML document contents.

@aDefaultEncoding
: The character encoding to use by default.

@aPredefinedEntities
: The DTD entities to use by default.

# cainteoir::xml::reader::set_predefined_entities
{: .doc }

Set the DTD entities to use by default.

@aPredefinedEntities
: The DTD entities to use by default.

# cainteoir::xml::reader::read
{: .doc }

Read the next node in the XML document.

@return
: `true` if the next node was read, `false` if there are no more nodes to read.

# cainteoir::xml::reader::nodeValue
{: .doc }

Get the content of the current node.

@return
: The content of the current node.

# cainteoir::xml::reader::nodeName
{: .doc }

Get the name of the current node.

@return
: The name of the current node.

# cainteoir::xml::reader::nodePrefix
{: .doc }

Get the namespace prefix of the current node.

@return
: The namespace prefix of the current node.

# cainteoir::xml::reader::namespaceUri
{: .doc }

Get the namespace URI of the current node.

@return
: The namespace URI of the current node.

# cainteoir::xml::reader::nodeType
{: .doc }

Get the type of the current node.

@return
: The type of the current node.

# cainteoir::xml::reader::isPlainText
{: .doc }

Is the document plain text?

@return
: `true` if the document is plain text, `false` otherwise.

# cainteoir::xml::reader::context
{: .doc }

Get the context entry for the current node.

@return
: The context entry for the current node.

# cainteoir::xml::reader::set_nodes
{: .doc }

Set the context entries for resolving the namespace's elements.

@aNS
: The namespace to associate the entries with.

@entries
: The elements associated with the namespace.

@match
: The comparison method to use (case sensitive or insensitive).

# cainteoir::xml::reader::set_nodes
{: .doc }

Set the context entries for resolving the namespace's elements.

@aNS
: The namespace to associate the entries with.

@entries
: The elements associated with the namespace.

@match
: The comparison method to use (case sensitive or insensitive).

# cainteoir::xml::reader::set_attrs
{: .doc }

Set the context entries for resolving the namespace's attributes.

@aNS
: The namespace to associate the entries with.

@entries
: The attributes associated with the namespace.

@match
: The comparison method to use (case sensitive or insensitive).

# cainteoir::xml::reader::set_attrs
{: .doc }

Set the context entries for resolving the namespace's attributes.

@aNS
: The namespace to associate the entries with.

@entries
: The attributes associated with the namespace.

@match
: The comparison method to use (case sensitive or insensitive).

# cainteoir::xml::reader::set_encoding
{: .doc }

Change the character encoding used by the parser.

@encoding
: The character encoding to change to.

@return
: `true` if the character encoding was changed, `false` otherwise.

# cainteoir::xml::reader::current
{: .doc }

Get the current position in the XML document.

@return
: The current position in the XML document.

# cainteoir::xml::reader::set_begin_tag_type
{: .doc }

Set the way begin tags are interpreted.

@aType
: The way begin tags are to be interpreted.

# cainteoir::xml::ns
{: .doc }

Manages an XML namespace.

# cainteoir::xml::ns::prefix
{: .doc }

The default prefix for the namespace.

# cainteoir::xml::ns::href
{: .doc }

The URI of the namespace.

# cainteoir::xml::ns::ns
{: .doc }

Create a new XML namespace object.

@aPrefix
: The prefix to use for the namespace.

@aHref
: The URI of the namespace.

# cainteoir::xml::xml_entities
{: .doc }

XML 1.0 entities.

# cainteoir::xml::html_entities
{: .doc }

HTML 5.0 entities.

# cainteoir::xml::context::entry
{: .doc }

Identifies an XML element or attribute.

# cainteoir::xml::context::entry::begin_tag_type
{: .doc }

Denotes whether the element is self-closing or not.

# cainteoir::xml::context::entry::styles
{: .doc }

The CSS styles associated with the element.

# cainteoir::xml::context::entry_ref
{: .doc }

Associates an element/attribute name with its entry object.

# cainteoir::xml::context::entry_ref::name
{: .doc }

The name of the element/attribute.

# cainteoir::xml::context::entry_ref::data
{: .doc }

The entry data associated with the element/attribute.

# cainteoir::xml::reader::node_type
{: .doc }

Specifies the type of the XML node.

# cainteoir::xml::reader::beginTagNode
{: .doc }

An start of an element.

# cainteoir::xml::reader::endTagNode
{: .doc }

An end of an element.

# cainteoir::xml::reader::beginProcessingInstructionNode
{: .doc }

The start of a processing instruction.

# cainteoir::xml::reader::endProcessingInstructionNode
{: .doc }

The end of a processing instruction.

# cainteoir::xml::reader::commentNode
{: .doc }

A comment.

# cainteoir::xml::reader::cdataNode
{: .doc }

A CDATA section.

# cainteoir::xml::reader::textNode
{: .doc }

A block of text.

# cainteoir::xml::reader::error
{: .doc }

An error occurred parsing the XML; the XML may be malformed.

# cainteoir::xml::reader::doctypeNode
{: .doc }

A DOCTYPE.

# cainteoir::xml::reader::attribute
{: .doc }

An attribute on an element or processing instruction.

# cainteoir::xml::reader::endOfData
{: .doc }

The document has no more XML data.

# cainteoir::xml::reader::dtdEntity
{: .doc }

An ENTITY reference declaration.

# cainteoir::xml::begin_tag_type
{: .doc }

Specifies how begin tags are interpreted.

# cainteoir::xml::begin_tag_type::open
{: .doc }

The begin tag opens an element block.

# cainteoir::xml::begin_tag_type::open_close
{: .doc }

The begin tag creates a self-contained element block.

This makes `<node>` behave the same way as `<node/>`, allowing
support for void elements as per HTML§12.1.2.

# cainteoir::createXmlReader
{: .doc }

Create a new XML document reader.

@aData
: The XML document contents.

@aDefaultEncoding
: The character encoding to use by default.

This function moves the reader to the first open tag element, so an XML document
consumer does not have to do this itself.

# cainteoir::xml::node_type_name
{: .doc }

Get the display name for the node type.

@aType
: The XML node type.

@return
: The display name for the node type.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
