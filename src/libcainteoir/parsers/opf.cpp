/* OPF Document Parser.
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

#include "parsers.hpp"
#include <stdexcept>
#include <algorithm>

namespace xml    = cainteoir::xml;
namespace xmlns  = cainteoir::xml::xmlns;
namespace rdf    = cainteoir::rdf;
namespace events = cainteoir::events;

namespace dc
{
	static const xml::context::entry contributor_node = {};
	static const xml::context::entry coverage_node    = {};
	static const xml::context::entry creator_node     = {};
	static const xml::context::entry date_node        = {};
	static const xml::context::entry description_node = {};
	static const xml::context::entry format_node      = {};
	static const xml::context::entry identifier_node  = {};
	static const xml::context::entry language_node    = {};
	static const xml::context::entry publisher_node   = {};
	static const xml::context::entry relation_node    = {};
	static const xml::context::entry rights_node      = {};
	static const xml::context::entry source_node      = {};
	static const xml::context::entry subject_node     = {};
	static const xml::context::entry title_node       = {};
	static const xml::context::entry type_node        = {};
}

namespace opf
{
	static const xml::context::entry dcmetadata_node = {};
	static const xml::context::entry item_node       = {};
	static const xml::context::entry itemref_node    = {};
	static const xml::context::entry link_node       = {};
	static const xml::context::entry manifest_node   = {};
	static const xml::context::entry meta_node       = {};
	static const xml::context::entry metadata_node   = {};
	static const xml::context::entry package_node    = {};
	static const xml::context::entry spine_node      = {};

	static const xml::context::entry about_attr         = {};
	static const xml::context::entry content_attr       = {};
	static const xml::context::entry datatype_attr      = {};
	static const xml::context::entry event_attr         = {};
	static const xml::context::entry fileas_attr        = {};
	static const xml::context::entry href_attr          = {};
	static const xml::context::entry id_attr            = {};
	static const xml::context::entry idref_attr         = {};
	static const xml::context::entry media_overlay_attr = {};
	static const xml::context::entry mediatype_attr     = {};
	static const xml::context::entry name_attr          = {};
	static const xml::context::entry prefer_attr        = {};
	static const xml::context::entry prefix_attr        = {};
	static const xml::context::entry properties_attr    = {};
	static const xml::context::entry property_attr      = {};
	static const xml::context::entry refines_attr       = {};
	static const xml::context::entry rel_attr           = {};
	static const xml::context::entry role_attr          = {};
	static const xml::context::entry scheme_attr        = {};
	static const xml::context::entry toc_attr           = {};
	static const xml::context::entry version_attr       = {};
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
	{ "about",         &opf::about_attr },
	{ "content",       &opf::content_attr },
	{ "datatype",      &opf::datatype_attr },
	{ "event",         &opf::event_attr },
	{ "file-as",       &opf::fileas_attr },
	{ "href",          &opf::href_attr },
	{ "id",            &opf::id_attr },
	{ "idref",         &opf::idref_attr },
	{ "media-overlay", &opf::media_overlay_attr },
	{ "media-type",    &opf::mediatype_attr },
	{ "name",          &opf::name_attr },
	{ "prefer",        &opf::prefer_attr },
	{ "prefix",        &opf::prefix_attr },
	{ "properties",    &opf::properties_attr },
	{ "property",      &opf::property_attr },
	{ "refines",       &opf::refines_attr },
	{ "rel",           &opf::rel_attr },
	{ "role",          &opf::role_attr },
	{ "scheme",        &opf::scheme_attr },
	{ "toc",           &opf::toc_attr },
	{ "version",       &opf::version_attr },
};

static void parseOpfMeta(xml::reader &reader, const rdf::uri &aSubject, rdf::uri &aCalibreSeries, rdf::graph &aGraph)
{
	std::string custom;
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
			auto meta = reader.nodeValue().str();
			std::shared_ptr<const rdf::uri> uri = aGraph.curie(meta);
			if (uri.get() && !uri->ns.empty())
				name = *uri;
			else if (meta.find("calibre:") == 0)
				custom = meta;
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
			{
				aGraph.statement(aSubject, name, rdf::literal(content));
			}
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
			else if (!custom.empty() && !content.empty())
			{
				if (custom == "calibre:series")
				{
					if (aCalibreSeries.empty())
						aCalibreSeries = aGraph.genid();
					aGraph.statement(aSubject, rdf::pkg("belongs-to-collection"), aCalibreSeries);
					aGraph.statement(aCalibreSeries, rdf::rdf("value"), rdf::literal(content));
					aGraph.statement(aCalibreSeries, rdf::pkg("collection-type"), rdf::literal("series"));
				}
				else if (custom == "calibre:series_index")
				{
					if (aCalibreSeries.empty())
						aCalibreSeries = aGraph.genid();
					int index = (int)strtof(content.c_str(), nullptr);
					aGraph.statement(aCalibreSeries, rdf::pkg("group-position"), rdf::literal(index));
				}
			}
			return;
		}
		break;
	default:
		break;
	}
}

static void parseOpfLink(xml::reader &reader, const rdf::uri &aSubject, rdf::graph &aGraph)
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
				aGraph.curie_list(rel, [&](const rdf::uri &aUri)
				{
					if (!id.empty())
					{
						const rdf::uri base(aSubject.str(), id);
						aGraph.statement(about, aUri, base);
						aGraph.statement(base, rdf::rdf("value"), href);
					}
					else
						aGraph.statement(about, aUri, href);
				});
			}
			return;
		}
		break;
	default:
		break;
	}
}

static void parseOpfDublinCore(xml::reader &reader, const rdf::uri &aSubject, rdf::graph &aGraph, const xml::context::entry *ctx)
{
	std::string id;
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
		else if (reader.context() == &opf::id_attr)
			id = reader.nodeValue().str();
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
			if ((reader.context() == &dc::contributor_node || reader.context() == &dc::creator_node) && (!role.empty() || !fileas.empty() || !id.empty()))
			{
				rdf::uri temp;
				if (id.empty())
					temp = aGraph.genid();
				else
					temp = rdf::uri(aSubject.str(), id);

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
			else if (reader.context() == &dc::identifier_node && (!scheme.empty() || !id.empty()))
			{
				rdf::uri temp;
				if (id.empty())
					temp = aGraph.genid();
				else
					temp = rdf::uri(aSubject.str(), id);

				aGraph.statement(aSubject, predicate, temp);
				aGraph.statement(temp, rdf::rdf("value"), rdf::literal(value, lang));
				if (!scheme.empty())
					aGraph.statement(temp, rdf::opf("scheme"), rdf::literal(scheme));
				return;
			}
			else if (reader.context() == &dc::subject_node)
			{
				std::string::size_type comma   = value.find(',');
				std::string::size_type divider = value.find(" -- ");
				if (comma != std::string::npos && divider == std::string::npos)
				{
					std::string::const_iterator a = value.begin();
					std::string::const_iterator b = value.begin();
					while (a != value.end())
					{
						a = b;
						while (a != value.end() && (*a == ',' || *a == ' '))
							++a;

						b = a;
						while (b != value.end() && *b != ',')
							++b;

						if (b != value.end())
						{
							while (b != a && (*b == ',' || *b == ' '))
								--b;
							++b;
						}

						if (a != b)
							aGraph.statement(aSubject, predicate, rdf::literal(std::string(a, b), lang));
					}
					return;
				}
			}
			aGraph.statement(aSubject, predicate, rdf::literal(value, lang));
			return;
		}
		break;
	default:
		break;
	}
}

static void parseOpfMetadata(xml::reader &reader, const rdf::uri &aSubject, rdf::graph &aGraph, const xml::context::entry *ctx)
{
	rdf::uri aCalibreSeries;
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
				parseOpfMetadata(reader, aSubject, aGraph, reader.context());
			else if (reader.context() == &opf::meta_node)
				parseOpfMeta(reader, aSubject, aCalibreSeries, aGraph);
			else if (reader.context() == &opf::link_node)
				parseOpfLink(reader, aSubject, aGraph);
			else if (reader.namespaceUri() == xmlns::dc.href && reader.context() != &xml::unknown_context)
				parseOpfDublinCore(reader, aSubject, aGraph, reader.context());
		}
		else if (reader.namespaceUri() == xmlns::dc.href && reader.context() != &xml::unknown_context)
			parseOpfDublinCore(reader, aSubject, aGraph, reader.context());
		break;
	case xml::reader::endTagNode:
		if (reader.context() == ctx)
			return;
		break;
	default:
		break;
	}
}

struct opf_document_reader : public cainteoir::document_reader
{
	opf_document_reader(const std::shared_ptr<xml::reader> &aReader, const rdf::uri &aSubject, rdf::graph &aPrimaryMetadata, const char *aMimeType);

	void read_package(rdf::graph &aMetadata);

	bool read(rdf::graph *aMetadata);

	void add_spine_item(rdf::graph *aMetadata, const rdf::uri &location);

	std::shared_ptr<xml::reader> mReader;
	rdf::uri mSubject;
	const xml::context::entry *mContext;
	rdf::uri mCurrentReference;
	bool mIsFirst;
};

opf_document_reader::opf_document_reader(const std::shared_ptr<xml::reader> &aReader, const rdf::uri &aSubject, rdf::graph &aPrimaryMetadata, const char *aMimeType)
	: mReader(aReader)
	, mSubject(aSubject)
	, mContext(nullptr)
	, mCurrentReference(aSubject)
	, mIsFirst(true)
{
	mReader->set_nodes(xmlns::opf, opf_nodes);
	mReader->set_attrs(xmlns::opf, opf_attrs);
	mReader->set_nodes(xmlns::dc,  dc_nodes);
	mReader->set_attrs(xmlns::xml, xml::attrs);

	aPrimaryMetadata.set_base(rdf::pkg.href);
	read_package(aPrimaryMetadata);
	aPrimaryMetadata.statement(mSubject, rdf::tts("mimetype"), rdf::literal(aMimeType));
}

void opf_document_reader::read_package(rdf::graph &aMetadata)
{
	while (mReader->read()) switch (mReader->nodeType())
	{
	case xml::reader::attribute:
		if (mReader->context() == &opf::version_attr && mReader->nodeValue().str() == "3.0")
			aMetadata
				<< rdf::ns("dcterms", rdf::dcterms.href)
				<< rdf::media
				<< rdf::onix
				<< rdf::marc
				<< rdf::xsd;
		else if (mReader->context() == &opf::prefix_attr)
			aMetadata.add_prefix(mReader->nodeValue().str());
		break;
	case xml::reader::beginTagNode:
		if (mReader->context() == &opf::metadata_node)
			parseOpfMetadata(*mReader, mSubject, aMetadata, mReader->context());
		else if (mReader->context() == &opf::spine_node ||
		         mReader->context() == &opf::manifest_node)
		{
			mContext = mReader->context();
			return;
		}
		break;
	default:
		break;
	}
}

bool opf_document_reader::read(rdf::graph *aMetadata)
{
	std::string id;
	std::string href;
	std::shared_ptr<cainteoir::buffer> mediatype;
	std::string media_overlay;
	std::string properties;
	std::string toc;

	while (mReader->read()) switch (mReader->nodeType())
	{
	case xml::reader::attribute:
		if (mContext == &opf::spine_node)
		{
			if (mReader->context() == &opf::toc_attr)
			{
				if (toc.empty())
					toc = mReader->nodeValue().str();
			}
		}
		else if (mContext == &opf::item_node) // manifest > item
		{
			if (mReader->context() == &opf::href_attr)
				href = mReader->nodeValue().str();
			else if (mReader->context() == &opf::id_attr)
				id = mReader->nodeValue().str();
			else if (mReader->context() == &opf::mediatype_attr)
				mediatype = mReader->nodeValue().buffer();
			else if (mReader->context() == &opf::media_overlay_attr)
				media_overlay = mReader->nodeValue().str();
			else if (mReader->context() == &opf::properties_attr)
				properties = mReader->nodeValue().str();
		}
		else if (mContext == &opf::itemref_node) // spine > itemref
		{
			if (mReader->context() == &opf::idref_attr)
			{
				std::string id = mReader->nodeValue().str();
				add_spine_item(aMetadata, rdf::uri(mSubject.str(), id));
			}
		}
		else if (mReader->context() == &opf::spine_node ||
		         mReader->context() == &opf::manifest_node)
			mContext = mReader->context();
		break;
	case xml::reader::beginTagNode:
		if (mContext == &opf::manifest_node)
		{
			if (mReader->context() == &opf::item_node)
				mContext = &opf::item_node;
		}
		else if (mContext == &opf::spine_node)
		{
			if (mReader->context() == &opf::itemref_node)
				mContext = &opf::itemref_node;
		}
		else if (mContext == nullptr)
		{
			if (mReader->context() == &opf::spine_node ||
			    mReader->context() == &opf::manifest_node)
				mContext = mReader->context();
		}
		break;
	case xml::reader::endTagNode:
		if (mContext == &opf::spine_node ||
		    mContext == &opf::manifest_node)
			mContext = nullptr;
		else if (mContext == &opf::item_node)
		{
			if (aMetadata)
			{
				rdf::uri item(mSubject.str(), id);
				aMetadata->statement(mSubject, rdf::ref("hasManifestItem"), item);
				aMetadata->statement(item, rdf::rdf("type"), rdf::ref("ManifestItem"));
				aMetadata->statement(item, rdf::ref("target"), rdf::href(href));
				aMetadata->statement(item, rdf::ref("mimetype"), rdf::literal(mediatype->str()));
				if (!media_overlay.empty())
				{
					rdf::uri overlay(mSubject.str(), media_overlay);
					aMetadata->statement(item, rdf::ref("media-overlay"), overlay);
				}
				aMetadata->curie_list(properties,
				                      [&aMetadata, &item, &toc, &id](const rdf::uri &aUri)
				{
					aMetadata->statement(item, rdf::ref("property"), aUri);
					if (aUri == rdf::pkg("nav"))
						toc = id;
				});

				id = {};
				href = {};
				mediatype = {};
				media_overlay = {};
				properties = {};
			}
			mContext = &opf::manifest_node;
		}
		else if (mContext == &opf::itemref_node)
			mContext = &opf::spine_node;
		break;
	default:
		break;
	}

	if (aMetadata)
	{
		if (!toc.empty())
			aMetadata->statement(mSubject, rdf::ref("toc"), rdf::uri(mSubject.str(), toc));
		if (!mIsFirst)
			aMetadata->statement(mCurrentReference, rdf::rdf("rest"), rdf::rdf("nil"));
	}
	return false;
}

void opf_document_reader::add_spine_item(rdf::graph *aMetadata, const rdf::uri &location)
{
	if (!aMetadata) return;

	if (mIsFirst)
	{
		mCurrentReference = aMetadata->genid();
		aMetadata->statement(mSubject, rdf::rdf("type"), rdf::ref("Manifest"));
		aMetadata->statement(mSubject, rdf::ref("spine"), mCurrentReference);
		mIsFirst = false;
	}
	else
	{
		const rdf::uri next = aMetadata->genid();
		aMetadata->statement(mCurrentReference, rdf::rdf("rest"), next);
		mCurrentReference = next;
	}

	aMetadata->statement(mCurrentReference, rdf::rdf("first"), location);
}

std::shared_ptr<cainteoir::document_reader>
cainteoir::createOpfReader(const std::shared_ptr<xml::reader> &aReader,
                           const rdf::uri &aSubject,
                           rdf::graph &aPrimaryMetadata,
                           const char *aMimeType)
{
	if (!aReader)
		return std::shared_ptr<document_reader>();

	return std::make_shared<opf_document_reader>(aReader, aSubject, aPrimaryMetadata, aMimeType);
}
