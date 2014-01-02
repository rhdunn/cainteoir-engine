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

enum syllable : uint8_t
{
	onset,
	nucleus,
	coda,
};

static bool is_vowel(const tts::phoneme &aPhoneme)
{
	for (const tts::feature f : aPhoneme)
	{
		if (f == tts::feature::vowel ||
		    f == tts::feature::rising_diphthong ||
		    f == tts::feature::falling_diphthong)
			return true;
	}
	return false;
}

static tts::feature get_stress(const tts::phoneme &aPhoneme)
{
	for (const tts::feature f : aPhoneme)
	{
		if (f == tts::feature::primary_stress ||
		    f == tts::feature::secondary_stress ||
		    f == tts::feature::syllable_break)
			return f;
	}
	return tts::feature::unspecified;
}

void tts::make_vowel_stressed(std::list<phoneme> &aPhonemes)
{
	tts::feature stress = tts::feature::unspecified;

	for (auto current = aPhonemes.begin(), last = aPhonemes.end(); current != last; ++current)
	{
		auto &phoneme = *current;
		tts::feature current_stress = get_stress(phoneme);
		if (current_stress == tts::feature::syllable_break)
		{
			current = aPhonemes.erase(current);
		}
		else if (current_stress != tts::feature::unspecified)
		{
			if (is_vowel(phoneme))
				stress = tts::feature::unspecified;
			else
			{
				stress = current_stress;
				phoneme.remove(stress);
			}
		}
		else if (is_vowel(phoneme))
		{
			if (stress != tts::feature::unspecified)
				phoneme.add(stress);
		}
	}
}

void tts::make_syllable_stressed(std::list<phoneme> &aPhonemes)
{
	static const tts::phoneme sbr = { tts::feature::syllable_break, tts::feature::unspecified, tts::feature::unspecified };

	auto onset = aPhonemes.begin();
	syllable state = syllable::onset;

	for (auto current = aPhonemes.begin(), last = aPhonemes.end(); current != last; ++current)
	{
		auto &phoneme = *current;
		tts::feature current_stress = get_stress(phoneme);
		if (is_vowel(phoneme) || phoneme.contains(tts::feature::syllabic))
		{
			if (state == syllable::nucleus) 
				onset = current;
			if (current_stress != tts::feature::unspecified)
			{
				// stressed syllable
				phoneme.remove(current_stress);
				onset->add(current_stress);
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
