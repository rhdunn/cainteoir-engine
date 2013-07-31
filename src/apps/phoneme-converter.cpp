/* Convert phonemes from one transcription scheme to another.
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
#include "compatibility.hpp"
#include "i18n.h"
#include "options.hpp"

#include <cainteoir/phoneme.hpp>

namespace tts = cainteoir::tts;

enum class phoneme_mode
{
	joined,
	separate,
	chart,
};

typedef tts::feature f;

static const std::initializer_list<std::pair<tts::feature, tts::feature>> manner_of_articulation = {
	{ f::nasal,       f::unspecified },
	{ f::plosive,     f::unspecified },
	{ f::affricate,   f::unspecified },
	{ f::sibilant,    f::fricative },
	{ f::fricative,   f::unspecified },
	{ f::approximant, f::unspecified },
	{ f::trill,       f::unspecified },
	{ f::flap,        f::unspecified },
	{ f::lateral,     f::flap },
	{ f::lateral,     f::fricative },
	{ f::lateral,     f::approximant },
	{ f::ejective,    f::unspecified },
	{ f::implosive,   f::unspecified },
	{ f::click,       f::unspecified },
};

static const std::initializer_list<tts::feature> place_of_articulation = {
	f::bilabial,
	f::labio_dental,
	f::dental,
	f::alveolar,
	f::palato_alveolar,
	f::retroflex,
	f::palatal,
	f::velar,
	f::uvular,
	f::pharyngeal,
	f::epiglottal,
	f::glottal,
};

static const std::initializer_list<tts::feature> vowel_backness = {
	f::front,
	f::center,
	f::back,
};

static const std::initializer_list<std::pair<tts::feature, tts::feature>> vowel_height = {
	{ f::high,      f::vowel },
	{ f::semi_high, f::vowel },
	{ f::upper_mid, f::vowel },
	{ f::mid,       f::vowel },
	{ f::lower_mid, f::vowel },
	{ f::semi_low,  f::vowel },
	{ f::low,       f::vowel },
};

static const std::initializer_list<tts::feature> roundness = {
	f::unrounded,
	f::rounded,
};

typedef std::pair<std::pair<const char *, const char *>, const std::initializer_list<tts::feature>>
        diacritics_t;

static const diacritics_t phonation = {{ i18n("Phonation"), "phonation" }, {
	f::voiceless,
	f::voiced,
	// breathy
	// creaky
}};

static const diacritics_t length = {{ i18n("Length"), "length" }, {
	f::unspecified,
	f::extra_short,
	f::half_long,
	f::long_
}};

static const diacritics_t stress = {{ i18n("Stress"), "stress" }, {
	f::unspecified,
	f::primary_stress,
	f::secondary_stress,
}};

static const diacritics_t coarticulation = {{ i18n("Co-articulation"), "coarticulation" }, {
	f::unspecified,
	f::labialized,
	f::palatalized,
	f::velarized,
	f::pharyngealized,
	f::nasalized,
	f::rhoticized,
}};

static const diacritics_t syllabicity = {{ i18n("Syllabicity"), "syllabicity" }, {
	f::unspecified,
	f::syllabic,
	// non-syllabic
}};

static const diacritics_t consonant_release = {{ i18n("Consonant release"), "release" }, {
	f::unspecified,
	f::aspirated,
	// inaudible release
	// nasal release
	// lateral release
}};

static const std::initializer_list<diacritics_t> diacritics = {
	phonation,
	length,
	stress,
	coarticulation,
	syllabicity,
	consonant_release,
};

#define begin_enumerate_diacritics(feature_set) \
	for (auto ph : phonation.second) { \
	for (auto ln : length.second) { \
	for (auto st : stress.second) { \
	for (auto co : coarticulation.second) { \
	for (auto sy : syllabicity.second) { \
	for (auto cr : consonant_release.second) { \
		const std::initializer_list<tts::feature> extra = { ph, ln, st, co, sy, cr };

#define end_enumerate_diacritics \
	}}}}}}

inline const char *get_feature_id(tts::feature f)
{
	const char *id = tts::get_feature_abbreviation(f);
	return id ? id : "und";
}

void print(const std::shared_ptr<cainteoir::buffer> &data)
{
	for (auto c : *data) switch (c)
	{
	case '<':
		fputs("&lt;", stdout);
		break;
	case '>':
		fputs("&gt;", stdout);
		break;
	case '&':
		fputs("&amp;", stdout);
		break;
	default:
		fputc(c, stdout);
		break;
	}
}

void print(const std::shared_ptr<tts::phoneme_writer> &ipa, tts::phoneme p)
{
	fputs("<td class=\"", stdout);
	bool need_space = false;
	for (auto feature : p)
	{
		if (feature != f::unspecified)
		{
			if (need_space) fputc(' ', stdout);
			fputs(tts::get_feature_abbreviation(feature), stdout);
			need_space = true;
		}
	}
	fputs("\">", stdout);

	cainteoir::memory_file out;
	ipa->reset(out);
	if (!ipa->write(p))
		fputs("\xC2\xA0", out);
	print(out.buffer());

	fputs("</td>\n", stdout);
}

void print_name(tts::phoneme p, int colspan = 1)
{
	if (colspan == 1)
		fputs("<th title=\"", stdout);
	else
		fprintf(stdout, "<th colspan=\"%d\" title=\"", colspan);
	bool need_space = false;
	for (auto feature : p)
	{
		if (feature != f::unspecified)
		{
			if (need_space) fputc(' ', stdout);
			fputs(tts::get_feature_name(feature), stdout);
			need_space = true;
		}
	}
	fputs("\">", stdout);
	need_space = false;
	for (auto feature : p)
	{
		if (feature != f::unspecified)
		{
			if (need_space) fputc(' ', stdout);
			fputs(tts::get_feature_abbreviation(feature), stdout);
			need_space = true;
		}
	}
	fputs("</th>\n", stdout);
}

void print_chart(const std::shared_ptr<tts::phoneme_writer> &ipa,
                 const char *caption,
                 const std::initializer_list<tts::feature> &x_features,
                 const std::initializer_list<std::pair<tts::feature, tts::feature>> &y_features,
                 const std::initializer_list<tts::feature> &z_features,
                 const std::initializer_list<tts::feature> &extra = {})
{
	fputs("<table width=\"100%\" cellspacing=\"0\" cellpadding=\"0\" class=\"chart\">\n", stdout);
	if (caption)
		fprintf(stdout, "<caption>%s</caption>", caption);
	fputs("<tr>\n", stdout);
	fputs("<th>&#xA0;</th>\n", stdout);
	int colspan = z_features.size() == 0 ? 1 : 2;
	for (auto x : x_features)
		print_name({ x, f::unspecified, f::unspecified }, colspan);
	fputs("</tr>\n", stdout);
	for (auto y : y_features)
	{
		fputs("<tr>\n", stdout);
		if (y.second == f::unspecified)
			print_name({ y.first, f::unspecified, f::unspecified });
		else
			print_name({ y.first, y.second, f::unspecified });
		for (auto x : x_features)
		{
			if (z_features.size() == 0)
			{
				tts::phoneme p;
				if (y.second == f::unspecified)
					p = { x, y.first, f::unspecified };
				else
					p = { x, y.first, y.second };
				for (auto e : extra)
					p.add(e);
				print(ipa, p);
			}
			else for (auto z : z_features)
			{
				tts::phoneme p;
				if (y.second == f::unspecified)
					p = { x, y.first, z };
				else
					p = { x, y.first, y.second, z };
				for (auto e : extra)
					p.add(e);
				print(ipa, p);
			}
		}
		fputs("</tr>\n", stdout);
	}
	fputs("</table>\n", stdout);
}

void print_chart(const std::shared_ptr<tts::phoneme_writer> &ipa, const char *name)
{
	fputs("<html>\n", stdout);
	fputs("<head>\n", stdout);
	fputs("<meta charset=\"UTF-8\"/>\n", stdout);
	fprintf(stdout, "<title>Phoneme Chart : %s</title>\n", name);
	fputs("<style type=\"text/css\">\n", stdout);
	fputs("    table   { border: 1px solid black; font-size: 14px; }\n", stdout);
	fputs("    td, th  { text-align: left; vertical-align: top; border: 1px solid black; padding: 0.2em; }\n", stdout);
	fputs("    caption { text-align: left; margin-top: 0.5em; margin-bottom: 0.5em; font-weight: bold; }\n", stdout);
	fputs("    .chart { font-family: 'Doulos SIL', 'Charis SIL', Gentium; }\n", stdout);
	fputs("    .chart .unr { text-align: left;  border-right: 0; }\n", stdout);
	fputs("    .chart .rnd { text-align: right; border-left:  0; }\n", stdout);
	fputs("    select { width: 100%; }\n", stdout);
	fputs("</style>\n", stdout);
	fputs("<script>\n", stdout);
	fputs("    function parse_features(text) {\n", stdout);
	fputs("        var feature_set;\n", stdout);
	fputs("        try {\n", stdout);
	fputs("            feature_set = JSON.parse(text.replace('#', ''));\n", stdout);
	fputs("        } catch (e) {\n", stdout);
	fputs("            feature_set = new Object();\n", stdout);
	fputs("        }\n", stdout);
	for (const auto &d : diacritics)
	{
		fprintf(stdout, "        if (feature_set[\"%s\"] === undefined) feature_set[\"%s\"] = \"%s\";\n",
		        d.first.second,
		        d.first.second,
		        get_feature_id(*d.second.begin()));
	}
	fputs("        return feature_set;\n", stdout);
	fputs("    }\n", stdout);
	fputs("    function check_feature_toggle(id, match) {\n", stdout);
	fputs("        var element = document.getElementById(id);\n", stdout);
	fputs("        if (id == match) {\n", stdout);
	fputs("            element.style.display = \"\";\n", stdout);
	fputs("        } else {\n", stdout);
	fputs("            element.style.display = \"none\";\n", stdout);
	fputs("        }\n", stdout);
	fputs("    }\n", stdout);
	fputs("    function feature_change(type, feature) {\n", stdout);
	fputs("        var feature_set = parse_features(window.location.hash);\n", stdout);
	fputs("        feature_set[type] = feature;\n", stdout);
	fputs("        window.location.hash = JSON.stringify(feature_set);\n", stdout);
	fputs("    }\n", stdout);
	fputs("    function update_features(feature_set) {\n", stdout);
	fputs("        var features = \"id\";\n", stdout);
	for (const auto &d : diacritics)
	{
		fprintf(stdout, "        features = features + \"_\" + feature_set[\"%s\"];\n",
		        d.first.second);
		fprintf(stdout, "        document.getElementById(\"%s\").value = feature_set[\"%s\"];\n",
		        d.first.second, d.first.second);
	}
	begin_enumerate_diacritics(extra)
		fprintf(stdout, "        check_feature_toggle(\"id_%s_%s_%s_%s_%s_%s\", features);\n",
		        get_feature_id(ph),
		        get_feature_id(ln),
		        get_feature_id(st),
		        get_feature_id(co),
		        get_feature_id(sy),
		        get_feature_id(cr));
	end_enumerate_diacritics
	fputs("    }\n", stdout);
	fputs("    window.onhashchange = function(evt) {\n", stdout);
	fputs("        var feature_set = parse_features(decodeURIComponent(evt.newURL.split('#')[1]));\n", stdout);
	fputs("        update_features(feature_set);\n", stdout);
	fputs("    }\n", stdout);
	fputs("    window.onload = function(evt) {\n", stdout);
	fputs("        var feature_set = parse_features(undefined);\n", stdout);
	fputs("        update_features(feature_set);\n", stdout);
	fputs("    }\n", stdout);
	fputs("</script>\n", stdout);
	fputs("</head>\n", stdout);
	fputs("<body>\n", stdout);

	fputs("<table width=\"100%\" cellspacing=\"0\" cellpadding=\"0\" class=\"chart\">\n", stdout);
	fprintf(stdout, "<caption>%s</caption>\n", i18n("Diacritics"));
	fputs("<tr>\n", stdout);
	int n = 0;
	for (const auto &d : diacritics)
	{
		fprintf(stdout, "<th>%s</th><td><select id='%s' onchange='feature_change(\"%s\", this.value);'>\n",
		        d.first.first, d.first.second, d.first.second);
		for (const auto &f : d.second)
		{
			const char *name = tts::get_feature_name(f);
			fprintf(stdout, " <option value=\"%s\">%s</option>\n",
			        get_feature_id(f),
			        name ? name : i18n("N/A"));
		}
		fputs("</select></td>\n", stdout);
		if (++n % 3 == 0)
			fputs("</tr><tr>\n", stdout);
	}
	fputs("</tr>\n", stdout);
	fputs("</table>\n", stdout);

	begin_enumerate_diacritics(extra)
		fprintf(stdout, "<div id=\"id_%s_%s_%s_%s_%s_%s\">\n",
		        get_feature_id(ph),
		        get_feature_id(ln),
		        get_feature_id(st),
		        get_feature_id(co),
		        get_feature_id(sy),
		        get_feature_id(cr));

		print_chart(ipa, i18n("Consonants"),
		            place_of_articulation, manner_of_articulation, {}, extra);

		print_chart(ipa, i18n("Vowels"),
		            vowel_backness, vowel_height, roundness, extra);

		fputs("</div>\n", stdout);
	end_enumerate_diacritics

	fputs("</body>\n", stdout);
	fputs("</html>\n", stdout);
}

int main(int argc, char ** argv)
{
	try
	{
		phoneme_mode mode = phoneme_mode::joined;
		bool no_pauses = false;
		bool show_features = false;

		const option_group general_options = { nullptr, {
			{ 'c', "chart", no_argument, nullptr,
			  i18n("Display the phoneme scheme as an IPA chart"),
			  [&mode](const char *) { mode = phoneme_mode::chart; }},
			{ 's', "separate", no_argument, nullptr,
			  i18n("Display each phoneme on a new line"),
			  [&mode](const char *) { mode = phoneme_mode::separate; }},
			{ 'f', "features", no_argument, nullptr,
			  i18n("Show the features along with the transcription"),
			  [&show_features](const char *) { show_features = true; }},
			{ 0, "no-pauses", no_argument, nullptr,
			  i18n("Do not process pause phonemes"),
			  [&no_pauses](const char *) { no_pauses = true; }},
		}};

		const std::initializer_list<const char *> usage = {
			i18n("phoneme-converter [OPTION..] FROM TO TRANSCRIPTION"),
			i18n("phoneme-converter [OPTION..] FROM TO"),
			i18n("phoneme-converter --chart PHONEMESET"),
		};

		if (!parse_command_line({ general_options }, usage, argc, argv))
			return 0;

		if (mode == phoneme_mode::chart)
		{
			if (argc != 1)
			{
				print_help({ general_options }, usage);
				return 0;
			}
			print_chart(tts::createPhonemeWriter(argv[0]), argv[0]);
		}
		else
		{
			if (argc != 2 && argc != 3)
			{
				print_help({ general_options }, usage);
				return 0;
			}
			auto from = tts::createPhonemeReader(argv[0]);
			auto to   = tts::createPhonemeWriter(argv[1]);
			auto feat = tts::createExplicitFeaturePhonemeWriter();

			from->reset(argc == 3 ? cainteoir::make_file_buffer(argv[2])
			                      : cainteoir::make_file_buffer(stdin));
			to->reset(stdout);
			feat->reset(stdout);
			while (from->read())
			{
				if (!from->contains(tts::feature::silent_pause) || !no_pauses)
				{
					to->write(*from);
					if (show_features)
					{
						fputc('\t', stdout);
						feat->write(*from);
					}
					if (mode == phoneme_mode::separate)
						fputc('\n', stdout);
				}
			}
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, i18n("error: %s\n"), e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
