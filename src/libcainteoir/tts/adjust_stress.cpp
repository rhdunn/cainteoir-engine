/* Adjust the stress patterns to a more natural rhythmn.
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
#include "compatibility.hpp"

#include <cainteoir/text.hpp>

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;

struct adjust_stress_t : public tts::clause_processor
{
public:
	adjust_stress_t();

	void process(std::list<tts::text_event> &aClause);
private:
	void adjust_phonemes(ipa::phonemes &aPhonemes);

	int  mPrevSyllableCount;
	bool mPrevIsStressed;
};

adjust_stress_t::adjust_stress_t()
	: mPrevSyllableCount(0)
	, mPrevIsStressed(false)
{
}

void
adjust_stress_t::process(std::list<tts::text_event> &aClause)
{
	for (auto current = aClause.begin(), last = aClause.end(); current != last; ++current)
	{
		switch (current->type)
		{
		case tts::comma:
			break;
		default:
			if (current->phonemes.empty())
			{
				mPrevSyllableCount = 0;
				mPrevIsStressed = false;
			}
			else
				adjust_phonemes(current->phonemes);
			break;
		}
	}
}

void adjust_stress_t::adjust_phonemes(ipa::phonemes &aPhonemes)
{
	auto stress_placement = aPhonemes.end();
	int syllable_count = 0;
	for (auto current = aPhonemes.begin(), last = aPhonemes.end(); current != last; ++current)
	{
		auto &phoneme = *current;

		bool is_stressed = phoneme.get(ipa::stress) != ipa::unstressed;
		bool is_vowel = phoneme.get(ipa::phoneme_type) == ipa::vowel;

		if (is_stressed && syllable_count == 0)
			stress_placement = current;
		if (is_vowel)
			++syllable_count;
	}

	bool is_stressed = stress_placement != aPhonemes.end();
	if (mPrevSyllableCount == 1 && mPrevIsStressed)
	{
		if (syllable_count == 1 && is_stressed)
		{
			stress_placement->set(ipa::stress, ipa::unstressed);
			is_stressed = false;
		}
	}

	mPrevSyllableCount = syllable_count;
	mPrevIsStressed = is_stressed;
}

std::shared_ptr<tts::clause_processor>
tts::adjust_stress()
{
	return std::make_shared<adjust_stress_t>();
}
