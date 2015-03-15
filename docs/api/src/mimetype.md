# cainteoir::mime::mimetype
{: .doc }

Manage a document MIME type with file content detection.

# cainteoir::mime::mimetype::name
{: .doc }

The name of this mimetype/content.

# cainteoir::mime::mimetype::mime_type
{: .doc }

The primary mimetype string.

# cainteoir::mime::mimetype::mimetype
{: .doc }

Create a MIME type manager object.

@aName
: The document type name.

@aMimeType
: The primary MIME type string.

@aInfo
: The internal MIME type detection information.

# cainteoir::mime::mimetype::match
{: .doc }

Does the document content match this MIME type?

@data
: The document content to check.

@return
: `true` if the content matches this MIME type, `false` otherwise.

# cainteoir::mime::mimetype::match
{: .doc }

Does the XML namespace match this MIME type?

@uri
: The URI of the XML namespace.

@name
: The localname of the root XML element.

@return
: `true` if the XML namespace matches this MIME type, `false` otherwise.

# cainteoir::mime::mimetype::metadata
{: .doc }

Get the RDF metadata for this MIME type.

@aGraph
: The RDF graph to write the metadata to.

@baseuri
: The base URI to use for the RDF subject associated with this MIME type.

@type
: The rdf:type of the RDF subject associated with this MIME type.

# cainteoir::mime::email
{: .doc }

Email mbox (mime) document.

# cainteoir::mime::epub
{: .doc }

Electronic publication (ePub) document.

# cainteoir::mime::gzip
{: .doc }

GZIP compressed document.

# cainteoir::mime::html
{: .doc }

HTML document.

# cainteoir::mime::mhtml
{: .doc }

Single-page HTML document.

# cainteoir::mime::mime
{: .doc }

MIME/HTTP/NEWS headers.

# cainteoir::mime::ncx
{: .doc }

Navigation control document.

# cainteoir::mime::ogg
{: .doc }

Ogg/Vorbis audio.

# cainteoir::mime::opf
{: .doc }

Open package format document.

# cainteoir::mime::pdf
{: .doc }

Portable document format.

# cainteoir::mime::rdfxml
{: .doc }

RDF/XML document.

# cainteoir::mime::rtf
{: .doc }

Rich text document.

# cainteoir::mime::smil
{: .doc }

SMIL document.

# cainteoir::mime::ssml
{: .doc }

Speech synthesis markup document.

# cainteoir::mime::text
{: .doc }

Plain text document.

# cainteoir::mime::wav
{: .doc }

Wave audio.

# cainteoir::mime::xhtml
{: .doc }

XHTML document.

# cainteoir::mime::xml
{: .doc }

Extensible markup language document.

# cainteoir::mime::zip
{: .doc }

ZIP (compressed) archive.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
