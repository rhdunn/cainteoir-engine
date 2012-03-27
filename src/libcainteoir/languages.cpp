/* Cainteoir Engine.
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

#include <cainteoir/languages.hpp>
#include <cainteoir/document.hpp>
#include <algorithm>
#include <libintl.h>

namespace rdf  = cainteoir::rdf;
namespace rql  = cainteoir::rdf::query;
namespace lang = cainteoir::language;

static std::string to_upper(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(), ::toupper);
	return s;
}

static std::string to_lower(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}

static std::string capitalize(std::string s)
{
	int len = s.length();
	if (len > 0)
	{
		s[0] = toupper(s[0]);
		std::transform(s.begin()+1, s.end(), s.begin()+1, ::tolower);
	}
	return s;
}

std::initializer_list<std::pair<std::string, const lang::tag>> alias_tags = {
	{ "be@latin",    { "be", "Latn" } },
	{ "ca@valencia", { "ca", "", "", "valencia" } },
	{ "en@boldquot", { "en" } },
	{ "en@quot",     { "en" } },
	{ "en@shaw",     { "en" } },
	{ "sr@ije",      { "sr" } },
	{ "sr@latin",    { "sr", "Latn" } },
	{ "sr@latn",     { "sr", "Latn" } },
	{ "uz@cyrillic", { "uz", "Cyrl" } },
	{ "art-lojban",  { "jbo" } },
	{ "cel-gaulish", { "cel-gaulish" } }, // parent=cel, children=[xtg, xcg, xlp, xga]
	{ "en-sc",       { "en", "", "", "scotland" } },
	{ "en-uk",       { "en", "", "GB" } },
	{ "en-wi",       { "en", "", "029" } }, // Caribbean
	{ "es-la",       { "es", "", "419" } }, // Latin America & Caribbean
	{ "hy-west",     { "hy" } },
	{ "i-ami",       { "ami" } },
	{ "i-bnn",       { "bnn" } },
	{ "i-default",   { "und" } },
	{ "i-enochian",  { "i-enochian" } }, // no corresponding preferred tag
	{ "i-hak",       { "hak" } },
	{ "i-klingon",   { "tlh" } },
	{ "i-lux",       { "lb" } },
	{ "i-mingo",     { "i-mingo" } }, // no corresponding preferred tag
	{ "i-navajo",    { "nv" } },
	{ "i-pwn",       { "pwn" } },
	{ "i-tao",       { "tao" } },
	{ "i-tay",       { "tay" } },
	{ "i-tsu",       { "tsu" } },
	{ "no-bok",      { "nb" } },
	{ "no-nyn",      { "nn" } },
	{ "zh-cmn",      { "cmn" } },
	{ "zh-guoyu",    { "cmn" } },
	{ "zh-hak",      { "hak" } },
	{ "zh-hakka",    { "hak" } },
	{ "zh-min",      { "nan" } },
	{ "zh-nan",      { "nan" } },
	{ "zh-xiang",    { "hsn" } },
	{ "zh-yue",      { "yue" } },
	{ "en-gb-oed",   { "en", "", "GB" } },
	{ "en-uk-north", { "en", "", "GB" } },
	{ "en-uk-rp",    { "en", "", "GB" } },
	{ "en-uk-wmids", { "en", "", "GB" } },
	{ "sgn-be-fr",   { "sfb" } },
	{ "sgn-be-nl",   { "vgt" } },
	{ "sgn-ch-de",   { "sgg" } },
	{ "zh-min-nan",  { "nan" } },
};

static const lang::tag *
lookup_lang(std::string lang,
            std::initializer_list<std::pair<std::string, const lang::tag>> &tags)
{
	lang = to_lower(lang);
	for (auto id = tags.begin(), last = tags.end(); id != last; ++id)
	{
		if (id->first == lang)
			return &id->second;
	}
	return NULL;
}

lang::tag lang::make_lang(const std::string &code)
{
	const lang::tag *alias = lookup_lang(code, alias_tags);
	if (alias)
		return *alias;

	lang::tag lang { "" };

	std::string::size_type a = 0;
	std::string::size_type b = 0;
	int n = 0;
	do
	{
		b = code.find('-', a);
		std::string item = (b == std::string::npos) ? code.substr(a) : code.substr(a, b-a);

		if (lang.lang.empty())
			lang.lang = item;
		else switch (item.length())
		{
		case 4:
			lang.script = item;
			break;
		case 3:
		case 2:
			lang.region = item;
			break;
		}

		a = b+1;
		++n;
	} while (b != std::string::npos);

	return { to_lower(lang.lang), capitalize(lang.script), to_upper(lang.region) };
}

bool lang::operator==(const tag &a, const tag &b)
{
	if (a.variant.empty() || b.variant.empty())
	{
		if (a.region.empty() || b.region.empty())
		{
			if (a.script.empty() || b.script.empty())
				return a.lang == b.lang;
			return a.lang == b.lang && a.script == b.script;
		}
		return a.lang == b.lang && a.script == b.script && a.region == b.region;
	}
	return a.lang == b.lang && a.script == b.script && a.region == b.region && a.variant == b.variant;
}

cainteoir::languages::languages(const char * locale)
{
	cainteoir::document_events events;
	rdf::graph data;
	try
	{
		const char * filename = DATADIR "/" PACKAGE "/languages.rdf";
		printf("loading language data from %s\n", filename);

		cainteoir::parseDocument(filename, events, data);
	}
	catch (const std::exception & e)
	{
		printf("error: %s\n", e.what());
	}

	// This should construct the m_subtags map directly from the RDF metadata.
	// However, with the current list-based implementation lookup of sublists
	// (e.g. all statements with a specific subject) is slow when lookup is
	// chained, especially for large statement sets like the subtag registry.
	//
	// This implementation performs a single pass over the data so the performance
	// is O(n) where n is the number of statements, not O(n^2) or O(n^3).

	std::map<std::string, std::pair<std::string, std::string>> mapping;
	foreach_iter(lang, data)
	{
		if (rql::predicate(*lang) == rdf::rdf("value"))
			mapping[rql::subject(*lang).str()].first = rql::value(*lang);
		else if (rql::predicate(*lang) == rdf::dcterms("title"))
			mapping[rql::subject(*lang).str()].second = rql::value(*lang);
	}

	foreach_iter(lang, mapping)
	{
		const auto &entry = lang->second;
		if (!entry.first.empty() && !entry.second.empty())
			m_subtags[entry.first] = entry.second;
	}
}

const char *cainteoir::languages::language(const lang::tag &id) const
{
	auto entry = m_subtags.find(id.lang);
	if (entry == m_subtags.end())
		return id.lang.c_str();
	return dgettext("iso_639", entry->second.c_str());
}

const char *cainteoir::languages::script(const lang::tag &id) const
{
	auto entry = m_subtags.find(id.script);
	if (entry == m_subtags.end())
		return id.script.c_str();
	return dgettext("iso_15924", entry->second.c_str());
}

const char *cainteoir::languages::region(const lang::tag &id) const
{
	auto entry = m_subtags.find(id.region);
	if (entry == m_subtags.end())
		return id.region.c_str();
	return dgettext("iso_3166", entry->second.c_str());
}

std::string cainteoir::languages::operator()(const std::string & langid)
{
	lang::tag lang = lang::make_lang(langid);

	std::ostringstream name;
	name << language(lang);
	if (!lang.region.empty())
		name << " (" << region(lang) << ")";

	return name.str();
}
