/* Document Parser API.
 *
 * Copyright (C) 2010-2011 Reece H. Dunn
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

#ifndef CAINTEOIR_ENGINE_PARSERS_HPP
#define CAINTEOIR_ENGINE_PARSERS_HPP

#include <cainteoir/document.hpp>
#include "xml.hpp"
#include <list>
#include <map>

namespace cainteoir
{
	/** @brief Open Container Format (OCF)
	  * @see   http://www.idpf.org/ocf/ocf1.0/download/ocf10.htm
	  * @see   http://www.idpf.org/specs.htm
	  */
	class ocf_reader
	{
	public:
		ocf_reader(std::tr1::shared_ptr<cainteoir::buffer> aData);

		bool read();

		const std::string &mediaType() const { return mMediaType; }

		const std::string &path() const { return mPath; }
	private:
		xml::reader mReader;
		std::string mMediaType;
		std::string mPath;
	};

	/** @brief Navigation Control File (NCX)
	  * @see   http://www.niso.org/workrooms/daisy/Z39-86-2005.html#NCX
	  *
	  * @param aData    The document data.
	  * @param aSubject The subject to use for any metadata.
	  * @param events   The events callback to handle document events.
	  */
	void parseNcxDocument(std::tr1::shared_ptr<cainteoir::buffer> aData, const rdf::uri &aSubject, document_events &events, rdf::graph &aGraph);

	/** @brief Open Publication Format (OPF)
	  * @see   http://www.idpf.org/2007/opf/opf2.0/download/
	  * @see   http://www.idpf.org/specs.htm
	  *
	  * @param aData    The document data.
	  * @param aSubject The subject to use for any Dublin Core metadata.
	  * @param events   The events callback to handle document events.
	  */
	void parseOpfDocument(std::tr1::shared_ptr<cainteoir::buffer> aData, const rdf::uri &aSubject, document_events &events, rdf::graph &aGraph);

	/** @brief XML encoded HTML (XHTML)
	  * @see   http://www.w3.org/TR/xhtml1/
	  *
	  * @param aData    The document data.
	  * @param aSubject The base to use for any relative URIs.
	  * @param events   The events callback to handle document events.
	  */
	void parseXHtmlDocument(std::tr1::shared_ptr<cainteoir::buffer> aData, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph);

	/** @brief RDF/XML
	  * @see   http://www.w3.org/TR/2004/REC-rdf-syntax-grammar-20040210/
	  *
	  * @param aData    The document data.
	  * @param aSubject The base to use for any relative URIs.
	  * @param events   The events callback to handle document events.
	  */
	void parseRdfXmlDocument(std::tr1::shared_ptr<cainteoir::buffer> aData, const rdf::uri &aSubject, document_events &events, rdf::graph &aGraph);

	/** @brief Synchronized Multimedia Integration Language (SMIL)
	  * @see   http://www.w3.org/TR/2008/REC-SMIL3-20081201/
	  *
	  * @param aData    The document data.
	  * @param aSubject The base to use for any relative URIs.
	  * @param events   The events callback to handle document events.
	  */
	void parseSmilDocument(std::tr1::shared_ptr<cainteoir::buffer> aData, const rdf::uri &aSubject, document_events &events, rdf::graph &aGraph);

	/** @brief Speech Synthesis Markup Language (SSML)
	  * @see   http://www.w3.org/TR/speech-synthesis/
	  *
	  * @param aData    The document data.
	  * @param aSubject The base to use for any relative URIs.
	  * @param events   The events callback to handle document events.
	  */
	void parseSsmlDocument(std::tr1::shared_ptr<cainteoir::buffer> aData, const rdf::uri &aSubject, document_events &events, rdf::graph &aGraph);

	/** @brief ePub
	  * @see   http://www.idpf.org/specs.htm
	  *
	  * @param aData    The document data.
	  * @param aSubject The base to use for any relative URIs.
	  * @param events   The events callback to handle document events.
	  */
	void parseEpubDocument(std::tr1::shared_ptr<cainteoir::buffer> aData, const rdf::uri &aSubject, document_events &events, rdf::graph &aGraph);

	/** @brief RTF
	  *
	  * @param aData    The document data.
	  * @param aSubject The base to use for any relative URIs.
	  * @param events   The events callback to handle document events.
	  */
	void parseRtfDocument(std::tr1::shared_ptr<cainteoir::buffer> aData, const rdf::uri &aSubject, document_events &events, rdf::graph &aGraph);
}

#endif
