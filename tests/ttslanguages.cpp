/* Tests for mapping TTS language codes to BCP47 / RFC4646 / RFC5646.
 *
 * Copyright (C) 2013 Reece H. Dunn
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

#include <cainteoir/metadata.hpp>
#include <cainteoir/document.hpp>
#include <cainteoir/engines.hpp>
#include <cainteoir/locale.hpp>
#include <iostream>
#include <cassert>
#include <cstdlib>

#include "tester.hpp"

namespace lang = cainteoir::language;
namespace rdf  = cainteoir::rdf;
namespace rql  = cainteoir::rdf::query;
namespace tts  = cainteoir::tts;

void get_codes(std::set<std::string> &codes, rdf::graph &data, const rdf::uri &type)
{
	for (auto &code : rql::select(data,
	                              rql::predicate == rdf::rdf("type") && rql::object == type))
	{
		for (auto &statement : rql::select(data, rql::subject == rql::subject(code)))
		{
			if (rql::predicate(statement) == rdf::iana("code"))
				codes.insert(rql::value(statement));
		}
	}
}

std::set<std::string> get_codes(rdf::graph &data, const rdf::uri &type)
{
	std::set<std::string> codes;
	get_codes(codes, data, type);
	return codes;
}

void contains_(const std::set<std::string> &codes, const std::string &code, const char *file, int line)
{
	std::string found;
	if (codes.find(code) != codes.end())
		found = code;
	assert_location(code == found, file, line);
}

#define contains(codes, code) contains_(codes, code, __FILE__, __LINE__)

REGISTER_TESTSUITE("TTS Languages");

TEST_CASE("language codes")
{
	rdf::graph data;
	cainteoir::createDocumentReader("data/languages.rdf.gz", data, std::string());

	auto languages = get_codes(data, rdf::iana("Language"));
	get_codes(languages, data, rdf::iana("MacroLanguage"));
	auto extlangs = get_codes(data, rdf::iana("ExtLang"));
	auto scripts = get_codes(data, rdf::iana("Script"));
	auto regions = get_codes(data, rdf::iana("Region"));
	auto variants = get_codes(data, rdf::iana("Variant"));
	auto private_use = get_codes(data, rdf::iana("Private"));

	rdf::graph metadata;
	cainteoir::tts::engines tts(metadata);

	for (auto &voice : rql::select(metadata,
	                               rql::predicate == rdf::rdf("type") && rql::object == rdf::tts("Voice")))
	{
		std::string language;
		std::string name;
		for (auto &statement : rql::select(metadata, rql::subject == rql::subject(voice)))
		{
			if (rql::predicate(statement) == rdf::dc("language"))
				language = rql::value(statement);
			else if (rql::predicate(statement) == rdf::tts("name"))
				name = rql::value(statement);
		}

		printf("... ... testing voice %s language %s\n", name.c_str(), language.c_str());
		lang::tag code = lang::make_lang(language);

		contains(languages, code.lang);
		contains(extlangs, code.extlang);
		contains(scripts, code.script);
		contains(regions, code.region);
		contains(variants, code.variant);
		contains(private_use, code.private_use);
	}

}
