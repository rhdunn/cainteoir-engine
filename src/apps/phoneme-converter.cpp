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
namespace ipa = cainteoir::ipa;

enum class phoneme_mode
{
	joined,
	separate,
	broad_markers,
	narrow_markers,
	espeak_markers,
};

void print_phonemes(std::shared_ptr<tts::phoneme_reader> &aFrom,
                    std::shared_ptr<tts::phoneme_writer> &aTo,
                    const std::shared_ptr<cainteoir::buffer> &aTranscription,
                    tts::stress_type aStress)
{
	std::list<ipa::phoneme> phonemes;
	aFrom->reset(aTranscription);
	while (aFrom->read())
		phonemes.push_back(*aFrom);

	tts::make_stressed(phonemes, aStress);

	aTo->reset(stdout);
	for (const auto &phoneme : phonemes)
		aTo->write(phoneme);
	aTo->flush();
}

void print_open_marker(phoneme_mode aMode)
{
	switch (aMode)
	{
	case phoneme_mode::broad_markers:
		fprintf(stdout, "/");
		break;
	case phoneme_mode::narrow_markers:
		fprintf(stdout, "[");
		break;
	case phoneme_mode::espeak_markers:
		fprintf(stdout, "[[");
		break;
	}
}

void print_close_marker(phoneme_mode aMode)
{
	switch (aMode)
	{
	case phoneme_mode::broad_markers:
		fprintf(stdout, "/");
		break;
	case phoneme_mode::narrow_markers:
		fprintf(stdout, "]");
		break;
	case phoneme_mode::espeak_markers:
		fprintf(stdout, "]]");
		break;
	}
}

void print_phonemes(std::shared_ptr<tts::phoneme_reader> &aFrom,
                    std::shared_ptr<tts::phoneme_writer> &aTo,
                    const std::shared_ptr<cainteoir::buffer> &aTranscription,
                    phoneme_mode aMode,
                    bool aNoPauses,
                    bool aShowFeatures)
{
	auto feat = tts::createPhonemeWriter("features");
	bool need_open_marker = true;

	aFrom->reset(aTranscription);
	aTo->reset(stdout);
	feat->reset(stdout);
	while (true)
	{
		try
		{
			if (!aFrom->read())
			{
				aTo->flush();
				return;
			}
		}
		catch (const tts::phoneme_error &e)
		{
			fflush(stdout);
			fprintf(stderr, "\nerror: %s\n", e.what());
			fflush(stderr);
			continue;
		}

		if (aFrom->get(ipa::phoneme_type) == ipa::pause && aNoPauses)
			aTo->flush();
		else
		{
			if (need_open_marker)
			{
				print_open_marker(aMode);
				need_open_marker = false;
			}
			if (aFrom->get(ipa::phoneme_type | ipa::length) == (ipa::pause))
			{
				aTo->flush();
				print_close_marker(aMode);
				need_open_marker = true;
			}
			aTo->write(*aFrom);
			if (aShowFeatures)
			{
				fputc('\t', stdout);
				feat->write(*aFrom);
			}
			if (aMode == phoneme_mode::separate && !aFrom->get(ipa::joined_to_next_phoneme))
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
			{ 's', "separate", bind_value(mode, phoneme_mode::separate),
			  i18n("Display each phoneme on a new line") },
			{ 'f', "features", bind_value(show_features, true),
			  i18n("Show the features along with the transcription") },
			{ 0, "no-pauses", bind_value(no_pauses, true),
			  i18n("Do not process pause phonemes") },
		}};

		const option_group stress_options = { i18n("Phoneme Stress Placement:"), {
			{ 0, "vowel-stress", bind_value(stress, tts::stress_type::vowel),
			  i18n("Place the stress on vowels (e.g. espeak, arpabet)") },
			{ 0, "syllable-stress", bind_value(stress, tts::stress_type::syllable),
			  i18n("Place the stress on syllable boundaries") },
		}};

		const option_group marker_options = { i18n("Phoneme Markers:"), {
			{ 0, "broad", bind_value(mode, phoneme_mode::broad_markers),
			  i18n("Use /.../ between phonetic transcriptions") },
			{ 0, "narrow", bind_value(mode, phoneme_mode::narrow_markers),
			  i18n("Use [...] between phonetic transcriptions") },
			{ 0, "espeak", bind_value(mode, phoneme_mode::espeak_markers),
			  i18n("Use [[...]] between phonetic transcriptions") },
		}};

		const std::initializer_list<const char *> usage = {
			i18n("phoneme-converter [OPTION..] FROM TO TRANSCRIPTION"),
			i18n("phoneme-converter [OPTION..] FROM TO"),
		};

		if (!parse_command_line({ general_options, stress_options, marker_options }, usage, argc, argv))
			return 0;

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
	catch (std::runtime_error &e)
	{
		fprintf(stderr, i18n("error: %s\n"), e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
