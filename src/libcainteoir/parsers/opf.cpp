/* OPF Document Parser.
 *
 * Copyright (C) 2010-2012 Reece H. Dunn
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

#include "parsers.hpp"
#include <stdexcept>

namespace xml   = cainteoir::xml;
namespace xmlns = cainteoir::xml::xmlns;
namespace rdf   = cainteoir::rdf;

namespace dc
{
	namespace events = cainteoir::events;

	static const xml::context::entry contributor_node = { events::unknown, 0 };
	static const xml::context::entry coverage_node    = { events::unknown, 0 };
	static const xml::context::entry creator_node     = { events::unknown, 0 };
	static const xml::context::entry date_node        = { events::unknown, 0 };
	static const xml::context::entry description_node = { events::unknown, 0 };
	static const xml::context::entry format_node      = { events::unknown, 0 };
	static const xml::context::entry identifier_node  = { events::unknown, 0 };
	static const xml::context::entry language_node    = { events::unknown, 0 };
	static const xml::context::entry publisher_node   = { events::unknown, 0 };
	static const xml::context::entry relation_node    = { events::unknown, 0 };
	static const xml::context::entry rights_node      = { events::unknown, 0 };
	static const xml::context::entry source_node      = { events::unknown, 0 };
	static const xml::context::entry subject_node     = { events::unknown, 0 };
	static const xml::context::entry title_node       = { events::unknown, 0 };
	static const xml::context::entry type_node        = { events::unknown, 0 };
}

namespace opf
{
	namespace events = cainteoir::events;

	static const xml::context::entry dcmetadata_node = { events::unknown, 0 };
	static const xml::context::entry item_node       = { events::unknown, 0 };
	static const xml::context::entry itemref_node    = { events::unknown, 0 };
	static const xml::context::entry link_node       = { events::unknown, 0 };
	static const xml::context::entry manifest_node   = { events::unknown, 0 };
	static const xml::context::entry meta_node       = { events::unknown, 0 };
	static const xml::context::entry metadata_node   = { events::unknown, 0 };
	static const xml::context::entry package_node    = { events::unknown, 0 };
	static const xml::context::entry spine_node      = { events::unknown, 0 };

	static const xml::context::entry about_attr     = { events::unknown, 0 };
	static const xml::context::entry content_attr   = { events::unknown, 0 };
	static const xml::context::entry datatype_attr  = { events::unknown, 0 };
	static const xml::context::entry event_attr     = { events::unknown, 0 };
	static const xml::context::entry fileas_attr    = { events::unknown, 0 };
	static const xml::context::entry href_attr      = { events::unknown, 0 };
	static const xml::context::entry id_attr        = { events::unknown, 0 };
	static const xml::context::entry idref_attr     = { events::unknown, 0 };
	static const xml::context::entry mediatype_attr = { events::unknown, 0 };
	static const xml::context::entry name_attr      = { events::unknown, 0 };
	static const xml::context::entry prefer_attr    = { events::unknown, 0 };
	static const xml::context::entry prefix_attr    = { events::unknown, 0 };
	static const xml::context::entry property_attr  = { events::unknown, 0 };
	static const xml::context::entry refines_attr   = { events::unknown, 0 };
	static const xml::context::entry rel_attr       = { events::unknown, 0 };
	static const xml::context::entry role_attr      = { events::unknown, 0 };
	static const xml::context::entry scheme_attr    = { events::unknown, 0 };
	static const xml::context::entry toc_attr       = { events::unknown, 0 };
	static const xml::context::entry version_attr   = { events::unknown, 0 };
}

static const std::initializer_list<const xml::context::entry_ref> dc_nodes =
{
	{ "contributor", &dc::contributor_node },
	{ "coverage",    &dc::coverage_node },
	{ "creator",     &dc::creator_node },
	{ "date",        &dc::date_node },
	{ "description", &dc::description_node },
	{ "format",      &dc::format_node },
	{ "identifier",  &dc::identifier_node },
	{ "language",    &dc::language_node },
	{ "publisher",   &dc::publisher_node },
	{ "relation",    &dc::relation_node },
	{ "rights",      &dc::rights_node },
	{ "source",      &dc::source_node },
	{ "subject",     &dc::subject_node },
	{ "title",       &dc::title_node },
	{ "type",        &dc::type_node },
};

static const std::initializer_list<const xml::context::entry_ref> opf_nodes =
{
	{ "dc-metadata", &opf::dcmetadata_node },
	{ "item",        &opf::item_node },
	{ "itemref",     &opf::itemref_node },
	{ "link",        &opf::link_node },
	{ "manifest",    &opf::manifest_node },
	{ "meta",        &opf::meta_node },
	{ "metadata",    &opf::metadata_node },
	{ "package",     &opf::package_node },
	{ "spine",       &opf::spine_node },
};

static const std::initializer_list<const xml::context::entry_ref> opf_attrs =
{
	{ "about",      &opf::about_attr },
	{ "content",    &opf::content_attr },
	{ "datatype",   &opf::datatype_attr },
	{ "event",      &opf::event_attr },
	{ "file-as",    &opf::fileas_attr },
	{ "href",       &opf::href_attr },
	{ "id",         &opf::id_attr },
	{ "idref",      &opf::idref_attr },
	{ "media-type", &opf::mediatype_attr },
	{ "name",       &opf::name_attr },
	{ "prefer",     &opf::prefer_attr },
	{ "prefix",     &opf::prefix_attr },
	{ "property",   &opf::property_attr },
	{ "refines",    &opf::refines_attr },
	{ "rel",        &opf::rel_attr },
	{ "role",       &opf::role_attr },
	{ "scheme",     &opf::scheme_attr },
	{ "toc",        &opf::toc_attr },
	{ "version",    &opf::version_attr },
};

struct fileinfo
{
	std::string filename; /**< @brief The name of the file. */
	std::string mimetype; /**< @brief The mime type of the file. */
	std::string id;       /**< @brief The unique id for the file within the current document. */

	fileinfo(const std::string &aFileName, const std::string &aMimeType, const std::string &aId)
		: filename(aFileName)
		, mimetype(aMimeType)
		, id(aId)
	{
	}

	fileinfo()
	{
	}
};

void parseOpfMeta(xml::reader &reader, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph)
{
	std::string value;
	std::string content;
	std::string id;
	rdf::uri name;
	rdf::uri property;
	rdf::uri about = aSubject;
	rdf::uri datatype;

	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::attribute:
		if (reader.context() == &opf::name_attr)
		{
			std::shared_ptr<const rdf::uri> uri = aGraph.curie(reader.nodeValue().str());
			if (uri.get() && !uri->ns.empty())
				name = *uri;
		}
		else if (reader.context() == &opf::content_attr)
			content = reader.nodeValue().str();
		else if (reader.context() == &opf::property_attr)
		{
			std::shared_ptr<const rdf::uri> uri = aGraph.curie(reader.nodeValue().str());
			if (uri.get() && !uri->ns.empty())
			{
				property = *uri;
				if (datatype.empty() && property == rdf::pkg("display-seq"))
					datatype = rdf::xsd("unsignedInt");
			}
		}
		else if (reader.context() == &opf::id_attr)
			id = reader.nodeValue().str();
		else if (reader.context() == &opf::about_attr || reader.context() == &opf::refines_attr)
			about = rdf::uri(aSubject.str(), reader.nodeValue().str().substr(1));
		else if (reader.context() == &opf::datatype_attr || reader.context() == &opf::scheme_attr)
		{
			std::shared_ptr<const rdf::uri> uri = aGraph.curie(reader.nodeValue().str());
			if (uri.get() && !uri->ns.empty())
				datatype = *uri;
		}
		break;
	case xml::reader::textNode:
	case xml::reader::cdataNode:
		value = reader.nodeValue().str();
		break;
	case xml::reader::endTagNode:
		if (reader.context() == &opf::meta_node)
		{
			if (!name.empty() && !content.empty())
				aGraph.statement(aSubject, name, rdf::literal(content));
			else if (!property.empty())
			{
				rdf::literal object = rdf::literal(value, datatype);
				if (!id.empty())
				{
					const rdf::uri base(aSubject.str(), id);
					aGraph.statement(about, property, base);
					aGraph.statement(base, rdf::rdf("value"), object);
				}
				else
					aGraph.statement(about, property, object);
			}
			return;
		}
		break;
	}
}

void parseOpfLink(xml::reader &reader, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph)
{
	std::string rel;
	std::string id;
	rdf::uri href;
	rdf::uri about = aSubject;

	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::attribute:
		if (reader.context() == &opf::rel_attr)
			rel = reader.nodeValue().str();
		else if (reader.context() == &opf::href_attr)
			href = reader.nodeValue().str();
		else if (reader.context() == &opf::id_attr)
			id = reader.nodeValue().str();
		else if (reader.context() == &opf::about_attr || reader.context() == &opf::refines_attr)
			about = rdf::uri(aSubject.str(), reader.nodeValue().str().substr(1));
		break;
	case xml::reader::endTagNode:
		if (reader.context() == &opf::link_node)
		{
			if (!rel.empty() && !href.empty())
			{
				std::istringstream ss(rel);
				while (ss >> rel)
				{
					std::shared_ptr<const rdf::uri> uri = aGraph.curie(rel);
					if (uri.get() && !uri->ns.empty())
					{
						if (!id.empty())
						{
							const rdf::uri base(aSubject.str(), id);
							aGraph.statement(about, *uri, base);
							aGraph.statement(base, rdf::rdf("value"), href);
						}
						else
							aGraph.statement(about, *uri, href);
					}
				}
			}
			return;
		}
		break;
	}
}

void parseOpfDublinCore(xml::reader &reader, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph, const xml::context::entry *ctx)
{
	std::string lang;
	std::string value;
	std::string role;
	std::string fileas;
	std::string event;
	std::string scheme;

	const rdf::uri predicate = rdf::uri(reader.namespaceUri(), reader.nodeName().str());

	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::attribute:
		if (reader.context() == &opf::prefer_attr)
			return;
		else if (reader.context() == &xml::lang_attr)
			lang = reader.nodeValue().str();
		else if (reader.context() == &opf::fileas_attr)
			fileas = reader.nodeValue().str();
		else if (reader.context() == &opf::role_attr)
			role = reader.nodeValue().str();
		else if (reader.context() == &opf::event_attr)
			event = reader.nodeValue().str();
		else if (reader.context() == &opf::scheme_attr)
			scheme = reader.nodeValue().str();
		break;
	case xml::reader::textNode:
	case xml::reader::cdataNode:
		value = reader.nodeValue().normalize()->str();
		break;
	case xml::reader::endTagNode:
		if (reader.context() == ctx)
		{
			if ((reader.context() == &dc::contributor_node || reader.context() == &dc::creator_node) && (!role.empty() || !fileas.empty()))
			{
				const rdf::uri temp = aGraph.genid();
				aGraph.statement(aSubject, predicate, temp);
				aGraph.statement(temp, rdf::rdf("value"), rdf::literal(value, lang));
				if (!role.empty())
					aGraph.statement(temp, rdf::opf("role"), rdf::literal(role));
				if (!fileas.empty())
					aGraph.statement(temp, rdf::opf("file-as"), rdf::literal(fileas));
				return;
			}
			else if (reader.context() == &dc::date_node)
			{
				std::string::size_type dm = value.find('/');
				std::string::size_type my = value.find('/', dm+1);

				if (dm != std::string::npos && my != std::string::npos)
				{
					std::string day   = value.substr(0, dm);
					std::string month = value.substr(dm+1, my-dm-1);
					std::string year  = value.substr(my+1);

					std::ostringstream date;
					date << year << '-' << month << '-' << day;
					value = date.str();
				}

				if (!event.empty())
				{
					const rdf::uri temp = aGraph.genid();
					aGraph.statement(aSubject, predicate, temp);
					aGraph.statement(temp, rdf::rdf("value"), rdf::literal(value, lang));
					aGraph.statement(temp, rdf::opf("event"), rdf::literal(event));
					return;
				}
			}
			else if (reader.context() == &dc::identifier_node && !scheme.empty())
			{
				const rdf::uri temp = aGraph.genid();
				aGraph.statement(aSubject, predicate, temp);
				aGraph.statement(temp, rdf::rdf("value"), rdf::literal(value, lang));
				aGraph.statement(temp, rdf::opf("scheme"), rdf::literal(scheme));
				return;
			}
			aGraph.statement(aSubject, predicate, rdf::literal(value, lang));
			return;
		}
		break;
	}
}

void parseOpfMetadata(xml::reader &reader, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph, const xml::context::entry *ctx)
{
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::attribute:
		if (reader.context() == &opf::prefix_attr && ctx == &opf::metadata_node)
			aGraph.add_prefix(reader.nodeValue().str());
		break;
	case xml::reader::beginTagNode:
		if (ctx == &opf::metadata_node)
		{
			if (reader.context() == &opf::dcmetadata_node)
				parseOpfMetadata(reader, aSubject, events, aGraph, reader.context());
			else if (reader.context() == &opf::meta_node)
				parseOpfMeta(reader, aSubject, events, aGraph);
			else if (reader.context() == &opf::link_node)
				parseOpfLink(reader, aSubject, events, aGraph);
			else if (reader.namespaceUri() == xmlns::dc.href && reader.context() != &xml::unknown_context)
				parseOpfDublinCore(reader, aSubject, events, aGraph, reader.context());
		}
		else if (reader.namespaceUri() == xmlns::dc.href && reader.context() != &xml::unknown_context)
			parseOpfDublinCore(reader, aSubject, events, aGraph, reader.context());
		break;
	case xml::reader::endTagNode:
		if (reader.context() == ctx)
			return;
		break;
	}
}

void parseOpfItem(xml::reader &reader, std::map<std::string, fileinfo> &aItemSet)
{
	std::string id;
	std::string href;
	std::string mediatype;

	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::attribute:
		if (reader.context() == &opf::href_attr)
			href = reader.nodeValue().str();
		else if (reader.context() == &opf::id_attr)
			id = reader.nodeValue().str();
		else if (reader.context() == &opf::mediatype_attr)
			mediatype = reader.nodeValue().str();
		break;
	case xml::reader::endTagNode:
		if (reader.context() == &opf::item_node)
		{
			aItemSet[id] = fileinfo(href, mediatype, id);
			return;
		}
		break;
	}
}

void parseOpfManifest(xml::reader &reader, std::map<std::string, fileinfo> &aItemSet)
{
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::beginTagNode:
		if (reader.context() == &opf::item_node)
			parseOpfItem(reader, aItemSet);
		break;
	case xml::reader::endTagNode:
		if (reader.context() == &opf::manifest_node)
			return;
		break;
	}
}

std::string parseOpfItemRef(xml::reader &reader)
{
	std::string ref;
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::attribute:
		if (reader.context() == &opf::idref_attr)
			ref = reader.nodeValue().str();
		break;
	case xml::reader::endTagNode:
		if (reader.context() == &opf::itemref_node)
			return ref;
		break;
	}
	return ref;
}

void parseOpfSpine(xml::reader &reader, cainteoir::document_events &events, std::list<std::string> &aSpine, std::map<std::string, fileinfo> &files)
{
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::attribute:
		if (reader.context() == &opf::toc_attr)
		{
			fileinfo &toc = files[reader.nodeValue().str()];
			events.anchor(rdf::uri(toc.filename, std::string()), toc.mimetype);
		}
		break;
	case xml::reader::beginTagNode:
		if (reader.context() == &opf::itemref_node)
			aSpine.push_back(parseOpfItemRef(reader));
		break;
	}
}

void cainteoir::parseOpfDocument(xml::reader &reader, const rdf::uri &aSubject, document_events &events, rdf::graph &aGraph)
{
	reader.set_nodes(xmlns::opf, opf_nodes);
	reader.set_attrs(xmlns::opf, opf_attrs);
	reader.set_nodes(xmlns::dc,  dc_nodes);
	reader.set_attrs(xmlns::xml, xml::attrs);

	std::string toc;
	std::list<std::string> spine;
	std::map<std::string, fileinfo> files;

	aGraph.set_base(rdf::pkg.href);

	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::attribute:
		if (reader.context() == &opf::version_attr && reader.nodeValue().str() == "3.0")
			aGraph << rdf::ns("dcterms", rdf::dcterms.href) << rdf::media << rdf::xsd;
		else if (reader.context() == &opf::prefix_attr)
			aGraph.add_prefix(reader.nodeValue().str());
		break;
	case xml::reader::beginTagNode:
		if (reader.context() == &opf::metadata_node)
			parseOpfMetadata(reader, aSubject, events, aGraph, reader.context());
		else if (reader.context() == &opf::manifest_node)
			parseOpfManifest(reader, files);
		else if (reader.context() == &opf::spine_node)
			parseOpfSpine(reader, events, spine, files);
		break;
	}

	foreach_iter(id, spine)
	{
		fileinfo &ref = files[*id];
		events.anchor(rdf::uri(ref.filename, std::string()), ref.mimetype);
	}
}
