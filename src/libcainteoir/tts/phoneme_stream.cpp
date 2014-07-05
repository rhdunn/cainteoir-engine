/* Phoneme Stream.
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

#include <cainteoir/text.hpp>

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;

struct words_to_phonemes_t : public tts::clause_processor
{
public:
	words_to_phonemes_t(const std::shared_ptr<tts::phoneme_reader> &aRules,
	                    const std::shared_ptr<tts::dictionary_reader> &aExceptionDictionary);

	void process(std::list<tts::text_event> &aClause);
private:
	std::shared_ptr<tts::phoneme_reader> mRules;
	tts::dictionary mExceptionDictionary;
};

words_to_phonemes_t::words_to_phonemes_t(const std::shared_ptr<tts::phoneme_reader> &aRules,
                                         const std::shared_ptr<tts::dictionary_reader> &aExceptionDictionary)
	: mRules(aRules)
{
	if (aExceptionDictionary) while (aExceptionDictionary->read())
		mExceptionDictionary.add_entry(aExceptionDictionary->word, aExceptionDictionary->entry);
}

void
words_to_phonemes_t::process(std::list<tts::text_event> &aClause)
{
	for (auto current = aClause.begin(), last = aClause.end(); current != last; ++current)
	{
		switch (current->type)
		{
		case tts::word_uppercase:
		case tts::word_lowercase:
		case tts::word_capitalized:
		case tts::word_mixedcase:
		case tts::word_script:
			if (!mExceptionDictionary.pronounce(current->text, mRules, current->phonemes))
			{
				// TODO: Should support using spelling logic
				// here to spell out the unpronouncible word.
				fprintf(stderr, "error: cannot pronounce '%s'\n", current->text->str().c_str());
			}
			break;
		}
	}
}

std::shared_ptr<tts::clause_processor>
tts::words_to_phonemes(const std::shared_ptr<phoneme_reader> &aRules,
                       const std::shared_ptr<dictionary_reader> &aExceptionDictionary)
{
	return std::make_shared<words_to_phonemes_t>(aRules, aExceptionDictionary);
}

void tts::generate_phonemes(const std::shared_ptr<tts::text_reader> &reader,
                            tts::clause_processor_chain &processor,
                            FILE *out,
                            const char *phonemeset,
                            tts::stress_type stress,
                            const char *open,
                            const char *close,
                            const char *phrase)
{
	auto ipa = tts::createPhonemeWriter(phonemeset);
	ipa->reset(out);

	bool need_open  = true;
	bool need_space = false;

	std::list<tts::text_event> clause;
	while (tts::next_clause(reader, clause))
	{
		processor.process(clause);
		for (auto && event : clause) switch (event.type)
		{
		default:
			if (!need_open)
			{
				fprintf(out, "%s", close ? close : "");
				need_open  = true;
				need_space = false;
			}
			if (event.type == tts::paragraph)
				fprintf(out, "\n\n");
			else
				fprintf(out, "%s%s", event.text->str().c_str(), phrase);
			break;
		case tts::word_uppercase:
		case tts::word_lowercase:
		case tts::word_capitalized:
		case tts::word_mixedcase:
		case tts::word_script:
			if (need_open)
			{
				if (open) fprintf(out, "%s", open);
				need_open  = false;
				need_space = false;
			}
			if (need_space)
				fprintf(out, " ");
			{
				auto phonemes = event.phonemes;
				tts::make_stressed(phonemes, stress);
				for (auto p : phonemes)
					ipa->write(p);
			}
			need_space = true;
			break;
		}
	}

	if (!need_open)
		fprintf(out, "%s\n", close ? close : "");
}
