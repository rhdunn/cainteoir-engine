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

struct adjust_stress : public tts::text_reader
{
public:
	adjust_stress();

	void bind(const std::shared_ptr<tts::text_reader> &aReader);

	const tts::text_event &event() const { return mReader->event(); }

	bool read();
private:
	void adjust_phonemes(std::list<tts::phoneme> &aPhonemes);

	std::shared_ptr<tts::text_reader> mReader;
	int  mPrevSyllableCount;
	bool mPrevIsStressed;
};

adjust_stress::adjust_stress()
	: mPrevSyllableCount(0)
	, mPrevIsStressed(false)
{
}

void
adjust_stress::bind(const std::shared_ptr<tts::text_reader> &aReader)
{
	mReader = aReader;
}

bool
adjust_stress::read()
{
	if (mReader && mReader->read())
	{
		auto &event = mReader->event();
		switch (event.type)
		{
		case tts::phonemes:
			adjust_phonemes(const_cast<std::list<tts::phoneme> &>(event.phonemes));
			break;
		case tts::comma:
			break;
		default:
			mPrevSyllableCount = 0;
			mPrevIsStressed = false;
			break;
		}
		return true;
	}
	return false;
}

void adjust_stress::adjust_phonemes(std::list<tts::phoneme> &aPhonemes)
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

std::shared_ptr<tts::text_reader>
tts::adjust_stress()
{
	return std::make_shared<::adjust_stress>();
}
