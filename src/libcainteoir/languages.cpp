/* Cainteoir Engine.
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

#include "config.h"

#include <cainteoir/languages.hpp>
#include <cainteoir/document.hpp>
#include <algorithm>

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

std::initializer_list<std::pair<std::string, lang::tag>> alias2tag = {
	{ "art-lojban",  { "jbo", "", "" } },
	{ "cel-gaulish", { "cel-gaulish", "", "" } }, // parent=cel, children=[xtg, xcg, xlp, xga]
	{ "i-ami",       { "ami", "", "" } },
	{ "i-bnn",       { "bnn", "", "" } },
	{ "i-default",   { "und", "", "" } },
	{ "i-enochian",  { "i-enochian", "", "" } }, // no corresponding preferred tag
	{ "i-hak",       { "hak", "", "" } },
	{ "i-klingon",   { "tlh", "", "" } },
	{ "i-lux",       { "lb",  "", "" } },
	{ "i-mingo",     { "i-mingo",  "", "" } }, // no corresponding preferred tag
	{ "i-navajo",    { "nv",  "", "" } },
	{ "i-pwn",       { "pwn", "", "" } },
	{ "i-tao",       { "tao", "", "" } },
	{ "i-tay",       { "tay", "", "" } },
	{ "i-tsu",       { "tsu", "", "" } },
	{ "no-bok",      { "nb",  "", "" } },
	{ "no-nyn",      { "nn",  "", "" } },
	{ "zh-cmn",      { "cmn", "", "" } },
	{ "zh-guoyu",    { "cmn", "", "" } },
	{ "zh-hak",      { "hak", "", "" } },
	{ "zh-hakka",    { "hak", "", "" } },
	{ "zh-min",      { "nan", "", "" } },
	{ "zh-nan",      { "nan", "", "" } },
	{ "zh-xiang",    { "hsn", "", "" } },
};

std::initializer_list<std::pair<std::string, lang::tag>> alias3tag = {
	{ "en-gb-oed",  { "en",  "", "GB" } },
	{ "sgn-be-fr",  { "sfb", "", "" } },
	{ "sgn-be-nl",  { "vgt", "", "" } },
	{ "sgn-ch-de",  { "sgg", "", "" } },
	{ "zh-min-nan", { "nan", "", "" } },
};

lang::tag lang::make_lang(const std::string &lang)
{
	std::string::size_type a = lang.find('-');
	if (a == std::string::npos)
		return { to_lower(lang), "", "" };

	std::string::size_type b = lang.find('-', a+1);
	if (b == std::string::npos)
	{
		std::string subtag = lang.substr(a+1);

		std::string langid = to_lower(lang);
		for (auto id = alias2tag.begin(), last = alias2tag.end(); id != last; ++id)
		{
			if (id->first == langid)
				return id->second;
		}

		if (subtag.length() == 4)
			return { to_lower(lang.substr(0, a)), capitalize(subtag), "" };

		return { to_lower(lang.substr(0, a)), "", to_upper(subtag) };
	}

	std::string langid = to_lower(lang);
	for (auto id = alias3tag.begin(), last = alias3tag.end(); id != last; ++id)
	{
		if (id->first == langid)
			return id->second;
	}

	return { to_lower(lang.substr(0, a)), capitalize(lang.substr(a+1, b-a-1)), to_upper(lang.substr(b+1)) };
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

	rql::results languages = rql::select(data,
		rql::both(rql::matches(rql::predicate, rdf::rdf("type")),
		          rql::matches(rql::object, rdf::skos("Concept"))));

	foreach_iter(lang, languages)
	{
		const rdf::uri *uri = rql::subject(*lang);
		if (uri)
		{
			rql::results statements = rql::select(data, rql::matches(rql::subject, *uri));
			std::string name = rql::select_value<std::string>(statements, rql::matches(rql::predicate, rdf::skos("prefLabel")));

			foreach_iter(statement, statements)
			{
				if (rql::predicate(*statement) == rdf::skos("altLabel"))
					m_languages[ rql::value(*statement) ] = name;
			}
		}
	}
}

std::string cainteoir::languages::operator()(const std::string & langid)
{
	std::string & lang = m_languages[langid];
	if (lang.empty())
		lang = langid;
	return lang;
}
