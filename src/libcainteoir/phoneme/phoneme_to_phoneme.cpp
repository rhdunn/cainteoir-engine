/* Phoneme-to-Phoneme Rule Processor.
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

#include "../cainteoir_file_reader.hpp"

#include <map>

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;

struct phoneme_to_phoneme : public tts::phoneme_reader
{
	phoneme_to_phoneme(const char *aPhonemeToPhonemeRules,
	                   const std::shared_ptr<tts::phoneme_reader> &aPhonemes);

	void reset(const std::shared_ptr<cainteoir::buffer> &aBuffer);

	bool read();
private:
	std::shared_ptr<tts::phoneme_reader> mPhonemes;
	std::map<ipa::phoneme, ipa::phoneme> mPhonemeMap;
};

phoneme_to_phoneme::phoneme_to_phoneme(const char *aPhonemeToPhonemeRules,
                                       const std::shared_ptr<tts::phoneme_reader> &aPhonemes)
	: mPhonemes(aPhonemes)
{
	std::shared_ptr<tts::phoneme_reader> phonemes;
	cainteoir_file_reader rules{ cainteoir::path(aPhonemeToPhonemeRules) };

	while (rules.read())
	{
		if (rules.type() == cainteoir_file_reader::directive)
		{
			auto directive = rules.match();
			if (!rules.read() || rules.type() != cainteoir_file_reader::directive_contents)
			{
				fprintf(stderr, "error: missing directive body\n");
				continue;
			}

			if (directive.compare(".phonemeset") == 0)
			{
				phonemes = tts::createPhonemeReader(rules.match().str().c_str());
			}
		}
		else if (rules.type() == cainteoir_file_reader::phonemes)
		{
			phonemes->reset(cainteoir::make_buffer(rules.match().begin(), rules.match().size()));
			if (!phonemes->read())
				throw std::runtime_error("Unknown content in the phoneme-to-phoneme rule file.");

			ipa::phoneme from = *phonemes;
			if (phonemes->read())
			{
				fprintf(stderr, "error: mapping multiple phonemes is not currently supported.\n");
				continue;
			}

			if (!rules.read() || rules.type() != cainteoir_file_reader::phonemes)
			{
				throw std::runtime_error("Unknown content in the phoneme-to-phoneme rule file.");
			}

			phonemes->reset(cainteoir::make_buffer(rules.match().begin(), rules.match().size()));
			if (!phonemes->read())
				throw std::runtime_error("Unknown content in the phoneme-to-phoneme rule file.");

			ipa::phoneme to = *phonemes;
			if (phonemes->read())
			{
				fprintf(stderr, "error: mapping multiple phonemes is not currently supported.\n");
				continue;
			}

			mPhonemeMap[from] = to;
		}
		else
		{
			throw std::runtime_error("Unknown content in the phoneme-to-phoneme rule file.");
		}
	}
}

void phoneme_to_phoneme::reset(const std::shared_ptr<cainteoir::buffer> &aBuffer)
{
	mPhonemes->reset(aBuffer);
}

bool phoneme_to_phoneme::read()
{
	static constexpr auto mask = ipa::main | ipa::diacritics | ipa::length;

	if (!mPhonemes->read())
		return false;

	*(ipa::phoneme *)this = *mPhonemes;

	auto match = mPhonemeMap.find(mPhonemes->get(mask));
	if (match != mPhonemeMap.end())
	{
		set(match->second.get(mask), mask);
	}
	return true;
}

std::shared_ptr<tts::phoneme_reader>
tts::createPhonemeToPhonemeConverter(const char *aPhonemeToPhonemeRules,
                                     const std::shared_ptr<tts::phoneme_reader> &aPhonemes)
{
	return std::make_shared<phoneme_to_phoneme>(aPhonemeToPhonemeRules, aPhonemes);
}
