/* ePub Document Parser.
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

#include "config.h"
#include "i18n.h"
#include "parsers.hpp"

#include <stdexcept>

namespace rdf    = cainteoir::rdf;
namespace xml    = cainteoir::xml;
namespace events = cainteoir::events;

#ifndef DOXYGEN

static std::string path_to(const std::string &filename, const std::string &opffile)
{
	std::string::size_type pos = opffile.rfind('/');
	if (pos == std::string::npos)
		return filename;
	else
		return opffile.substr(0, pos + 1) + filename;
}

struct epub_document_reader : public cainteoir::document_reader
{
	epub_document_reader(std::shared_ptr<cainteoir::archive> &aData, const rdf::uri &aSubject, rdf::graph &aPrimaryMetadata, const char *aDefaultEncoding);

	bool read();

	std::string opf_file;
	std::shared_ptr<cainteoir::document_reader> opf;
	std::shared_ptr<cainteoir::document_reader> child;
	bool is_toc;

	std::shared_ptr<cainteoir::archive> mData;
	rdf::uri mSubject;
	const char *mDefaultEncoding;
};

epub_document_reader::epub_document_reader(std::shared_ptr<cainteoir::archive> &aData, const rdf::uri &aSubject, rdf::graph &aPrimaryMetadata, const char *aDefaultEncoding)
	: mData(aData)
	, mSubject(aSubject)
	, is_toc(false)
	, mDefaultEncoding(aDefaultEncoding)
{
	auto ocf = cainteoir::createOcfReader(cainteoir::createXmlReader(mData->read("META-INF/container.xml"), mDefaultEncoding));
	if (ocf) while (ocf->read() && opf_file.empty())
	{
		if (!ocf->text->compare("application/oebps-package+xml"))
			opf_file = ocf->anchor.str();
	}

	if (opf_file.empty())
		throw std::runtime_error(i18n("Unsupported ePub content: OPF file not specified."));

	auto reader = cainteoir::createXmlReader(mData->read(opf_file.c_str()), mDefaultEncoding);
	if (!reader)
		throw std::runtime_error(i18n("Unsupported ePub content: OPF file not found."));

	opf = cainteoir::createOpfReader(reader, aSubject, aPrimaryMetadata, "application/epub+zip");
}

bool epub_document_reader::read()
{
	if (child)
	{
		while (child->read())
		{
			if (child->type & cainteoir::events::toc_entry)
			{
				if (is_toc)
				{
					if (child->anchor == mSubject)
						anchor = mSubject;
					else
						anchor = mData->location(path_to(child->anchor.ns, opf_file), child->anchor.ref);
					type   = child->type;
					styles = child->styles;
					text   = child->text;
					return true;
				}
				else
					child->type &= ~cainteoir::events::toc_entry;
			}

			if (child->type)
			{
				type   = child->type;
				styles = child->styles;
				text   = child->text;
				anchor = child->anchor;
				return true;
			}
		}
		child.reset();
	}

	if (opf) while (opf->read())
	{
		if (opf->type & cainteoir::events::toc_entry)
		{
			std::string filename = path_to(opf->anchor.ns, opf_file);
			auto reader = cainteoir::createXmlReader(mData->read(filename.c_str()), mDefaultEncoding);
			if (!reader)
			{
				fprintf(stderr, i18n("document '%s' not found in ePub archive.\n"), filename.c_str());
				continue;
			}

			if (!opf->text->compare("application/x-dtbncx+xml"))
			{
				rdf::graph innerMetadata;
				child = cainteoir::createNcxReader(reader, mSubject, innerMetadata, std::string());
				is_toc = true;
				return read();
			}
			else if (!opf->text->compare("application/xhtml+xml"))
			{
				anchor = mData->location(filename, opf->anchor.ref);

				rdf::graph innerMetadata;
				child = cainteoir::createHtmlReader(reader, anchor, innerMetadata, std::string(), "application/xhtml+xml");
				is_toc = false;
				return read();
			}
		}
	}

	return false;
}

std::shared_ptr<cainteoir::document_reader>
cainteoir::createEpubReader(std::shared_ptr<archive> &aData, const rdf::uri &aSubject, rdf::graph &aPrimaryMetadata, const char *aDefaultEncoding)
{
	return std::make_shared<epub_document_reader>(aData, aSubject, aPrimaryMetadata, aDefaultEncoding);
}

#endif
