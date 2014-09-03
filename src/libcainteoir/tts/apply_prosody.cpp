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

struct apply_prosody_t : public tts::clause_processor
{
public:
	apply_prosody_t();

	void process(std::list<tts::text_event> &aClause);
};

apply_prosody_t::apply_prosody_t()
{
}

void
apply_prosody_t::process(std::list<tts::text_event> &aClause)
{
	for (auto current = aClause.begin(), last = aClause.end(); current != last; ++current)
	{
		auto &event = *current;
		switch (event.type)
		{
		case tts::comma:
		case tts::semicolon:
		case tts::colon:
		case tts::ellipsis:
			event.phonemes.push_back(ipa::foot_break);
			break;
		case tts::full_stop:
		case tts::exclamation:
		case tts::question:
		case tts::paragraph:
			event.phonemes.push_back(ipa::intonation_break);
			break;
		case tts::word_uppercase:
		case tts::word_lowercase:
		case tts::word_capitalized:
		case tts::word_mixedcase:
		case tts::word_script:
			for (auto &p : event.phonemes)
			{
				if (p.get(ipa::phoneme_type | ipa::syllabicity) == ipa::vowel)
				{
					p.set(ipa::tone_start_bottom, ipa::tone_start);
				}
			}
			break;
		}
	}
}

std::shared_ptr<tts::clause_processor>
tts::apply_prosody()
{
	return std::make_shared<apply_prosody_t>();
}
