/* Internal Document Parser API.
 *
 * Copyright (C) 2010-2013 Reece H. Dunn
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
#include <cainteoir/archive.hpp>
#include <cainteoir/path.hpp>

namespace cainteoir
{
	std::shared_ptr<document_reader>
	createEpubReader(std::shared_ptr<archive> &aData,
	                 const rdf::uri &aSubject,
	                 rdf::graph &aPrimaryMetadata,
	                 const char *aDefaultEncoding);

	std::shared_ptr<document_reader>
	createHtmlReader(const std::shared_ptr<xml::reader> &aReader,
	                 const rdf::uri &aSubject,
	                 rdf::graph &aPrimaryMetadata,
	                 const std::string &aTitle,
	                 const char *aMimeType);

	std::shared_ptr<document_reader>
	createMimeReader(std::shared_ptr<buffer> &aData,
	                 const rdf::uri &aSubject,
	                 rdf::graph &aPrimaryMetadata,
	                 const std::string &aTitle);

	std::shared_ptr<document_reader>
	createMimeInHtmlReader(std::shared_ptr<buffer> &aData,
	                       const rdf::uri &aSubject,
	                       rdf::graph &aPrimaryMetadata,
	                       const std::string &aTitle,
                               const char *aDefaultEncoding);

	std::shared_ptr<document_reader>
	createNcxReader(const std::shared_ptr<xml::reader> &aReader,
	                const rdf::uri &aSubject,
	                rdf::graph &aPrimaryMetadata,
	                const std::string &aTitle,
	                const cainteoir::path &aBaseUri);

	std::shared_ptr<document_reader>
	createOcfReader(const std::shared_ptr<xml::reader> &aReader);

	std::shared_ptr<document_reader>
	createOpfReader(const std::shared_ptr<xml::reader> &aReader,
	                const rdf::uri &aSubject,
	                rdf::graph &aPrimaryMetadata,
	                const char *aMimeType = "application/oebps-package+xml");

	std::shared_ptr<document_reader>
	createPdfReader(std::shared_ptr<buffer> &aData,
	                const rdf::uri &aSubject,
	                rdf::graph &aPrimaryMetadata,
	                const std::string &aTitle);

	std::shared_ptr<document_reader>
	createPlainTextReader(std::shared_ptr<buffer> &aData,
	                      const rdf::uri &aSubject,
	                      rdf::graph &aPrimaryMetadata,
	                      const std::string &aTitle);

	std::shared_ptr<document_reader>
	createRdfXmlReader(const std::shared_ptr<xml::reader> &aReader,
	                   const rdf::uri &aSubject,
	                   rdf::graph &aPrimaryMetadata);

	std::shared_ptr<document_reader>
	createRtfReader(std::shared_ptr<buffer> &aData,
	                const rdf::uri &aSubject,
	                rdf::graph &aPrimaryMetadata,
	                const std::string &aTitle);

	std::shared_ptr<document_reader>
	createSmilReader(const std::shared_ptr<xml::reader> &aReader,
	                 const rdf::uri &aSubject,
	                 rdf::graph &aPrimaryMetadata,
	                 const std::string &aTitle);

	std::shared_ptr<document_reader>
	createSsmlReader(const std::shared_ptr<xml::reader> &aReader,
	                 const rdf::uri &aSubject,
	                 rdf::graph &aPrimaryMetadata,
	                 const std::string &aTitle);

	std::shared_ptr<document_reader>
	createZipReader(std::shared_ptr<archive> &aData);

	std::pair<bool, std::shared_ptr<buffer>>
	parseMimeHeaders(std::shared_ptr<buffer> &aData,
                         const rdf::uri &aSubject,
                         rdf::graph &aPrimaryMetadata,
                         std::string &aTitle);
}

#endif
