/* Phoneme Stress Patterns.
 *
 * Copyright (C) 2014 Reece H. Dunn
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
#include "i18n.h"
#include "compatibility.hpp"

#include <cainteoir/phoneme.hpp>
#include <utility>
#include <string.h>

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;

enum syllable_t : uint8_t
{
	onset,
	nucleus,
	coda,
};

struct syllable_reader_t : public tts::syllable_reader
{
	void reset(ipa::phonemes &aPhonemes);

	bool read();

	ipa::phonemes::iterator last;
};

void syllable_reader_t::reset(ipa::phonemes &aPhonemes)
{
	end  = aPhonemes.begin();
	last = aPhonemes.end();
}

bool syllable_reader_t::read()
{
	syllable_t state = syllable_t::onset;
	ipa::phonemes::iterator coda_start = end;
	onset = end;

	for (auto current = onset; current != last; ++current)
	{
		auto &phoneme = *current;
		switch (phoneme.get(ipa::phoneme_type))
		{
		case ipa::vowel:
			switch (state)
			{
			case syllable_t::onset:
				state = syllable_t::nucleus;
				nucleus = current;
				break;
			case syllable_t::nucleus:
				if (phoneme.get(ipa::syllabicity) != ipa::non_syllabic)
				{
					coda = end = current;
					return true;
				}
				break;
			case syllable_t::coda:
				coda = end = coda_start;
				return true;
			}
			break;
		case ipa::separator:
			switch (state)
			{
			case syllable_t::onset:
				onset = coda_start = current;
				break;
			case syllable_t::nucleus:
				coda = current;
				end = current;
				return true;
			case syllable_t::coda:
				coda = coda_start;
				end = current;
				return true;
			}
			break;
		default:
			switch (state)
			{
			case syllable_t::onset:
				break;
			case syllable_t::nucleus:
				state = syllable_t::coda;
				coda_start = current;
				break;
			case syllable_t::coda:
				break;
			}
			break;
		}
	}

	onset = nucleus = coda = end = last;
	return false;
}

std::shared_ptr<tts::syllable_reader> tts::create_syllable_reader()
{
	return std::make_shared<syllable_reader_t>();
}

static void make_vowel_stressed(ipa::phonemes &aPhonemes)
{
	ipa::phoneme::value_type stress = ipa::unstressed;

	for (auto current = aPhonemes.begin(), last = aPhonemes.end(); current != last; ++current)
	{
		auto &phoneme = *current;
		ipa::phoneme::value_type current_stress = phoneme.get(ipa::stress);
		if (current_stress != ipa::unstressed)
		{
			if (phoneme.get(ipa::phoneme_type) == ipa::vowel)
				stress = ipa::unstressed;
			else
			{
				stress = current_stress;
				phoneme.set(ipa::unstressed, ipa::stress);
			}
		}
		else if (phoneme.get(ipa::phoneme_type) == ipa::vowel)
		{
			if (stress != ipa::unstressed)
			{
				phoneme.set(stress, ipa::stress);
				stress = ipa::unstressed;
			}
		}
	}
}

static void make_syllable_stressed(ipa::phonemes &aPhonemes)
{
	auto onset = aPhonemes.begin();
	syllable_t state = syllable_t::onset;

	for (auto current = aPhonemes.begin(), last = aPhonemes.end(); current != last; ++current)
	{
		auto &phoneme = *current;
		ipa::phoneme::value_type current_stress = phoneme.get(ipa::stress);
		if (phoneme.get(ipa::phoneme_type) == ipa::vowel || phoneme.get(ipa::syllabic))
		{
			if (state == syllable_t::nucleus)
				onset = current;
			if (current_stress != ipa::unstressed)
			{
				// stressed syllable
				phoneme.set(ipa::unstressed, ipa::stress);
				onset->set(current_stress, ipa::stress);
			}
			state = syllable_t::nucleus;
		}
		else
		{
			if (state == syllable_t::nucleus)
				state = syllable_t::coda;
			if (state == syllable_t::coda) // coda = maximal consonant sequence
				onset = current;
		}
	}
}

static void make_primary_stressed(const ipa::phonemes &aPhonemes, ipa::phonemes &aOutput)
{
	bool initial = true;
	for (const auto &p : aPhonemes)
	{
		if (initial)
		{
			initial = false;
			auto out = p;
			out.set(ipa::primary_stress, ipa::stress);
			aOutput.push_back(out);
		}
		else
			aOutput.push_back(p);
	}
}

static void make_secondary_stressed(const ipa::phonemes &aPhonemes, ipa::phonemes &aOutput)
{
	bool initial = true;
	for (const auto &p : aPhonemes)
	{
		if (initial)
		{
			initial = false;
			auto out = p;
			out.set(ipa::secondary_stress, ipa::stress);
			aOutput.push_back(out);
		}
		else
			aOutput.push_back(p);
	}
}

static void make_initial_unstressed(const ipa::phonemes &aPhonemes, ipa::phonemes &aOutput)
{
	bool initial = true;
	for (const auto &p : aPhonemes)
	{
		if (initial)
		{
			initial = false;
			auto out = p;
			out.set(0, ipa::stress);
			aOutput.push_back(out);
		}
		else
			aOutput.push_back(p);
	}
}

void tts::make_stressed(ipa::phonemes &aPhonemes, stress_type aType)
{
	switch (aType)
	{
	case stress_type::as_transcribed:
		break; // no change
	case stress_type::vowel:
		make_vowel_stressed(aPhonemes);
		break;
	case stress_type::syllable:
		make_syllable_stressed(aPhonemes);
		break;
	}
}

void tts::make_stressed(const ipa::phonemes &aPhonemes,
                        ipa::phonemes &aOutput,
                        initial_stress aType)
{
	switch (aType)
	{
	case initial_stress::as_transcribed:
		for (const auto &p : aPhonemes)
			aOutput.push_back(p);
		break;
	case initial_stress::primary:
		make_primary_stressed(aPhonemes, aOutput);
		break;
	case initial_stress::secondary:
		make_secondary_stressed(aPhonemes, aOutput);
		break;
	case initial_stress::unstressed:
		make_initial_unstressed(aPhonemes, aOutput);
		break;
	}
}
