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
#include <cainteoir/buffer.hpp>

namespace cainteoir
{
	struct document_events
	{
		virtual void metadata(const rdf::statement &aStatement) = 0;

		virtual const rdf::bnode genid() = 0;

		virtual void text(std::tr1::shared_ptr<cainteoir::buffer> aText) = 0;

		virtual ~document_events() {}
	};

	enum event_type
	{
		text_event,
	};

	struct event
	{
		event_type type;
		std::tr1::shared_ptr<cainteoir::buffer> data;

		event(event_type aType, std::tr1::shared_ptr<cainteoir::buffer> aData)
			: type(aType)
			, data(aData)
		{
		}
	};

	/** @brief Read the contents of a document.
	  *
	  * @param aFilename The path to the ePub document.
	  * @param aMetadata The RDF model to add any metadata to.
	  * @param aEvents   The list of reading events that make up the document.
	  *
	  * @retval true  If aFilename contains a supported document format.
	  * @retval false If aFilename contains an unsupported document format.
	  */
	bool parseDocument(const char *aFilename, rdf::graph &aMetadata, std::list<event> &aEvents);
}

#endif
