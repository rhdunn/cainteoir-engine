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
#include "compatibility.hpp"
#include "i18n.h"

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

struct LanguageData
{
	LanguageData();

	std::map<std::string, std::string> subtags;
	std::map<std::string, lang::tag> extlangs;
};

LanguageData::LanguageData()
{
	rdf::graph data;
	try
	{
		const char *datadir = getenv("CAINTEOIR_DATA_DIR");
		if (!datadir)
			datadir = DATADIR "/" PACKAGE;

		const std::string filename = datadir + std::string("/languages.rdf.gz");
		printf("loading language data from %s\n", filename.c_str());

		auto reader = cainteoir::createDocumentReader(filename.c_str(), data, std::string());
	}
	catch (const std::exception & e)
	{
		printf("error: %s\n", e.what());
	}

	for (auto &language : rql::select(data, rql::predicate == rdf::rdf("type")))
	{
		rql::results statements = rql::select(data, rql::subject == rql::subject(language));
		auto id     = rql::select_value<std::string>(statements, rql::predicate == rdf::rdf("value"));
		auto name   = rql::select_value<std::string>(statements, rql::predicate == rdf::dcterms("title"));
		auto prefix = rql::select_value<std::string>(statements, rql::predicate == rdf::iana("prefix"));

		subtags[id] = name;
		if (rql::object(language) == rdf::iana("ExtLang"))
			extlangs.insert({ id, { prefix, id }});
	}
}

static LanguageData *language_data()
{
	static std::shared_ptr<LanguageData> data;
	if (!data.get())
		data = std::make_shared<LanguageData>();
	return data.get();
}

static const char *localize_subtag(const char *iso_codes, const std::string &id)
{
	auto data = language_data();
	auto entry = data->subtags.find(id);
	if (entry == data->subtags.end())
		return id.c_str();
	return dgettext(iso_codes, entry->second.c_str());
}

static const std::initializer_list<std::pair<std::string, const lang::tag>> alias_tags = {
	{ "art-lojban",  { "jbo" } },
	{ "be@latin",    { "be", "", "Latn" } },
	{ "ca@valencia", { "ca", "", "", "", "valencia" } },
	{ "cel-gaulish", { "cel-gaulish" } }, // parent=cel, children=[xtg, xcg, xlp, xga]
	{ "en-gb-oed",   { "en", "", "", "GB" } },
	{ "en-sc",       { "en", "", "", "", "scotland" } },
	{ "en-uk",       { "en", "", "", "GB" } },
	{ "en-uk-north", { "en", "", "", "GB" } },
	{ "en-uk-rp",    { "en", "", "", "GB" } },
	{ "en-uk-wmids", { "en", "", "", "GB" } },
	{ "en-wi",       { "en", "", "", "029" } }, // Caribbean
	{ "en@boldquot", { "en" } },
	{ "en@quot",     { "en" } },
	{ "en@shaw",     { "en" } },
	{ "es-la",       { "es", "", "", "419" } }, // Latin America & Caribbean
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
	{ "sgn-be-fr",   { "sfb" } },
	{ "sgn-be-nl",   { "vgt" } },
	{ "sgn-ch-de",   { "sgg" } },
	{ "sr@ije",      { "sr" } },
	{ "sr@latin",    { "sr", "", "Latn" } },
	{ "sr@latn",     { "sr", "", "Latn" } },
	{ "uz@cyrillic", { "uz", "", "Cyrl" } },
	{ "zh-guoyu",    { "zh", "cmn" } },
	{ "zh-hakka",    { "zh", "hak" } },
	{ "zh-min",      { "zh", "nan" } },
	{ "zh-min-nan",  { "zh", "nan" } },
	{ "zh-xiang",    { "zh", "hsn" } },
};

static const lang::tag *lookup_alias(std::string lang)
{
	lang = to_lower(lang);

	int begin = 0;
	int end = alias_tags.size() - 1;

	while (begin <= end)
	{
		int pos = (begin + end) / 2;

		int comp = lang.compare((alias_tags.begin() + pos)->first);
		if (comp == 0)
			return &(alias_tags.begin() + pos)->second;
		else if (comp > 0)
			begin = pos + 1;
		else
			end = pos - 1;
	}

	return nullptr;
}

static const lang::tag *lookup_extlang(std::string lang)
{
	auto data = language_data();
	auto entry = data->extlangs.find(to_lower(lang));
	if (entry == data->extlangs.end())
		return nullptr;
	return &entry->second;
}

/** @brief Extract language tag information from a BCP 47 language id.
  * @see   http://www.ietf.org/rfc/rfc5646.txt
  *
  * @param[in] code The language identifier, e.g. "es-MX".
  *
  * @return The extracted language, script and country codes.
  */
lang::tag lang::make_lang(const std::string &code)
{
	const lang::tag *alias = lookup_alias(code);
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
		{
			const lang::tag *extlang = lookup_extlang(item);
			if (extlang)
				lang = *extlang;
			else
				lang.lang = item;
		}
		else switch (item.length())
		{
		case 4:
			if (lang.script.empty())
				lang.script = item;
			else if (lang.variant.empty())
				lang.variant = item;
			break;
		case 3:
			if (lang.extlang.empty())
			{
				const lang::tag *extlang = lookup_extlang(item);
				if (extlang && extlang->lang == lang.lang)
					lang.extlang = extlang->extlang;
				else
					lang.region = item;
			}
			else
				lang.region = item;
			break;
		case 2:
			lang.region = item;
			break;
		case 1:
			if (item == "*")
			{
				if (lang.script.empty())
					lang.script = item;
			}
			else
				lang.variant = item;
			break;
		default:
			lang.variant = item;
			break;
		}

		a = b+1;
		++n;
	} while (b != std::string::npos);

	return { to_lower(lang.lang),
	         to_lower(lang.extlang),
	         capitalize(lang.script),
	         to_upper(lang.region),
	         lang.variant };
}

/** @brief Compare two language tags for equality.
  *
  * @param[in] a The fist language tag to compare.
  * @param[in] b The second language tag to compare.
  *
  * @retval true  If the language tags match.
  * @retval false If the language tags do not match.
  */
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
		else if (a.script == "*" || b.script == "*")
			return a.lang == b.lang && a.region == b.region;
		return a.lang == b.lang && a.script == b.script && a.region == b.region;
	}
	return a.lang == b.lang && a.script == b.script && a.region == b.region && a.variant == b.variant;
}

/** @struct cainteoir::languages
  * @brief  Helper for localizing language tags.
  */

/** @brief Get the translated ISO 639 language code.
  *
  * @param[in] id The localized tag to localize.
  *
  * @return The localized name.
  */
const char *cainteoir::languages::language(const lang::tag &id) const
{
	if (!id.extlang.empty())
		return localize_subtag("iso_639", id.extlang);
	return localize_subtag("iso_639", id.lang);
}

/** @brief Get the localized ISO 15924 script code.
  *
  * @param[in] id The language tag to localize.
  *
  * @return The localized name.
  */
const char *cainteoir::languages::script(const lang::tag &id) const
{
	return localize_subtag("iso_15924", id.script);
}

/** @brief Get the localized ISO 3166 region code.
  *
  * @param[in] id The language tag to localize.
  *
  * @return The localized name.
  */
const char *cainteoir::languages::region(const lang::tag &id) const
{
	return localize_subtag("iso_3166", id.region);
}

/** @brief Get the localized name of the language.
  *
  * @param[in] langid The language tag to localize.
  *
  * @return The localized name.
  */
std::string cainteoir::languages::operator()(const std::string & langid)
{
	lang::tag lang = lang::make_lang(langid);

	std::ostringstream name;
	name << language(lang);
	if (!lang.region.empty())
		name << " (" << region(lang) << ")";

	return name.str();
}

/** @struct cainteoir::language::tag
  * @brief  BCP 47 / RFC 5646 language tag.
  * @see    http://www.ietf.org/rfc/rfc5646.txt
  */
 
/** @fn    cainteoir::language::tag::tag(const std::string &l, const std::string &e, const std::string &s, const std::string &r, const std::string &v)
  * @brief Create a language tag object.
  *
  * @param[in] l The primary language code.
  * @param[in] e The extended language code.
  * @param[in] s The writing script code.
  * @param[in] r The region code.
  * @param[in] v The variant code.
  */

/** @var   cainteoir::language::tag::lang
  * @brief ISO 639 language code (primary language).
  */

/** @var   cainteoir::language::tag::extlang
  * @brief ISO 639 language code (extended language).
  */

/** @var   cainteoir::language::tag::script
  * @brief ISO 15924 script code.
  */

/** @var   cainteoir::language::tag::region
  * @brief ISO 3166 or UN M.49 region code.
  */

/** @var   cainteoir::language::tag::variant
  * @brief IANA variant subtag.
  */
