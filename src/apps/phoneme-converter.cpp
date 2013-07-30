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

static const std::initializer_list<tts::feature> voicing = {
	f::voiceless,
	f::voiced,
};

static const std::initializer_list<tts::feature> roundness = {
	f::unrounded,
	f::rounded,
};

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
	for (auto x : x_features)
		print_name({ x, f::unspecified, f::unspecified }, 2);
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
			for (auto z : z_features)
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
	fputs("    h1      { font-size: 16px; font-family: serif; }\n", stdout);
	fputs("    table   { border: 1px solid black; font-size: 14px; }\n", stdout);
	fputs("    td, th  { text-align: left; vertical-align: top; border: 1px solid black; padding: 0.2em; }\n", stdout);
	fputs("    caption { text-align: left; margin-top: 0.5em; margin-bottom: 0.5em; font-weight: bold; }\n", stdout);
	fputs("    .chart .vls, .chart .unr { text-align: left;  font-family: 'Doulos SIL', 'Charis SIL', Gentium; border-right: 0; }\n", stdout);
	fputs("    .chart .vcd, .chart .rnd { text-align: right; font-family: 'Doulos SIL', 'Charis SIL', Gentium; border-left:  0; }\n", stdout);
	fputs("    .layout, .layout > tr > td, .layout > tbody > tr > td { border: 0; }\n", stdout);
	fputs("    .unpronouncible { background-color: lightgray; }\n", stdout);
	fputs("    .button { cursor: pointer; border: 1px solid black; margin-left: 0.1em; padding: 0.2em; border-radius: 0.25em; }\n", stdout);
	fputs("</style>\n", stdout);
	fputs("</head>\n", stdout);
	fputs("<body>\n", stdout);

	print_chart(ipa, i18n("Consonants"),
	            place_of_articulation, manner_of_articulation, voicing);

	print_chart(ipa, i18n("Vowels"),
	            vowel_backness, vowel_height, roundness);

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
