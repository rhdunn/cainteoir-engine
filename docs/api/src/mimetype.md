#<cldoc:cainteoir::mime::mimetype>

Manage a document MIME type with file content detection.

#<cldoc:cainteoir::mime::mimetype::name>

The name of this mimetype/content.

#<cldoc:cainteoir::mime::mimetype::mime_type>

The primary mimetype string.

#<cldoc:cainteoir::mime::mimetype::mimetype>

Create a MIME type manager object.
@aName     The document type name.
@aMimeType The primary MIME type string.
@aInfo     The internal MIME type detection information.

#<cldoc:cainteoir::mime::mimetype::match>

Does the document content match this MIME type?
@data The document content to check.

@return `true` if the content matches this MIME type, `false` otherwise.

#<cldoc:cainteoir::mime::mimetype::match>

Does the XML namespace match this MIME type?
@uri  The URI of the XML namespace.
@name The localname of the root XML element.

@return `true` if the XML namespace matches this MIME type, `false` otherwise.

#<cldoc:cainteoir::mime::mimetype::metadata>

Get the RDF metadata for this MIME type.
@aGraph  The RDF graph to write the metadata to.
@baseuri The base URI to use for the RDF subject associated with this MIME type.
@type    The rdf:type of the RDF subject associated with this MIME type.

#<cldoc:cainteoir::mime::email>

Email mbox (mime) document.

#<cldoc:cainteoir::mime::epub>

Electronic publication (ePub) document.

#<cldoc:cainteoir::mime::gzip>

GZIP compressed document.

#<cldoc:cainteoir::mime::html>

HTML document.

#<cldoc:cainteoir::mime::mhtml>

Single-page HTML document.

#<cldoc:cainteoir::mime::mime>

MIME/HTTP/NEWS headers.

#<cldoc:cainteoir::mime::ncx>

Navigation control document.

#<cldoc:cainteoir::mime::ogg>

Ogg/Vorbis audio.

#<cldoc:cainteoir::mime::opf>

Open package format document.

#<cldoc:cainteoir::mime::pdf>

Portable document format.

#<cldoc:cainteoir::mime::rdfxml>

RDF/XML document.

#<cldoc:cainteoir::mime::rtf>

Rich text document.

#<cldoc:cainteoir::mime::smil>

SMIL document.

#<cldoc:cainteoir::mime::ssml>

Speech synthesis markup document.

#<cldoc:cainteoir::mime::text>

Plain text document.

#<cldoc:cainteoir::mime::wav>

Wave audio.

#<cldoc:cainteoir::mime::xhtml>

XHTML document.

#<cldoc:cainteoir::mime::xml>

Extensible markup language document.

#<cldoc:cainteoir::mime::zip>

ZIP (compressed) archive.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
