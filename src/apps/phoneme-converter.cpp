/* Convert phonemes from one transcription scheme to another.
 *
 * Copyright (C) 2013-2014 Reece H. Dunn
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
	{ f::sibilant,    f::fricative },
	{ f::fricative,   f::unspecified },
	{ f::approximant, f::unspecified },
	{ f::flap,        f::unspecified },
	{ f::trill,       f::unspecified },
	{ f::lateral,     f::fricative },
	{ f::lateral,     f::approximant },
	{ f::lateral,     f::flap },
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

static const std::initializer_list<tts::feature> voicing = {
	f::voiceless,
	f::voiced,
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
                 const char *caption, const char *id,
                 const std::initializer_list<tts::feature> &x_features,
                 const std::initializer_list<std::pair<tts::feature, tts::feature>> &y_features,
                 const std::initializer_list<tts::feature> &z_features,
                 const std::initializer_list<tts::feature> &extra)
{
	fprintf(stdout, "<table width=\"100%\" cellspacing=\"0\" cellpadding=\"0\" class=\"chart\" id=\"%s\">\n", id);
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
	fputs("    #consonants .vls, #vowels .unr { text-align: left;  border-right: 0; }\n", stdout);
	fputs("    #consonants .vcd, #vowels .rnd { text-align: right; border-left:  0; }\n", stdout);
	fputs("</style>\n", stdout);
	fputs("</head>\n", stdout);
	fputs("<body>\n", stdout);

	print_chart(ipa, i18n("Consonants"), "consonants",
	            place_of_articulation, manner_of_articulation, voicing, {});

	print_chart(ipa, i18n("Vowels"), "vowels",
	            vowel_backness, vowel_height, roundness, { f::voiced, f::vowel });

	fputs("</body>\n", stdout);
	fputs("</html>\n", stdout);
}

void print_phonemes(std::shared_ptr<tts::phoneme_reader> &aFrom,
                    std::shared_ptr<tts::phoneme_writer> &aTo,
                    const std::shared_ptr<cainteoir::buffer> &aTranscription,
                    tts::stress_type aStress)
{
	std::list<tts::phoneme> phonemes;
	aFrom->reset(aTranscription);
	while (aFrom->read())
		phonemes.push_back(*aFrom);

	tts::make_stressed(phonemes, aStress);

	aTo->reset(stdout);
	for (const auto &phoneme : phonemes)
		aTo->write(phoneme);
}

void print_phonemes(std::shared_ptr<tts::phoneme_reader> &aFrom,
                    std::shared_ptr<tts::phoneme_writer> &aTo,
                    const std::shared_ptr<cainteoir::buffer> &aTranscription,
                    phoneme_mode aMode,
                    bool aNoPauses,
                    bool aShowFeatures)
{
	auto feat = tts::createExplicitFeaturePhonemeWriter();

	aFrom->reset(aTranscription);
	aTo->reset(stdout);
	feat->reset(stdout);
	while (aFrom->read())
	{
		if (!aFrom->contains(tts::feature::silent_pause) || !aNoPauses)
		{
			try
			{
				aTo->write(*aFrom);
			}
			catch (const tts::phoneme_error &e)
			{
				fflush(stdout);
				fprintf(stderr, "\nerror: %s\n", e.what());
				fflush(stderr);
			}
			if (aShowFeatures)
			{
				fputc('\t', stdout);
				feat->write(*aFrom);
			}
			if (aMode == phoneme_mode::separate)
				fputc('\n', stdout);
		}
	}
}

int main(int argc, char ** argv)
{
	try
	{
		tts::stress_type stress = tts::stress_type::as_transcribed;
		phoneme_mode mode = phoneme_mode::joined;
		bool no_pauses = false;
		bool show_features = false;

		const option_group general_options = { nullptr, {
			{ 'c', "chart", bind_value(mode, phoneme_mode::chart),
			  i18n("Display the phoneme scheme as an IPA chart") },
			{ 's', "separate", bind_value(mode, phoneme_mode::separate),
			  i18n("Display each phoneme on a new line") },
			{ 'f', "features", bind_value(show_features, true),
			  i18n("Show the features along with the transcription") },
			{ 0, "no-pauses", bind_value(no_pauses, true),
			  i18n("Do not process pause phonemes") },
			{ 0, "vowel-stress", bind_value(stress, tts::stress_type::vowel),
			  i18n("Place the stress on vowels (e.g. espeak, arpabet)") },
			{ 0, "syllable-stress", bind_value(stress, tts::stress_type::syllable),
			  i18n("Place the stress on syllable boundaries") },
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
			auto data = argc == 3 ? cainteoir::make_file_buffer(argv[2])
			                      : cainteoir::make_file_buffer(stdin);

			if (stress == tts::stress_type::as_transcribed)
				print_phonemes(from, to, data, mode, no_pauses, show_features);
			else
				print_phonemes(from, to, data, stress);
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, i18n("error: %s\n"), e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
