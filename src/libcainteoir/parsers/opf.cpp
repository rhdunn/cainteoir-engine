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
namespace xml = cainteoir::xmldom;

void parseOpfMetadata(const xml::node &opf, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::namespaces &rdfa, bool recurse)
{
	for (xml::attribute attr = opf.firstAttribute(); attr.isValid(); attr.next())
	{
		if (!strcmp(attr.name(), "prefix"))
			rdfa.add_prefix(attr.content());
	}

	for (xml::node node = opf.firstChild(); node.isValid(); node.next())
	{
		if (node.type() != XML_ELEMENT_NODE)
			continue;

		if (node == rdf::opf("dc-metadata") && recurse)
			parseOpfMetadata(node, aSubject, events, rdfa, false);
		else if (node == rdf::opf("meta"))
		{
			std::string content;
			std::string id;
			rdf::uri name;
			rdf::uri property;
			rdf::uri about = aSubject;
			rdf::uri datatype;

			for (xml::attribute attr = node.firstAttribute(); attr.isValid(); attr.next())
			{
				if (!strcmp(attr.name(), "name"))
				{
					std::tr1::shared_ptr<const rdf::detail::resource> type = rdfa(attr.content());
					const rdf::uri *uri = dynamic_cast<const rdf::uri *>(type.get());
					if (uri)
						name = *uri;
				}
				else if (!strcmp(attr.name(), "content"))
					content = attr.content();
				else if (!strcmp(attr.name(), "property"))
				{
					std::tr1::shared_ptr<const rdf::detail::resource> type = rdfa(attr.content());
					const rdf::uri *uri = dynamic_cast<const rdf::uri *>(type.get());
					if (uri)
					{
						property = *uri;
						if (datatype.empty() && property == rdf::pkg("display-seq"))
							datatype = rdf::xsd("unsignedInt");
					}
				}
				else if (!strcmp(attr.name(), "id"))
					id = attr.content();
				else if (!strcmp(attr.name(), "about"))
					about = rdf::uri(aSubject.str(), attr.content().substr(1));
				else if (!strcmp(attr.name(), "datatype"))
				{
					std::tr1::shared_ptr<const rdf::detail::resource> type = rdfa(attr.content());
					const rdf::uri *uri = dynamic_cast<const rdf::uri *>(type.get());
					if (uri)
						datatype = *uri;
				}
			}

			if (!name.empty() && !content.empty())
				events.metadata(rdf::statement(aSubject, name, rdf::literal(content)));
			else if (!property.empty())
			{
				rdf::literal object = rdf::literal(node.content()->str(), datatype);

				if (!id.empty())
				{
					const rdf::uri base(aSubject.str(), id);
					events.metadata(rdf::statement(about, property, base));
					events.metadata(rdf::statement(base, rdf::rdf("value"), object));
				}
				else
					events.metadata(rdf::statement(about, property, object));
			}
		}
		else if (node == rdf::opf("link"))
		{
			std::string rel;
			std::string id;
			rdf::uri href;
			rdf::uri about = aSubject;

			for (xml::attribute attr = node.firstAttribute(); attr.isValid(); attr.next())
			{
				if (!strcmp(attr.name(), "rel"))
					rel = attr.content();
				else if (!strcmp(attr.name(), "href"))
					href = rdf::href(attr.content());
				else if (!strcmp(attr.name(), "id"))
					id = attr.content();
				else if (!strcmp(attr.name(), "about"))
					about = rdf::uri(aSubject.str(), attr.content().substr(1));
			}

			if (!rel.empty() && !href.empty())
			{
				std::istringstream ss(rel);
				while (ss >> rel)
				{
					std::tr1::shared_ptr<const rdf::detail::resource> type = rdfa(rel);
					const rdf::uri *uri = dynamic_cast<const rdf::uri *>(type.get());
					if (uri)
					{
						if (!id.empty())
						{
							const rdf::uri base(aSubject.str(), id);
							events.metadata(rdf::statement(about, *uri, base));
							events.metadata(rdf::statement(base, rdf::rdf("value"), href));
						}
						else
							events.metadata(rdf::statement(about, *uri, href));
					}
				}
			}
		}
		else if (node.namespaceURI() == rdf::dc)
		{
			bool preferOther = false;
			for (xml::attribute attr = node.firstAttribute(); attr.isValid(); attr.next())
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
					const rdf::uri temp = events.genid();
					events.metadata(rdf::statement(aSubject, predicate, temp));
					events.metadata(rdf::statement(temp, rdf::rdf("value"), rdf::literal(value, lang)));
					if (!role.empty())
						events.metadata(rdf::statement(temp, rdf::opf("role"), rdf::literal(role)));
					if (!fileas.empty())
						events.metadata(rdf::statement(temp, rdf::opf("file-as"), rdf::literal(fileas)));
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
					const rdf::uri temp = events.genid();
					events.metadata(rdf::statement(aSubject, predicate, temp));
					events.metadata(rdf::statement(temp, rdf::rdf("value"), rdf::literal(value, lang)));
					events.metadata(rdf::statement(temp, rdf::opf("event"), rdf::literal(event)));
					continue;
				}
			}
			else if (!strcmp(node.name(), "identifier"))
			{
				std::string scheme = node.attr(rdf::opf("scheme")).content();
				if (!scheme.empty())
				{
					const rdf::uri temp = events.genid();
					events.metadata(rdf::statement(aSubject, predicate, temp));
					events.metadata(rdf::statement(temp, rdf::rdf("value"), rdf::literal(value, lang)));
					events.metadata(rdf::statement(temp, rdf::opf("scheme"), rdf::literal(scheme)));
					continue;
				}
			}

			events.metadata(rdf::statement(aSubject, predicate, rdf::literal(value, lang)));
		}
	}
}

void parseOpfManifest(const xml::node &opf, const rdf::uri &subject, std::map<std::string, cainteoir::fileinfo> &aItemSet)
{
	for (xml::node node = opf.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE && node == rdf::opf("item"))
		{
			std::string id;
			std::string href;
			std::string mediatype;

			for (xml::attribute attr = node.firstAttribute(); attr.isValid(); attr.next())
			{
				if (!strcmp(attr.name(), "id"))
					id = attr.content();
				else if (!strcmp(attr.name(), "href"))
					href = attr.content();
				else if (!strcmp(attr.name(), "media-type"))
					mediatype = attr.content();
			}

			aItemSet[id] = cainteoir::fileinfo(href, mediatype, id);
		}
	}
}

void parseOpfSpine(const xml::node &opf, const rdf::uri &subject, std::list<std::string> &aSpine)
{
	for (xml::node node = opf.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE && node == rdf::opf("itemref"))
		{
			for (xml::attribute attr = node.firstAttribute(); attr.isValid(); attr.next())
			{
				if (!strcmp(attr.name(), "idref"))
					aSpine.push_back(attr.content());
			}
		}
	}
}

void cainteoir::parseOpfDocument(const xml::node &opf, const rdf::uri &subject, cainteoir::document_events &events, opffiles &aOpfFiles)
{
	if (opf != rdf::opf("package"))
		throw std::runtime_error(_("OPF file is not of a recognised format."));

	std::string toc;
	std::list<std::string> spine;
	std::map<std::string, fileinfo> files;

	rdf::namespaces rdfa;
	rdfa.set_base(rdf::pkg.href);

	for (xml::attribute attr = opf.firstAttribute(); attr.isValid(); attr.next())
	{
		if (!strcmp(attr.name(), "profile") && attr.content() == "http://www.idpf.org/epub/30/profile/package/")
			rdfa << rdf::ns("dcterms", rdf::dcterms.href) << rdf::media << rdf::xsd;
		else if (!strcmp(attr.name(), "prefix"))
			rdfa.add_prefix(attr.content());
	}

	for (xml::node section = opf.firstChild(); section.isValid(); section.next())
	{
		if (section.type() == XML_ELEMENT_NODE)
		{
			if (section == rdf::opf("metadata"))
				parseOpfMetadata(section, subject, events, rdfa, true);
			else if (section == rdf::opf("manifest"))
				parseOpfManifest(section, subject, files);
			else if (section == rdf::opf("spine"))
			{
				for (xml::attribute attr = section.firstAttribute(); attr.isValid(); attr.next())
				{
					if (!strcmp(attr.name(), "toc"))
						toc = attr.content();
				}
				parseOpfSpine(section, subject, spine);
			}
		}
	}

	aOpfFiles.toc = files[toc];

	foreach_iter(id, spine)
		aOpfFiles.spine.push_back(files[*id]);
}
