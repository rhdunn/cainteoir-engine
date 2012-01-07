/* OPF Document Parser.
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

#include "parsers.hpp"
#include <cainteoir/platform.hpp>

namespace rdf = cainteoir::rdf;
namespace xmldom = cainteoir::xmldom;

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

void parseOpfMetadata(const xmldom::node &opf, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph, bool recurse)
{
	for (xmldom::attribute attr = opf.firstAttribute(); attr.isValid(); attr.next())
	{
		if (!strcmp(attr.name(), "prefix"))
			aGraph.add_prefix(attr.content());
	}

	for (xmldom::node node = opf.firstChild(); node.isValid(); node.next())
	{
		if (node.type() != XML_ELEMENT_NODE)
			continue;

		if (node == rdf::opf("dc-metadata") && recurse)
			parseOpfMetadata(node, aSubject, events, aGraph, false);
		else if (node == rdf::opf("meta"))
		{
			std::string content;
			std::string id;
			rdf::uri name;
			rdf::uri property;
			rdf::uri about = aSubject;
			rdf::uri datatype;

			for (xmldom::attribute attr = node.firstAttribute(); attr.isValid(); attr.next())
			{
				if (!strcmp(attr.name(), "name"))
				{
					std::tr1::shared_ptr<const rdf::uri> uri = aGraph.curie(attr.content());
					if (uri.get() && !uri->ns.empty())
						name = *uri;
				}
				else if (!strcmp(attr.name(), "content"))
					content = attr.content();
				else if (!strcmp(attr.name(), "property"))
				{
					std::tr1::shared_ptr<const rdf::uri> uri = aGraph.curie(attr.content());
					if (uri.get() && !uri->ns.empty())
					{
						property = *uri;
						if (datatype.empty() && property == rdf::pkg("display-seq"))
							datatype = rdf::xsd("unsignedInt");
					}
				}
				else if (!strcmp(attr.name(), "id"))
					id = attr.content();
				else if (!strcmp(attr.name(), "about") || !strcmp(attr.name(), "refines"))
					about = rdf::uri(aSubject.str(), attr.content().substr(1));
				else if (!strcmp(attr.name(), "datatype") || !strcmp(attr.name(), "scheme"))
				{
					std::tr1::shared_ptr<const rdf::uri> uri = aGraph.curie(attr.content());
					if (uri.get() && !uri->ns.empty())
						datatype = *uri;
				}
			}

			if (!name.empty() && !content.empty())
				aGraph.statement(aSubject, name, rdf::literal(content));
			else if (!property.empty())
			{
				rdf::literal object = rdf::literal(node.content()->str(), datatype);

				if (!id.empty())
				{
					const rdf::uri base(aSubject.str(), id);
					aGraph.statement(about, property, base);
					aGraph.statement(base, rdf::rdf("value"), object);
				}
				else
					aGraph.statement(about, property, object);
			}
		}
		else if (node == rdf::opf("link"))
		{
			std::string rel;
			std::string id;
			rdf::uri href;
			rdf::uri about = aSubject;

			for (xmldom::attribute attr = node.firstAttribute(); attr.isValid(); attr.next())
			{
				if (!strcmp(attr.name(), "rel"))
					rel = attr.content();
				else if (!strcmp(attr.name(), "href"))
					href = aGraph.href(attr.content());
				else if (!strcmp(attr.name(), "id"))
					id = attr.content();
				else if (!strcmp(attr.name(), "about") || !strcmp(attr.name(), "refines"))
					about = rdf::uri(aSubject.str(), attr.content().substr(1));
			}

			if (!rel.empty() && !href.empty())
			{
				std::istringstream ss(rel);
				while (ss >> rel)
				{
					std::tr1::shared_ptr<const rdf::uri> uri = aGraph.curie(rel);
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
		}
		else if (node.namespaceURI() == rdf::dc)
		{
			bool preferOther = false;
			for (xmldom::attribute attr = node.firstAttribute(); attr.isValid(); attr.next())
			{
				if (!strcmp(attr.name(), "prefer"))
					preferOther = true;
			}
			if (preferOther)
				continue;

			std::string lang = node.attr(rdf::xml("lang")).content();
			std::string value = node.content()->str();

			const rdf::uri predicate = rdf::uri(node.namespaceURI(), node.name());

			if (!strcmp(node.name(), "creator") || !strcmp(node.name(), "contributor"))
			{
				std::string role = node.attr(rdf::opf("role")).content();
				std::string fileas = node.attr(rdf::opf("file-as")).content();
				if (!role.empty() || !fileas.empty())
				{
					const rdf::uri temp = aGraph.genid();
					aGraph.statement(aSubject, predicate, temp);
					aGraph.statement(temp, rdf::rdf("value"), rdf::literal(value, lang));
					if (!role.empty())
						aGraph.statement(temp, rdf::opf("role"), rdf::literal(role));
					if (!fileas.empty())
						aGraph.statement(temp, rdf::opf("file-as"), rdf::literal(fileas));
					continue;
				}
			}
			else if (!strcmp(node.name(), "date"))
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

				std::string event = node.attr(rdf::opf("event")).content();
				if (!event.empty())
				{
					const rdf::uri temp = aGraph.genid();
					aGraph.statement(aSubject, predicate, temp);
					aGraph.statement(temp, rdf::rdf("value"), rdf::literal(value, lang));
					aGraph.statement(temp, rdf::opf("event"), rdf::literal(event));
					continue;
				}
			}
			else if (!strcmp(node.name(), "identifier"))
			{
				std::string scheme = node.attr(rdf::opf("scheme")).content();
				if (!scheme.empty())
				{
					const rdf::uri temp = aGraph.genid();
					aGraph.statement(aSubject, predicate, temp);
					aGraph.statement(temp, rdf::rdf("value"), rdf::literal(value, lang));
					aGraph.statement(temp, rdf::opf("scheme"), rdf::literal(scheme));
					continue;
				}
			}

			aGraph.statement(aSubject, predicate, rdf::literal(value, lang));
		}
	}
}

void parseOpfManifest(const xmldom::node &opf, const rdf::uri &subject, std::map<std::string, fileinfo> &aItemSet)
{
	for (xmldom::node node = opf.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE && node == rdf::opf("item"))
		{
			std::string id;
			std::string href;
			std::string mediatype;

			for (xmldom::attribute attr = node.firstAttribute(); attr.isValid(); attr.next())
			{
				if (!strcmp(attr.name(), "id"))
					id = attr.content();
				else if (!strcmp(attr.name(), "href"))
					href = attr.content();
				else if (!strcmp(attr.name(), "media-type"))
					mediatype = attr.content();
			}

			aItemSet[id] = fileinfo(href, mediatype, id);
		}
	}
}

void parseOpfSpine(const xmldom::node &opf, const rdf::uri &subject, std::list<std::string> &aSpine)
{
	for (xmldom::node node = opf.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE && node == rdf::opf("itemref"))
		{
			for (xmldom::attribute attr = node.firstAttribute(); attr.isValid(); attr.next())
			{
				if (!strcmp(attr.name(), "idref"))
					aSpine.push_back(attr.content());
			}
		}
	}
}

void cainteoir::parseOpfDocument(std::tr1::shared_ptr<cainteoir::buffer> aData, const rdf::uri &aSubject, document_events &events, rdf::graph &aGraph)
{
	xmldom::document doc(aData);
	xmldom::node opf = doc.root();

	if (opf != rdf::opf("package"))
		throw std::runtime_error(_("OPF file is not of a recognised format."));

	std::string toc;
	std::list<std::string> spine;
	std::map<std::string, fileinfo> files;

	aGraph.set_base(rdf::pkg.href);

	for (xmldom::attribute attr = opf.firstAttribute(); attr.isValid(); attr.next())
	{
		if (!strcmp(attr.name(), "profile") && attr.content() == "http://www.idpf.org/epub/30/profile/package/")
			aGraph << rdf::ns("dcterms", rdf::dcterms.href) << rdf::media << rdf::xsd;
		else if (!strcmp(attr.name(), "prefix"))
			aGraph.add_prefix(attr.content());
	}

	for (xmldom::node section = opf.firstChild(); section.isValid(); section.next())
	{
		if (section.type() == XML_ELEMENT_NODE)
		{
			if (section == rdf::opf("metadata"))
				parseOpfMetadata(section, aSubject, events, aGraph, true);
			else if (section == rdf::opf("manifest"))
				parseOpfManifest(section, aSubject, files);
			else if (section == rdf::opf("spine"))
			{
				for (xmldom::attribute attr = section.firstAttribute(); attr.isValid(); attr.next())
				{
					if (!strcmp(attr.name(), "toc"))
					{
						fileinfo &toc = files[attr.content()];
						events.anchor(rdf::uri(toc.filename, std::string()), toc.mimetype);
					}
				}
				parseOpfSpine(section, aSubject, spine);
			}
		}
	}

	foreach_iter(id, spine)
	{
		fileinfo &ref = files[*id];
		events.anchor(rdf::uri(ref.filename, std::string()), ref.mimetype);
	}
}
