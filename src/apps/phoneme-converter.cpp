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
	syllables,
};

void print_phonemes(ipa::phonemes &aPhonemes,
                    std::shared_ptr<tts::phoneme_writer> &aTo,
                    const std::shared_ptr<tts::syllable_reader> &aSyllables)
{
	aTo->reset(stdout);
	aSyllables->reset(aPhonemes);
	bool have_syllable = aSyllables->read();
	for (auto p = aPhonemes.begin(), last = aPhonemes.end(); p != last;)
	{
		if (p == aSyllables->onset)
		{
			aTo->flush();
			fputc('{', stdout);
		}
		if (p == aSyllables->nucleus)
		{
			aTo->flush();
			fputc('[', stdout);
		}
		if (p == aSyllables->coda)
		{
			aTo->flush();
			fputc(']', stdout);
		}
		if (p == aSyllables->end)
		{
			aTo->flush();
			fputc('}', stdout);
			if (have_syllable = aSyllables->read())
				continue;
		}
		aTo->write(*p);
		++p;
	}
	if (have_syllable)
	{
		 if (aSyllables->coda == aPhonemes.end())
			fputc(']', stdout);
		 if (aSyllables->end == aPhonemes.end())
			fputc('}', stdout);
	}
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

void print_phonemes(ipa::phonemes &aPhonemes,
                    std::shared_ptr<tts::phoneme_writer> &aTo,
                    phoneme_mode aMode,
                    bool aNoPauses,
                    bool aShowFeatures)
{
	auto feat = tts::createPhonemeWriter("features");
	bool need_open_marker = true;

	aTo->reset(stdout);
	feat->reset(stdout);
	for (auto &&phoneme = aPhonemes.begin(), last = aPhonemes.end(); phoneme != last; ++phoneme)
	{
		if (phoneme->get(ipa::phoneme_type) == ipa::separator && aNoPauses)
			aTo->flush();
		else
		{
			if (need_open_marker)
			{
				print_open_marker(aMode);
				need_open_marker = false;
			}
			if (phoneme->get(ipa::phoneme_type | ipa::length) == ipa::separator)
			{
				aTo->flush();
				print_close_marker(aMode);
				need_open_marker = true;
			}
			aTo->write(*phoneme);
			if (aShowFeatures)
			{
				fputc('\t', stdout);
				feat->write(*phoneme);
			}

			if (aMode == phoneme_mode::separate)
			{
				bool is_linked_phoneme = false;
				if (phoneme->get(ipa::joined_to_next_phoneme))
					is_linked_phoneme = true;
				else
				{
					auto next = phoneme;
					++next;
					if (next != last)
					{
						if (next->get(ipa::diacritized))
							is_linked_phoneme = true;
					}
				}

				if (!is_linked_phoneme)
					fputc('\n', stdout);
			}
		}
	}
	aTo->flush();
}

bool read_phonemes(std::shared_ptr<tts::phoneme_reader> &aFrom,
                   ipa::phonemes &aPhonemes)
{
	aPhonemes.clear();
	while (true)
	{
		try
		{
			if (!aFrom->read())
				return !aPhonemes.empty();
		}
		catch (const tts::phoneme_error &e)
		{
			fflush(stdout);
			fprintf(stderr, "\nerror: %s\n", e.what());
			fflush(stderr);
			continue;
		}
		aPhonemes.push_back(*aFrom);
		if (aFrom->get(ipa::phoneme_type | ipa::length) == ipa::separator)
			return !aPhonemes.empty();
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
		const char *phoneme_map = nullptr;
		const char *accent = nullptr;

		const option_group general_options = { nullptr, {
			{ 's', "separate", bind_value(mode, phoneme_mode::separate),
			  i18n("Display each phoneme on a new line") },
			{ 'f', "features", bind_value(show_features, true),
			  i18n("Show the features along with the transcription") },
			{ 0, "no-pauses", bind_value(no_pauses, true),
			  i18n("Do not process pause phonemes") },
			{ 'M', "phoneme-map", phoneme_map, "PHONEME_MAP",
			  i18n("Use PHONEME_MAP to convert phonemes (e.g. accent conversion)") },
			{ 'a', "accent", accent, "ACCENT",
			  i18n("Use ACCENT to convert phonemes to the specified accent") },
		}};

		const option_group stress_options = { i18n("Phoneme Stress Placement:"), {
			{ 0, "syllables", bind_value(mode, phoneme_mode::syllables),
			  i18n("Show the syllable structure") },
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

		const std::initializer_list<const option_group *> options = {
			&general_options,
			&stress_options,
			&marker_options
		};

		const std::initializer_list<const char *> usage = {
			i18n("phoneme-converter [OPTION..] FROM TO TRANSCRIPTION"),
			i18n("phoneme-converter [OPTION..] FROM TO"),
		};

		if (!parse_command_line(options, usage, argc, argv))
			return 0;

		if (argc != 2 && argc != 3)
		{
			print_help(options, usage);
			return 0;
		}
		auto from = tts::createPhonemeReader(argv[0]);
		auto to   = tts::createPhonemeWriter(argv[1]);
		auto data = argc == 3 ? cainteoir::make_file_buffer(argv[2])
		                      : cainteoir::make_file_buffer(stdin);

		if (phoneme_map)
			from = tts::createPhonemeToPhonemeConverter(phoneme_map, from);
		if (accent)
			from = tts::createAccentConverter(accent, from);

		if (mode == phoneme_mode::syllables)
		{
			auto syllables = tts::create_syllable_reader();
			ipa::phonemes phonemes;
			from->reset(data);
			while (read_phonemes(from, phonemes))
			{
				tts::make_stressed(phonemes, stress);
				print_phonemes(phonemes, to, syllables);
			}
		}
		else
		{
			ipa::phonemes phonemes;
			from->reset(data);
			while (read_phonemes(from, phonemes))
			{
				tts::make_stressed(phonemes, stress);
				print_phonemes(phonemes, to, mode, no_pauses, show_features);
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
