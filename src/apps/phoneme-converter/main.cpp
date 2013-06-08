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

#include <cainteoir/phoneme.hpp>
#include <getopt.h>

namespace tts = cainteoir::tts;

static struct option options[] =
{
	{ "chart", no_argument, 0, 'c' },
	{ "help", no_argument, 0, 'h' },
	{ "separate", no_argument, 0, 's' },
	{ 0, 0, 0, 0 }
};

enum class phoneme_mode
{
	joined,
	separate,
	chart,
};

void help()
{
	fprintf(stdout, i18n("usage: phoneme-converter [OPTION..] <from-scheme> <to-scheme> document\n"));
	fprintf(stdout, i18n("usage: phoneme-converter --chart <phoneme-scheme>\n"));
	fprintf(stdout, "\n");
	fprintf(stdout, i18n("Options:\n"));
	fprintf(stdout, i18n(" -c, --chart            Display the phoneme scheme as an IPA chart\n"));
	fprintf(stdout, i18n(" -h, --help             This help text\n"));
	fprintf(stdout, i18n(" -s, --separate         Display each phoneme on a new line\n"));
	fprintf(stdout, "\n");
	fprintf(stdout, i18n("Report bugs to msclrhd@gmail.com\n"));
}

typedef tts::feature f;

static const std::initializer_list<std::pair<tts::feature, tts::feature>> manner_of_articulation = {
	{ f::nasal,       f::unspecified },
	{ f::plosive,     f::unspecified },
	{ f::affricate,   f::unspecified },
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
	{ f::high,      f::unspecified },
	{ f::semi_high, f::unspecified },
	{ f::upper_mid, f::unspecified },
	{ f::mid,       f::unspecified },
	{ f::lower_mid, f::unspecified },
	{ f::semi_low,  f::unspecified },
	{ f::low,       f::unspecified },
};

static const std::initializer_list<tts::feature> voicing = {
	f::voiceless,
	f::voiced,
};

static const std::initializer_list<tts::feature> roundness = {
	f::unrounded,
	f::rounded,
};

static const std::initializer_list<tts::phoneme> coarticulated_consonants = {
	{ f::voiceless, f::labialized,  f::velar,           f::approximant },
	{ f::voiced,    f::labialized,  f::velar,           f::approximant },
	{ f::voiced,    f::labialized,  f::palatal,         f::approximant },
	{ f::voiceless, f::palatalized, f::palato_alveolar, f::fricative },
	{ f::voiced,    f::palatalized, f::palato_alveolar, f::fricative },
	{ f::voiceless, f::palatalized, f::palato_alveolar, f::affricate },
	{ f::voiced,    f::palatalized, f::palato_alveolar, f::affricate },
};

static const std::initializer_list<tts::feature> consonant_diacritics = {
	f::unspecified,
	f::syllabic,
	f::labialized,
	f::palatalized,
};

static const std::initializer_list<tts::feature> vowel_diacritics = {
	f::unspecified,
	f::rhoticized,
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
	for (auto feature : p)
	{
		if (feature != f::unspecified)
		{
			fputs(tts::get_feature_abbreviation(feature), stdout);
			fputc(' ', stdout);
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
	for (auto feature : p)
	{
		if (feature != f::unspecified)
		{
			fputs(tts::get_feature_name(feature), stdout);
			fputc(' ', stdout);
		}
	}
	fputs("\">", stdout);
	for (auto feature : p)
	{
		if (feature != f::unspecified)
		{
			fputs(tts::get_feature_abbreviation(feature), stdout);
			fputc(' ', stdout);
		}
	}
	fputs("</th>", stdout);
}

void print_chart(const std::shared_ptr<tts::phoneme_writer> &ipa,
                 const char *caption,
                 const std::initializer_list<tts::feature>  &x_features,
                 const std::initializer_list<std::pair<tts::feature, tts::feature>> &y_features,
                 const std::initializer_list<tts::feature>  &z_features,
                 const tts::feature extra1,
                 const tts::feature extra2 = f::unspecified)
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
				if (y.second == f::unspecified)
					print(ipa, { x, y.first, z, extra1, extra2 });
				else
					print(ipa, { x, y.first, y.second, z, extra1 });
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
	fputs("    h1         { font-size: 16px; font-family: serif; }", stdout);
	fputs("    table      { border: 1px solid black; font-size: 12px; }", stdout);
	fputs("    td, th     { text-align: left; vertical-align: top; border: 1px solid black; padding: 0.2em; }", stdout);
	fputs("    caption    { text-align: left; margin-top: 0.5em; margin-bottom: 0.5em; font-weight: bold; }", stdout);
	fputs("    .chart .vls, .chart .unr { text-align: left;  font-family: monospace; border-right: 0; }", stdout);
	fputs("    .chart .vcd, .chart .rnd { text-align: right; font-family: monospace; border-left:  0; }", stdout);
	fputs("    .layout, .layout > tr > td, .layout > tbody > tr > td { border: 0; }", stdout);
	fputs("    .unpronouncible { background-color: lightgray; }", stdout);
	fputs("</style>\n", stdout);
	fputs("</head>\n", stdout);
	fputs("<body>\n", stdout);

	for (auto d : consonant_diacritics)
	{
		const char *name = tts::get_feature_name(d);
		if (name)
			fprintf(stdout, "<h1>%s (%s)</h1>", i18n("Consonants"), name);
		else
			fprintf(stdout, "<h1>%s</h1>", i18n("Consonants"));
		print_chart(ipa, nullptr,
		            place_of_articulation, manner_of_articulation, voicing, d);
	}

	fprintf(stdout, "<h1>%s</h1>", i18n("Vowels"));
	fputs("<table width=\"100%\" cellspacing=\"0\" cellpadding=\"0\" class=\"layout\">\n", stdout);
	fputs("<tr>\n", stdout);
	fputs("<td width=\"33%\">\n", stdout);
	print_chart(ipa, i18n("Short"),
	            vowel_backness, vowel_height, roundness, f::vowel);
	fputs("</td>\n", stdout);
	fputs("<td width=\"34%\">\n", stdout);
	print_chart(ipa, i18n("Rhoticized"),
	            vowel_backness, vowel_height, roundness, f::vowel, f::rhoticized);
	fputs("</td>\n", stdout);
	fputs("<td width=\"33%\">\n", stdout);
	print_chart(ipa, i18n("Long"),
	            vowel_backness, vowel_height, roundness, f::vowel, f::long_);
	fputs("</td>\n", stdout);
	fputs("</tr>\n", stdout);
	fputs("</table>\n", stdout);

	fputs("</body>\n", stdout);
	fputs("</html>\n", stdout);
}

int main(int argc, char ** argv)
{
	try
	{
		phoneme_mode mode = phoneme_mode::joined;

		while (1)
		{
			int option_index = 0;
			int c = getopt_long(argc, argv, "chs", options, &option_index);
			if (c == -1)
				break;

			switch (c)
			{
			case 'c':
				mode = phoneme_mode::chart;
				break;
			case 'h':
				help();
				return 0;
			case 's':
				mode = phoneme_mode::separate;
				break;
			}
		}

		argc -= optind;
		argv += optind;

		if (argc != (mode == phoneme_mode::chart ? 1 : 3))
		{
			help();
			return 0;
		}

		if (mode == phoneme_mode::chart)
			print_chart(tts::createPhonemeWriter(argv[0]), argv[0]);
		else
		{
			auto from = tts::createPhonemeReader(argv[0]);
			auto to   = tts::createPhonemeWriter(argv[1]);

			from->reset(cainteoir::make_file_buffer(argv[2]));
			to->reset(stdout);
			while (from->read())
			{
				to->write(*from);
				if (mode == phoneme_mode::separate)
					fputc('\n', stdout);
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
