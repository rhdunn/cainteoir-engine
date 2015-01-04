/* Phoneme-to-Phoneme Rule Processor.
 *
 * Copyright (C) 2014-2015 Reece H. Dunn
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
#include <cainteoir/trie.hpp>

#include "../cainteoir_file_reader.hpp"

#include <list>

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;

struct phoneme_to_phoneme : public tts::phoneme_reader
{
	phoneme_to_phoneme(const char *aPhonemeToPhonemeRules,
	                   const std::shared_ptr<tts::phoneme_reader> &aPhonemes);

	void reset(const std::shared_ptr<cainteoir::buffer> &aBuffer);

	bool read();
private:
	bool next_phoneme(ipa::phoneme &phoneme);

	struct entry_t
	{
		ipa::phonemes phonemes;
		bool valid;

		entry_t() : valid(false) {}
	};

	std::shared_ptr<tts::phoneme_reader> mPhonemes;
	cainteoir::trie<entry_t, ipa::phoneme> mPhonemeMap;
	std::list<ipa::phoneme> mReadQueue;

	ipa::phonemes::const_iterator mCurrentPhoneme;
	ipa::phonemes::const_iterator mLastPhoneme;
	ipa::phoneme::value_type mInitialPhonemeProsody;
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

			auto entry = const_cast<cainteoir::trie_node<entry_t, ipa::phoneme> *>(mPhonemeMap.root());
			while (phonemes->read())
				entry = entry->add(*phonemes);

			if (entry == mPhonemeMap.root())
				throw std::runtime_error("Unknown content in the phoneme-to-phoneme rule file.");

			if (!rules.read() || rules.type() != cainteoir_file_reader::phonemes)
			{
				throw std::runtime_error("Unknown content in the phoneme-to-phoneme rule file.");
			}

			phonemes->reset(cainteoir::make_buffer(rules.match().begin(), rules.match().size()));

			entry->item.valid = true;
			while (phonemes->read())
				entry->item.phonemes.push_back(*phonemes);
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

	if (mCurrentPhoneme != mLastPhoneme)
	{
		*(ipa::phoneme *)this = *mCurrentPhoneme;
		if (mInitialPhonemeProsody != ipa::unspecified)
		{
			set(mInitialPhonemeProsody, ~mask);
			mInitialPhonemeProsody = ipa::unspecified;
		}

		++mCurrentPhoneme;
		return true;
	}

	auto entry = mPhonemeMap.root();
	decltype(entry) match = nullptr;
	bool is_first_phoneme = true;

	std::list<ipa::phoneme> read_queue;
	ipa::phoneme phoneme = ipa::unspecified;
	while (next_phoneme(phoneme))
	{
		if (is_first_phoneme)
		{
			mInitialPhonemeProsody = phoneme.get(~mask);
			is_first_phoneme = false;
		}

		entry = entry->get(phoneme.get(mask));
		if (entry == nullptr)
		{
			while (!read_queue.empty())
			{
				mReadQueue.push_back(read_queue.front());
				read_queue.pop_front();
			}
			mReadQueue.push_back(phoneme);

			if (match)
			{
				mCurrentPhoneme = match->item.phonemes.begin();
				mLastPhoneme = match->item.phonemes.end();
				return read();
			}
			else
			{
				next_phoneme(phoneme);
				*(ipa::phoneme *)this = phoneme;
			}
			return true;
		}
		if (entry->item.valid)
		{
			read_queue.clear();
			match = entry;
		}
		else
			read_queue.push_back(phoneme);
	}

	while (!read_queue.empty())
	{
		mReadQueue.push_back(read_queue.front());
		read_queue.pop_front();
	}

	if (match)
	{
		mCurrentPhoneme = match->item.phonemes.begin();
		mLastPhoneme = match->item.phonemes.end();
		return read();
	}

	if (next_phoneme(phoneme))
	{
		*(ipa::phoneme *)this = phoneme;
		return true;
	}

	return false;
}

bool phoneme_to_phoneme::next_phoneme(ipa::phoneme &phoneme)
{
	if (!mReadQueue.empty())
	{
		phoneme = mReadQueue.front();
		mReadQueue.pop_front();
		return true;
	}

	if (!mPhonemes->read())
		return false;

	phoneme = *mPhonemes;
	return true;
}

std::shared_ptr<tts::phoneme_reader>
tts::createPhonemeToPhonemeConverter(const char *aPhonemeToPhonemeRules,
                                     const std::shared_ptr<tts::phoneme_reader> &aPhonemes)
{
	return std::make_shared<phoneme_to_phoneme>(aPhonemeToPhonemeRules, aPhonemes);
}

std::shared_ptr<tts::phoneme_reader>
tts::createAccentConverter(const char *aAccent,
                           const std::shared_ptr<phoneme_reader> &aPhonemes)
{
	auto accent = cainteoir::get_data_path() / "accents" / (std::string(aAccent) + ".ptp");
	return std::make_shared<phoneme_to_phoneme>(accent, aPhonemes);
}
