/* Document Parser API.
 *
 * Copyright (C) 2010 Reece H. Dunn
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

#ifndef CAINTEOIR_ENGINE_DOCUMENT_HPP
#define CAINTEOIR_ENGINE_DOCUMENT_HPP

#include <cainteoir/metadata.hpp>

namespace cainteoir
{
	/** @brief Read the contents of a document.
	  *
	  * @param aFilename The path to the ePub document.
	  * @param aMetadata The RDF model to add any metadata to.
	  *
	  * @retval true  If aFilename contains a supported document format.
	  * @retval false If aFilename contains an unsupported document format.
	  *
	  * The following document formats require adding parsers to support them:
	  *    -  Atom:    extract syndication information from Atom feeds.
	  *    -  DocBook: extract information from DocBook 4.0 (SGML) and DocBook 5.0 (XML) documents.
	  *    -  (X)HTML: extract information from HTML4, HTML5 and XHTML documents (including meta+DC and RDF/a metadata).
	  *    -  NCX:     extract Table of Content information from NCX documents (needed for complete ePub support).
	  *    -  ODT:     extract information from ODF/text documents.
	  *    -  PLS:     extract pronunciation information from Pronunciation Lexicon Specification documents.
	  *    -  RSS:     extract syndication information from RSS 1.0 (RSS/RDF/XML) and 2.0 (RSS/XML) feeds.
	  *    -  RTF:     extract information from Rich Text Format documents.
	  *    -  SSML:    extract information from Speech Synthesis Markup Language documents.
	  *    -  TXT:     extract information from plain text documents.
	  *
	  * The following document formats will not be supported:
	  *    -  DOC:     Microsoft Word document format.
	  *    -  DOCX:    Microsoft Word document format (XML-based).
	  *
	  * The following existing parsers need work to be complete:
	  *    -  OPF:     extract the information in the manifest and spine sections.
	  *    -  RDF/XML: complete the parser so that it is to spec (use test cases from the spec?).
	  *    -  SMIL:    extract text information from the SMIL document.
	  *    -  ePub:    extract text information from the ePub document (requires complete OPF, NCX and (X)HTML parsers).
	  */
	bool parseDocument(const char *aFilename, rdf::model &aMetadata);
}

#endif
