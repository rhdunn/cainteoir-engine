# cainteoir::events::event_type
{: .doc }

The type of the document event.

# cainteoir::events::begin_context
{: .doc }

The start of a rendering context.

	context <context> <parameter>

# cainteoir::events::end_context
{: .doc }

The end of a rendering context.

	end

# cainteoir::events::text
{: .doc }

Text data.

	text <content>

# cainteoir::events::anchor
{: .doc }

An anchor point in the document.

The anchor corresponds to the associated toc-entry.

	anchor <anchor>

# cainteoir::events::text_ref
{: .doc }

A reference to text defined elsewhere.

The anchor refers to a text block defined elsewhere in the document.

This is used in conjunction with [media_ref](^^cainteoir::events::media_ref)
to associate narrated audio with a block of text.

	text-ref <anchor>

# cainteoir::events::media_ref
{: .doc }

A media (audio or video) file.

The anchor refers to the media file being referenced. The point to start playback
at is media-begin and the point to finish playback is media-end. If the media
file has been read, content contains the media file data, otherwise content is
empty.

	media-ref <anchor> <media-begin> <media-end> <content>

# cainteoir::document_item
{: .doc }

Holds information about a part of a document.

Objects of this type are created by instances of the
[document_reader](^^cainteoir::document_reader) interface.

# cainteoir::document_item::document_item
{: .doc }

Initialize the document item object.

# cainteoir::document_item::type
{: .doc }

The type of the event.

# cainteoir::document_item::styles
{: .doc }

The CSS styles associated with this document item.

# cainteoir::document_item::content
{: .doc }

The text or audio data associated with the document event.

# cainteoir::document_item::anchor
{: .doc }

A uri that references the start of this event.

# cainteoir::document_item::media_begin
{: .doc }

The time to start playback of the media in the content buffer.

If this is [css::time::inherit](^^cainteoir::css::time::inherit), the playback
is from the start of the media object.

# cainteoir::document_item::media_end
{: .doc }

The time to end playback of the media in the content buffer.

If this is [css::time::inherit](^^cainteoir::css::time::inherit), the playback
is to the end of the media object.

# cainteoir::ref_entry
{: .doc }

Represents a [ref:Entry](http://reecedunn.co.uk/schema/2014/ref#Entry) item.

# cainteoir::ref_entry::depth
{: .doc }

The level of the entry.

# cainteoir::ref_entry::location
{: .doc }

The location of the entry in the document.

# cainteoir::ref_entry::title
{: .doc }

The title of the entry.

# cainteoir::ref_entry::ref_entry
{: .doc }

Create an entry object from [ref:Entry](http://reecedunn.co.uk/schema/2014/ref#Entry) metadata.

@aEntry
: The RDF metadata for the [ref:Entry](http://reecedunn.co.uk/schema/2014/ref#Entry).

# cainteoir::document
{: .doc }

Stores the [document_item](^^cainteoir::document_item) events from a document.

# cainteoir::document::list_type
{: .doc }

The type used to store the document items.

# cainteoir::document::const_iterator
{: .doc }

The type used to iterate over document items.

# cainteoir::document::range_type
{: .doc }

A range of [document_item](^^cainteoir::document_item) objects.

# cainteoir::document::document
{: .doc }

Create a document object with the [document_item](^^cainteoir::document_item)
events from the [document_reader](^^cainteoir::document_reader).

@aReader
: The document reader to read [document_item](^^cainteoir::document_item)
  events from.

@aMetadata
: The metadata associated with the document.

Any metadata contained within the document that is captured as the document is
read will be added to `aMetadata`.

# cainteoir::document::text_length
{: .doc }

Get the number of characters in the document.

@return The number of characters in the document.

# cainteoir::document::children
{: .doc }

Get the [document_item](^^cainteoir::document_item) objects associated with the
document in the specified range.

@aAnchors
: The start and end anchor for the range of children to get.

@return
: A range of [document_item](^^cainteoir::document_item) objects.

# cainteoir::document::children
{: .doc }

Get the [document_item](^^cainteoir::document_item) objects associated with the
document in the specified range.

@aListing
: The navigation listing to resolve the aRange indices with.

@aRange
: The start and end indices in `aListing` for the range of children to get.

@return
: A range of [document_item](^^cainteoir::document_item) objects.

# cainteoir::document::children
{: .doc }

Get all the [document_item](^^cainteoir::document_item) objects associated with
the document.

@return
: A range of [document_item](^^cainteoir::document_item) objects.

# cainteoir::navigation
{: .doc }

Get the navigation reference entries (e.g. table of contents) for the document.

@aMetadata
: The RDF metadata containing the navigation information collected from the document.

@aSubject
: The RDF subject of the document to get the navigation information for.

@aListing
: The listing type to query for.

@return
: The navigation entries associated with the document.

# cainteoir::document::indexof
{: .doc }

Get the index of the [document_item](^^cainteoir::document_item) referenced by
the anchor.

@aAnchor
: The anchor to resolve within the document.

@return
: The index of the [document_item](^^cainteoir::document_item) referenced by
  the anchor.

# cainteoir::document_reader
{: .doc }

Provides a reader-style API to the events corresponding to the document.

# cainteoir::document_reader::~document_reader
{: .doc }

Destroy the document reader object.

# cainteoir::document_reader::read
{: .doc }

Read the next event in the document.

@aMetadata
: Any additional metadata.

If `aMetadata` is not null, any metadata gathered while reading the next event
is added to the metadata graph. This allows metadata in the body of the
document to be extracted, such as RDFa or document reference metadata.

@return
: `true` if an event was read, `false` if there are no more events in the document.

# cainteoir::capability_types
{: .doc }

The capabilities provided by different document types.

# cainteoir::metadata_support
{: .doc }

The document type provides metadata information that can be extracted.

# cainteoir::text_support
{: .doc }

The document type contains text that can be extracted.

# cainteoir::supportedDocumentFormats
{: .doc }

Get the document formats that are supported by libcainteoir.

@metadata
: The RDF graph to write the format support to.

@capabilities
: The document capabilities to query for.

# cainteoir::createDocumentReader
{: .doc }

Create a document content reader.

@aFilename
: The path to the document.

@aPrimaryMetadata
: The main metadata that describes the document.

@aTitle
: The document title to use if none is specified.

@aDefaultEncoding
: The default character encoding to use.

If `aFilename` is null, the file content is read from stdin.

The top-level ToC entry is determined as follows (in order of preference):
*  the title specified by the document;
*  the title specified in aTitle;
*  the filename of the document.

After the call to this method, `aPrimaryMetadata` contains the metadata located
in the header section of the document. That is the part of the document before
the start of any document text.

@return
: A reader over the document contents, or a null pointer if the document
  is not supported.

# cainteoir::createDocumentReader
{: .doc }

Create a document content reader.

@aData
: The document content.

@aSubject
: The RDF subject for the document metadata.

@aPrimaryMetadata
: The main metadata that describes the document.

@aTitle
: The document title to use if none is specified.

@aDefaultEncoding
: The default character encoding to use.

The top-level ToC entry is determined as follows (in order of preference):
*  the title specified by the document;
*  the title specified in aTitle;
*  the filename of the document.

After the call to this method, `aPrimaryMetadata` contains the metadata located
in the header section of the document. That is the part of the document before
the start of any document text.

@return
: A reader over the document contents, or a null pointer if the document
  is not supported.

# cainteoir::createDocumentReader
{: .doc }

Create a document content reader.

@aDocumentRange
: The range of document items to read.

This is used to allow documents that have been parsed into a
[document](^^cainteoir::document) object to be passed to API that consume a
[document_reader](^^cainteoir::document_reader). It also allows a subset of
the document to be used (e.g. between two table of content entries).

@return
: A reader over the document item range.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
