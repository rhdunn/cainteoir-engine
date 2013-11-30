/* Phoneme Stream.
 *
 * Copyright (C) 2013 Reece H. Dunn
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

tts::phoneme_stream::phoneme_stream(const std::shared_ptr<document_reader> &aReader,
                                    const language::tag &aLocale,
                                    tts::word_stream::number_scale aScale,
                                    const std::shared_ptr<phoneme_reader> &aRules,
                                    const char *aExceptionDictionaryPath)
	: mReader(aReader, aLocale, aScale)
	, mRules(aRules)
{
	if (!parseCainteoirDictionary(mExceptionDictionary, aExceptionDictionaryPath))
		fprintf(stderr, "unable to load exception dictionary: %s\n", (const char *)aExceptionDictionaryPath);
}

bool tts::phoneme_stream::read()
{
	if (!mReader.read()) return false;

	const tts::text_event &e = mReader.event();
	switch (e.type)
	{
	case tts::word_uppercase:
	case tts::word_lowercase:
	case tts::word_capitalized:
	case tts::word_mixedcase:
	case tts::word_script:
		pronounce(e.text, e.range);
		break;
	case tts::comma:
		mEvent = { tts::pause, e.range, 50 };
		break;
	case tts::semicolon:
	case tts::colon:
	case tts::ellipsis:
		mEvent = { tts::pause, e.range, 75 };
		break;
	case tts::full_stop:
	case tts::exclamation:
	case tts::question:
		mEvent = { tts::pause, e.range, 150 };
		break;
	case tts::paragraph:
		mEvent = { tts::pause, e.range, 200 };
		break;
	default:
		return read();
	};

	return true;
}

void tts::phoneme_stream::pronounce(const std::shared_ptr<buffer> &aText, const range<uint32_t> &aRange)
{
	mEvent = { tts::phonemes, aRange, 0 };

	auto phonemes = mExceptionDictionary.pronounce(aText);
	if (phonemes.empty())
	{
		for (const tts::phoneme &p : phonemes)
			mEvent.phonemes.push_back(p);
	}
	else if (mRules.get())
	{
		mRules->reset(aText);
		while (mRules->read())
			mEvent.phonemes.push_back(*mRules);
	}
	else
		fprintf(stderr, "error: cannot pronounce '%s'\n", aText->str().c_str());
}
