Release Notes
=============

Version 0.8: Friday June 1st 2012 [Jun 2012]
--------------------------------------------

  *  define an archive interface with a zip file implementation
  *  report the filename as a toc entry if no toc is present
  *  move the UTF-8 helpers to a common location
  *  optimize decoding of utf-8 and us-ascii encodings
  *  do not load a new decoder when switching to the same encoding
  *  switch document parsing from an event model to a reader model

### applications:

  *  doc2doc: a simple command-line tool for converting documents from
     one format to another

### document formats:

  *  pdf:     support PDF documents using the Poppler library
  *  (x)html: support extracting metadata from several meta tag names
  *  zip:     support reading generic zipped document collections
  *  ePub:    make the ePub parser use the archive interface

Version 0.7: Friday March 30th 2012 [Mar 2012]
----------------------------------------------

  *  use BCP 47 for interpreting and comparing language tags
  *  translate the language and region names using the iso-codes package
  *  support for espeak using installed mbrola voices

### document formats:

  *  (x)html: report h1..h6 sections as table of content items
  *  email:   improve email content detection and support newsgroup information
  *  rdf/xml: support property attributes on an empty property element
  *  xml:     support entities in DTD blocks

### bug fixes:

  *  fix detection of html that is valid xml, but is not xhtml
  *  improve detection of control characters when identifying binary data

### cleanup/refactoring:

  *  simplified and cleaned up the RDF model and query APIs
  *  use std::shared_ptr consistently over std::tr1::shared_ptr
  *  use std::make_shared instead of allocating the object explicitly
  *  remove the libxml2 dependency and make all xml-based parsers use xmlreader

Version 0.6: Sunday January 29th 2012 [Jan 2012]
------------------------------------------------

  *  languages.rdf -- represent ISO-639-2/B, ISO-639-2/T, ISO-639-3, ISO-639-5,
     espeak and wikimedia language codes;
  *  add document format implementation status documentation.

### xmlreader xml parsing support:

  *  parse entities in attribues;
  *  support single-quote and html-style attributes;
  *  parse xml namespaces;
  *  unify xml and rdf namespace/uri support;
  *  support element/attribute mapping to context entry objects.

### document parsers:

  *  epub  -- support epub 3 @refines and @datatype metadata;
  *  html  -- support all known implicit close tags and recognise more elements;
  *  ocf   -- use the xmlreader api and expose a reader api;
  *  opf   -- use the xmlreader api.

### bug fixes:

  *  release the file handle when finished recording ogg files.

Version 0.5: Monday November 21st 2011 [Nov 2011]
-------------------------------------------------

  *  use the shared-mime-info database for MIME type detection.
  *  use iconv for character encoding conversions.
  *  support all Content-Transfer-Encoding types for MIME headers, including base64.
  *  improve audio error handling during reading/recording.
  *  support more language code mappings for new espeak voices.
  *  support UND as a language identifier (for Calibri eBooks).
  *  support XML encoded HTML without an associated xmlns.

Version 0.4: Tuesday July 26th 2011 [Jul 2011]
----------------------------------------------

### file formats:

  *  support ncx-based table of content in epub documents;
  *  support epub 3.0 metadata;
  *  basic support for SSML.

### cainteoir application and engine:

  *  support listing a document's table of contents and read/record a
     specified content range;
  *  support setting the voice reading speed, pitch and volume.

Version 0.3: Friday July 1st 2011 [Jun 2011]
--------------------------------------------

### file formats:

  *  support for single-file HTML pages (MHTML, MHT);
  *  support for RTF documents;
  *  enhanced HTML support.

### bug fixes:

  *  don't segfault if the OPF document is missing from an epub file.

### cainteoir:

  *  improved total reading time estimating heuristics.

Version 0.2: Sunday May 29th 2011 [May 2011]
--------------------------------------------

### file formats:

  *  support email (RFC822) mbox format;
  *  basic support for RTF documents.

### metadata:

  *  add rdfs:domain, rdfs:range and OWL markup to the text-to-speech schema;
  *  install the text-to-speech RDF schema used by the Cainteoir Engine;
  *  extract language and title metadata from HTML documents;
  *  extract metadata from email From and Subject headers.

### cainteoir:

  *  list supported document and audio formats in the --help output;
  *  don't output progress when writing audio data to stdout;
  *  don't crash when using the default voice (no dc:language metadata).

Version 0.1: Wednesday April 27th 2011 [Apr 2011]
-------------------------------------------------

### cainteoir:

  *  command-line text-to-speech program;
  *  support for using espeak text-to-speech voices;
  *  listen to text, html and epub documents on the command line;
  *  record text, html and epub documents to ogg/vorbis and wave audio;
  *  see the reading/recording progress and (estimated) total time;
  *  preserve metadata from document to audio (currently epub to ogg/vorbis).

### metadata:

  *  extract metadata from epub, smil, rdf/xml and other documents;
  *  generate RDF N-Triples
  *  generate RDF Turtle statements
  *  generate VorbisComment metadata
