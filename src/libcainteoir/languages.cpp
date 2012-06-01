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

std::initializer_list<std::pair<std::string, const lang::tag>> alias_tags = {
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

// TODO: get this information through the RDF metadata (rdf:type iana:ExtLang, rdf:value, iana:prefix).
std::initializer_list<std::pair<std::string, const lang::tag>> extlang_tags = {
	{ "aao", { "ar",  "aao" } },
	{ "abh", { "ar",  "abh" } },
	{ "abv", { "ar",  "abv" } },
	{ "acm", { "ar",  "acm" } },
	{ "acq", { "ar",  "acq" } },
	{ "acw", { "ar",  "acw" } },
	{ "acx", { "ar",  "acx" } },
	{ "acy", { "ar",  "acy" } },
	{ "adf", { "ar",  "adf" } },
	{ "ads", { "sgn", "ads" } },
	{ "aeb", { "ar",  "aeb" } },
	{ "aec", { "ar",  "aec" } },
	{ "aed", { "sgn", "aed" } },
	{ "aen", { "sgn", "aen" } },
	{ "afb", { "ar",  "afb" } },
	{ "afg", { "sgn", "afg" } },
	{ "ajp", { "ar",  "ajp" } },
	{ "apc", { "ar",  "apc" } },
	{ "apd", { "ar",  "apd" } },
	{ "arb", { "ar",  "arb" } },
	{ "arq", { "ar",  "arq" } },
	{ "ars", { "ar",  "ars" } },
	{ "ary", { "ar",  "ary" } },
	{ "arz", { "ar",  "arz" } },
	{ "ase", { "sgn", "ase" } },
	{ "asf", { "sgn", "asf" } },
	{ "asp", { "sgn", "asp" } },
	{ "asq", { "sgn", "asq" } },
	{ "asw", { "sgn", "asw" } },
	{ "auz", { "ar",  "auz" } },
	{ "avl", { "ar",  "avl" } },
	{ "ayh", { "ar",  "ayh" } },
	{ "ayl", { "ar",  "ayl" } },
	{ "ayn", { "ar",  "ayn" } },
	{ "ayp", { "ar",  "ayp" } },
	{ "bbz", { "ar",  "bbz" } },
	{ "bfi", { "sgn", "bfi" } },
	{ "bfk", { "sgn", "bfk" } },
	{ "bjn", { "ms",  "bjn" } },
	{ "bog", { "sgn", "bog" } },
	{ "bqn", { "sgn", "bqn" } },
	{ "bqy", { "sgn", "bqy" } },
	{ "btj", { "ms",  "btj" } },
	{ "bve", { "ms",  "bve" } },
	{ "bvl", { "sgn", "bvl" } },
	{ "bvu", { "ms",  "bvu" } },
	{ "bzs", { "sgn", "bzs" } },
	{ "cdo", { "zh",  "cdo" } },
	{ "cds", { "sgn", "cds" } },
	{ "cjy", { "zh",  "cjy" } },
	{ "cmn", { "zh",  "cmn" } },
	{ "coa", { "ms",  "coa" } },
	{ "cpx", { "zh",  "cpx" } },
	{ "csc", { "sgn", "csc" } },
	{ "csd", { "sgn", "csd" } },
	{ "cse", { "sgn", "cse" } },
	{ "csf", { "sgn", "csf" } },
	{ "csg", { "sgn", "csg" } },
	{ "csl", { "sgn", "csl" } },
	{ "csn", { "sgn", "csn" } },
	{ "csq", { "sgn", "csq" } },
	{ "csr", { "sgn", "csr" } },
	{ "czh", { "zh",  "czh" } },
	{ "czo", { "zh",  "czo" } },
	{ "doq", { "sgn", "doq" } },
	{ "dse", { "sgn", "dse" } },
	{ "dsl", { "sgn", "dsl" } },
	{ "dup", { "ms",  "dup" } },
	{ "ecs", { "sgn", "ecs" } },
	{ "esl", { "sgn", "esl" } },
	{ "esn", { "sgn", "esn" } },
	{ "eso", { "sgn", "eso" } },
	{ "eth", { "sgn", "eth" } },
	{ "fcs", { "sgn", "fcs" } },
	{ "fse", { "sgn", "fse" } },
	{ "fsl", { "sgn", "fsl" } },
	{ "fss", { "sgn", "fss" } },
	{ "gan", { "zh",  "gan" } },
	{ "gom", { "kok", "gom" } },
	{ "gse", { "sgn", "gse" } },
	{ "gsg", { "sgn", "gsg" } },
	{ "gsm", { "sgn", "gsm" } },
	{ "gss", { "sgn", "gss" } },
	{ "gus", { "sgn", "gus" } },
	{ "hab", { "sgn", "hab" } },
	{ "haf", { "sgn", "haf" } },
	{ "hak", { "zh",  "hak" } },
	{ "hds", { "sgn", "hds" } },
	{ "hji", { "ms",  "hji" } },
	{ "hks", { "sgn", "hks" } },
	{ "hos", { "sgn", "hos" } },
	{ "hps", { "sgn", "hps" } },
	{ "hsh", { "sgn", "hsh" } },
	{ "hsl", { "sgn", "hsl" } },
	{ "hsn", { "zh",  "hsn" } },
	{ "icl", { "sgn", "icl" } },
	{ "ils", { "sgn", "ils" } },
	{ "inl", { "sgn", "inl" } },
	{ "ins", { "sgn", "ins" } },
	{ "ise", { "sgn", "ise" } },
	{ "isg", { "sgn", "isg" } },
	{ "isr", { "sgn", "isr" } },
	{ "jak", { "ms",  "jak" } },
	{ "jax", { "ms",  "jax" } },
	{ "jcs", { "sgn", "jcs" } },
	{ "jhs", { "sgn", "jhs" } },
	{ "jls", { "sgn", "jls" } },
	{ "jos", { "sgn", "jos" } },
	{ "jsl", { "sgn", "jsl" } },
	{ "jus", { "sgn", "jus" } },
	{ "kgi", { "sgn", "kgi" } },
	{ "knn", { "kok", "knn" } },
	{ "kvb", { "ms",  "kvb" } },
	{ "kvk", { "sgn", "kvk" } },
	{ "kvr", { "ms",  "kvr" } },
	{ "kxd", { "ms",  "kxd" } },
	{ "lbs", { "sgn", "lbs" } },
	{ "lce", { "ms",  "lce" } },
	{ "lcf", { "ms",  "lcf" } },
	{ "liw", { "ms",  "liw" } },
	{ "lls", { "sgn", "lls" } },
	{ "lsg", { "sgn", "lsg" } },
	{ "lsl", { "sgn", "lsl" } },
	{ "lso", { "sgn", "lso" } },
	{ "lsp", { "sgn", "lsp" } },
	{ "lst", { "sgn", "lst" } },
	{ "lsy", { "sgn", "lsy" } },
	{ "ltg", { "lv",  "ltg" } },
	{ "lvs", { "lv",  "lvs" } },
	{ "lzh", { "zh",  "lzh" } },
	{ "max", { "ms",  "max" } },
	{ "mdl", { "sgn", "mdl" } },
	{ "meo", { "ms",  "meo" } },
	{ "mfa", { "ms",  "mfa" } },
	{ "mfb", { "ms",  "mfb" } },
	{ "mfs", { "sgn", "mfs" } },
	{ "min", { "ms",  "min" } },
	{ "mnp", { "zh",  "mnp" } },
	{ "mqg", { "ms",  "mqg" } },
	{ "mre", { "sgn", "mre" } },
	{ "msd", { "sgn", "msd" } },
	{ "msi", { "ms",  "msi" } },
	{ "msr", { "sgn", "msr" } },
	{ "mui", { "ms",  "mui" } },
	{ "mzc", { "sgn", "mzc" } },
	{ "mzg", { "sgn", "mzg" } },
	{ "mzy", { "sgn", "mzy" } },
	{ "nan", { "zh",  "nan" } },
	{ "nbs", { "sgn", "nbs" } },
	{ "ncs", { "sgn", "ncs" } },
	{ "nsi", { "sgn", "nsi" } },
	{ "nsl", { "sgn", "nsl" } },
	{ "nsp", { "sgn", "nsp" } },
	{ "nsr", { "sgn", "nsr" } },
	{ "nzs", { "sgn", "nzs" } },
	{ "okl", { "sgn", "okl" } },
	{ "orn", { "ms",  "orn" } },
	{ "ors", { "ms",  "ors" } },
	{ "pel", { "ms",  "pel" } },
	{ "pga", { "ar",  "pga" } },
	{ "pks", { "sgn", "pks" } },
	{ "prl", { "sgn", "prl" } },
	{ "prz", { "sgn", "prz" } },
	{ "psc", { "sgn", "psc" } },
	{ "psd", { "sgn", "psd" } },
	{ "pse", { "ms",  "pse" } },
	{ "psg", { "sgn", "psg" } },
	{ "psl", { "sgn", "psl" } },
	{ "pso", { "sgn", "pso" } },
	{ "psp", { "sgn", "psp" } },
	{ "psr", { "sgn", "psr" } },
	{ "pys", { "sgn", "pys" } },
	{ "rms", { "sgn", "rms" } },
	{ "rsi", { "sgn", "rsi" } },
	{ "rsl", { "sgn", "rsl" } },
	{ "sdl", { "sgn", "sdl" } },
	{ "sfb", { "sgn", "sfb" } },
	{ "sfs", { "sgn", "sfs" } },
	{ "sgg", { "sgn", "sgg" } },
	{ "sgx", { "sgn", "sgx" } },
	{ "shu", { "ar",  "shu" } },
	{ "slf", { "sgn", "slf" } },
	{ "sls", { "sgn", "sls" } },
	{ "sqs", { "sgn", "sqs" } },
	{ "ssh", { "ar",  "ssh" } },
	{ "ssp", { "sgn", "ssp" } },
	{ "ssr", { "sgn", "ssr" } },
	{ "svk", { "sgn", "svk" } },
	{ "swc", { "sw",  "swc" } },
	{ "swh", { "sw",  "swh" } },
	{ "swl", { "sgn", "swl" } },
	{ "syy", { "sgn", "syy" } },
	{ "tmw", { "ms",  "tmw" } },
	{ "tse", { "sgn", "tse" } },
	{ "tsm", { "sgn", "tsm" } },
	{ "tsq", { "sgn", "tsq" } },
	{ "tss", { "sgn", "tss" } },
	{ "tsy", { "sgn", "tsy" } },
	{ "tza", { "sgn", "tza" } },
	{ "ugn", { "sgn", "ugn" } },
	{ "ugy", { "sgn", "ugy" } },
	{ "ukl", { "sgn", "ukl" } },
	{ "uks", { "sgn", "uks" } },
	{ "urk", { "ms",  "urk" } },
	{ "uzn", { "uz",  "uzn" } },
	{ "uzs", { "uz",  "uzs" } },
	{ "vgt", { "sgn", "vgt" } },
	{ "vkk", { "ms",  "vkk" } },
	{ "vkt", { "ms",  "vkt" } },
	{ "vsi", { "sgn", "vsi" } },
	{ "vsl", { "sgn", "vsl" } },
	{ "vsv", { "sgn", "vsv" } },
	{ "wuu", { "zh",  "wuu" } },
	{ "xki", { "sgn", "xki" } },
	{ "xml", { "sgn", "xml" } },
	{ "xmm", { "ms",  "xmm" } },
	{ "xms", { "sgn", "xms" } },
	{ "yds", { "sgn", "yds" } },
	{ "ysl", { "sgn", "ysl" } },
	{ "yue", { "zh",  "yue" } },
	{ "zib", { "sgn", "zib" } },
	{ "zlm", { "ms",  "zlm" } },
	{ "zmi", { "ms",  "zmi" } },
	{ "zsl", { "sgn", "zsl" } },
	{ "zsm", { "ms",  "zsm" } },
};

static const lang::tag *
lookup_lang(std::string lang,
            std::initializer_list<std::pair<std::string, const lang::tag>> &tags)
{
	lang = to_lower(lang);

	int begin = 0;
	int end = tags.size() - 1;

	while (begin <= end)
	{
		int pos = (begin + end) / 2;

		int comp = lang.compare((tags.begin() + pos)->first);
		if (comp == 0)
			return &(tags.begin() + pos)->second;
		else if (comp > 0)
			begin = pos + 1;
		else
			end = pos - 1;
	}

	return nullptr;
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
		{
			const lang::tag *extlang = lookup_lang(item, extlang_tags);
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
				const lang::tag *extlang = lookup_lang(item, extlang_tags);
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

cainteoir::languages::languages()
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
	if (!id.extlang.empty())
	{
		auto entry = m_subtags.find(id.extlang);
		if (entry == m_subtags.end())
			return id.extlang.c_str();
		return dgettext("iso_639", entry->second.c_str());
	}
	else
	{
		auto entry = m_subtags.find(id.lang);
		if (entry == m_subtags.end())
			return id.lang.c_str();
		return dgettext("iso_639", entry->second.c_str());
	}
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
