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

namespace rdf = cainteoir::rdf;
namespace xml = cainteoir::xmldom;

void parseOpfMetadata(const xml::node &opf, const rdf::uri &subject, rdf::graph &metadata, bool recurse)
{
	for (xml::node node = opf.firstChild(); node.isValid(); node.next())
	{
		if (node.type() != XML_ELEMENT_NODE)
			continue;

		if (node == rdf::opf("dc-metadata") && recurse)
			parseOpfMetadata(node, subject, metadata, false);
		else if (node.namespaceURI() == rdf::dc)
		{
			std::string lang = node.attr(rdf::xml("lang")).content();
			std::string value = node.content()->str();

			const rdf::uri predicate = rdf::uri(node.namespaceURI(), node.name());

			if (!strcmp(node.name(), "creator") || !strcmp(node.name(), "contributor"))
			{
				std::string role = node.attr(rdf::opf("role")).content();
				std::string fileas = node.attr(rdf::opf("file-as")).content();
				if (!role.empty() || !fileas.empty())
				{
					const rdf::bnode temp = metadata.genid();
					metadata.push_back(rdf::statement(subject, predicate, temp));
					metadata.push_back(rdf::statement(temp, rdf::rdf("value"), rdf::literal(value, lang)));
					if (!role.empty())
						metadata.push_back(rdf::statement(temp, rdf::opf("role"), rdf::literal(role)));
					if (!fileas.empty())
						metadata.push_back(rdf::statement(temp, rdf::opf("file-as"), rdf::literal(fileas)));
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
					const rdf::bnode temp = metadata.genid();
					metadata.push_back(rdf::statement(subject, predicate, temp));
					metadata.push_back(rdf::statement(temp, rdf::rdf("value"), rdf::literal(value, lang)));
					metadata.push_back(rdf::statement(temp, rdf::opf("event"), rdf::literal(event)));
					continue;
				}
			}
			else if (!strcmp(node.name(), "identifier"))
			{
				std::string scheme = node.attr(rdf::opf("scheme")).content();
				if (!scheme.empty())
				{
					const rdf::bnode temp = metadata.genid();
					metadata.push_back(rdf::statement(subject, predicate, temp));
					metadata.push_back(rdf::statement(temp, rdf::rdf("value"), rdf::literal(value, lang)));
					metadata.push_back(rdf::statement(temp, rdf::opf("scheme"), rdf::literal(scheme)));
					continue;
				}
			}

			metadata.push_back(rdf::statement(subject, predicate, rdf::literal(value, lang)));
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

void cainteoir::parseOpfDocument(const xml::node &opf, const rdf::uri &subject, rdf::graph &metadata, opffiles &aOpfFiles)
{
	if (opf != rdf::opf("package"))
		throw std::runtime_error("OPF file is not of a recognised format.");

	std::string toc;
	std::list<std::string> spine;
	std::map<std::string, fileinfo> files;

	for (xml::node section = opf.firstChild(); section.isValid(); section.next())
	{
		if (section.type() == XML_ELEMENT_NODE)
		{
			if (section == rdf::opf("metadata"))
				parseOpfMetadata(section, subject, metadata, true);
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
