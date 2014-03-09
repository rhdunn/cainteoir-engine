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

enum syllable : uint8_t
{
	onset,
	nucleus,
	coda,
};

static void make_vowel_stressed(std::list<tts::phoneme> &aPhonemes)
{
	tts::phoneme::value_type stress = ipa::unstressed;

	for (auto current = aPhonemes.begin(), last = aPhonemes.end(); current != last; ++current)
	{
		auto &phoneme = *current;
		if (phoneme.get(ipa::phoneme_type) == ipa::syllable_break)
		{
			current = aPhonemes.erase(current);
			continue;
		}

		tts::phoneme::value_type current_stress = phoneme.get(ipa::stress);
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

static void make_syllable_stressed(std::list<tts::phoneme> &aPhonemes)
{
	auto onset = aPhonemes.begin();
	syllable state = syllable::onset;

	for (auto current = aPhonemes.begin(), last = aPhonemes.end(); current != last; ++current)
	{
		auto &phoneme = *current;
		tts::phoneme::value_type current_stress = phoneme.get(ipa::stress);
		if (phoneme.get(ipa::phoneme_type) == ipa::vowel || phoneme.get(ipa::syllabic))
		{
			if (state == syllable::nucleus) 
				onset = current;
			if (current_stress != ipa::unstressed)
			{
				// stressed syllable
				phoneme.set(ipa::unstressed, ipa::stress);
				onset->set(current_stress, ipa::stress);
			}
			state = syllable::nucleus;
		}
		else
		{
			if (state == syllable::nucleus)
				state = syllable::coda;
			if (state == syllable::coda) // coda = maximal consonant sequence
				onset = current;
		}
	}
}

void tts::make_stressed(std::list<phoneme> &aPhonemes, stress_type aType)
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
