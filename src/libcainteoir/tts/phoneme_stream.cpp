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

struct hyphenated_word
{
	hyphenated_word(const std::shared_ptr<cainteoir::buffer> &aText)
		: first(aText->begin())
		, last(aText->end())
		, next(first)
	{
		advance();
	}

	bool have_word() { return first <= last; }

	std::shared_ptr<cainteoir::buffer> next_word();
private:
	void advance();

	const char *first;
	const char *last;
	const char *next;
};

std::shared_ptr<cainteoir::buffer> hyphenated_word::next_word()
{
	auto ret = std::make_shared<cainteoir::buffer>(first, next-1);
	advance();
	return ret;
}

void hyphenated_word::advance()
{
	first = next;
	while (next <= last && *next != '-')
		++next;
	if (*next == '-')
		++next;
}

struct words_to_phonemes : public tts::text_reader
{
public:
	words_to_phonemes(const std::shared_ptr<tts::phoneme_reader> &aRules,
	                  const std::shared_ptr<tts::dictionary_reader> &aExceptionDictionary);

	void bind(const std::shared_ptr<tts::text_reader> &aReader);

	const tts::text_event &event() const { return mEvent; }

	bool read();
private:
	bool pronounce(const std::shared_ptr<cainteoir::buffer> &aText, const cainteoir::range<uint32_t> &aRange);

	std::shared_ptr<tts::text_reader> mReader;
	tts::text_event mEvent;
	std::shared_ptr<tts::phoneme_reader> mRules;
	tts::dictionary mExceptionDictionary;
};

words_to_phonemes::words_to_phonemes(const std::shared_ptr<tts::phoneme_reader> &aRules,
                                     const std::shared_ptr<tts::dictionary_reader> &aExceptionDictionary)
	: mRules(aRules)
{
	if (aExceptionDictionary) while (aExceptionDictionary->read())
		mExceptionDictionary.add_entry(aExceptionDictionary->word, aExceptionDictionary->entry);
}

void
words_to_phonemes::bind(const std::shared_ptr<tts::text_reader> &aReader)
{
	mReader = aReader;
}

bool
words_to_phonemes::read()
{
	if (mReader) while (mReader->read()) switch (mReader->event().type)
	{
	case tts::word_uppercase:
	case tts::word_lowercase:
	case tts::word_capitalized:
	case tts::word_mixedcase:
	case tts::word_script:
		if (pronounce(mReader->event().text, mReader->event().range))
			return true;
		break;
	default:
		mEvent = mReader->event();
		return true;
	}
	return false;
}

bool
words_to_phonemes::pronounce(const std::shared_ptr<cainteoir::buffer> &aText,
                             const cainteoir::range<uint32_t> &aRange)
{
	mEvent = { tts::phonemes, aRange, 0 };

	// Try looking up the pronunciation in the exception dictionary ...

	if (mExceptionDictionary.pronounce(aText, mEvent.phonemes))
		return true;

	// Try looking up hyphenated words individually ...

	hyphenated_word hyphenated{ aText };
	if (hyphenated.have_word()) try
	{
		while (hyphenated.have_word())
		{
			auto word = hyphenated.next_word();

			// Try pronouncing the word ...

			std::list<ipa::phoneme> phonemes;
			if (mExceptionDictionary.pronounce(word, phonemes))
			{
				for (const auto &p : phonemes)
					mEvent.phonemes.push_back(p);
			}
			else if (mRules.get())
			{
				mRules->reset(word);
				while (mRules->read())
					mEvent.phonemes.push_back(*mRules);
			}
			else
				throw tts::phoneme_error("unable to pronounce the hyphenated word");
		}

		return true;
	}
	catch (const tts::phoneme_error &e)
	{
		mEvent.phonemes.clear();
	}

	// Try using pronunciation rules ...

	if (mRules.get()) try
	{
		mRules->reset(aText);
		while (mRules->read())
			mEvent.phonemes.push_back(*mRules);
		return true;
	}
	catch (const tts::phoneme_error &e)
	{
		// Unable to pronounce the word using the ruleset, so fall
		// through to the failure logic below.
	}

	// TODO: Should use spelling logic here to spell out the unpronouncible word.

	fprintf(stderr, "error: cannot pronounce '%s'\n", aText->str().c_str());
	return false;
}

std::shared_ptr<tts::text_reader>
tts::words_to_phonemes(const std::shared_ptr<phoneme_reader> &aRules,
                       const std::shared_ptr<dictionary_reader> &aExceptionDictionary)
{
	return std::make_shared<::words_to_phonemes>(aRules, aExceptionDictionary);
}

void tts::generate_phonemes(const std::shared_ptr<tts::text_reader> &reader,
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
	while (reader->read())
	{
		auto &event = reader->event();
		switch (event.type)
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
		case tts::phonemes:
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
