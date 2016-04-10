/* Pronunciation Dictionary API.
 *
 * Copyright (C) 2013-2015 Reece H. Dunn
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

#ifndef CAINTEOIR_ENGINE_DICTIONARY_HPP
#define CAINTEOIR_ENGINE_DICTIONARY_HPP

#include "phoneme.hpp"
#include "object.hpp"

#include <list>
#include <unordered_map>

namespace cainteoir { namespace tts
{
	struct multiword_entry
	{
		struct value_type
		{
			tts::initial_stress stress;
			std::shared_ptr<cainteoir::buffer> word;
		};

		typedef const value_type &const_reference;

		enum mode_t
		{
			hyphenated,
			stressed,
		};

		multiword_entry(const std::shared_ptr<cainteoir::buffer> &aText, mode_t aMode)
			: first(aText->begin())
			, last(aText->end())
			, next(first)
			, mCurrentWord(0)
			, mMode(aMode)
		{
			mValue.stress = tts::initial_stress::as_transcribed;
			advance();
		}

		bool is_multiword() const { return next != last; }

		size_t position() const { return mCurrentWord; }

		bool have_word() const { return first != last; }

		void operator++();

		const_reference operator*() const { return mValue; }
	private:
		void advance();

		const char *first;
		const char *last;
		const char *next;

		value_type mValue;
		size_t mCurrentWord;
		mode_t mMode;
	};

	struct dictionary
	{
		typedef std::shared_ptr<buffer> key_type;

		struct key_hash : public std::unary_function<key_type, std::size_t>
		{
			std::size_t operator()(const key_type &a) const;
		};

		struct key_equals : public std::binary_function<key_type, key_type, bool>
		{
			bool operator()(const key_type &a, const key_type &b) const
			{
				return a->compare(*b) == 0;
			}
		};

		typedef std::unordered_map<key_type, object, key_hash, key_equals> storage_type;
		typedef storage_type::const_iterator const_iterator;

		void add_entry(const key_type &aWord, const object &aEntry);
		void add_entry(const key_type &aWord, const std::shared_ptr<buffer> &aEntry);
		void add_entry(const key_type &aWord, const ipa::phonemes &aEntry);

		const object &lookup(const key_type &aWord) const;

		std::size_t size()  const { return mEntries.size();  }
		bool        empty() const { return mEntries.empty(); }

		const_iterator begin() const { return mEntries.begin(); }
		const_iterator end()   const { return mEntries.end();   }

		bool pronounce(const std::shared_ptr<buffer> &aWord,
		               const std::shared_ptr<tts::phoneme_reader> &aPronunciationRules,
		               ipa::phonemes &aPhonemes)
		{
			return pronounce(aWord, aPronunciationRules, aPhonemes, 0);
		}
	private:
		bool pronounce(const std::shared_ptr<buffer> &aWord,
		               const std::shared_ptr<tts::phoneme_reader> &aPronunciationRules,
		               ipa::phonemes &aPhonemes,
		               int depth);

		storage_type mEntries;
	};

	struct dictionary_reader
	{
		std::shared_ptr<cainteoir::buffer> word;
		object entry;

		virtual bool read() = 0;

		virtual ~dictionary_reader() {}
	};

	std::shared_ptr<dictionary_reader> createDictionaryReader(const char *aDictionaryPath);

	struct dictionary_formatter
	{
		virtual void write_phoneme_entry(const std::shared_ptr<cainteoir::buffer> &word,
		                                 std::shared_ptr<tts::phoneme_writer> &writer,
		                                 const ipa::phonemes &phonemes,
	                                         const object &entry,
		                                 const char *line_separator = "\n") = 0;

		virtual void write_say_as_entry(const std::shared_ptr<cainteoir::buffer> &word,
                                                const std::shared_ptr<cainteoir::buffer> &say_as,
	                                        const object &entry,
		                                const char *line_separator = "\n") = 0;

		virtual ~dictionary_formatter() {}
	};

	std::shared_ptr<dictionary_formatter> createDictionaryFormatter(FILE *out, const char *aFormat);

	void formatDictionary(tts::dictionary &dict,
	                      std::shared_ptr<dictionary_formatter> &formatter,
	                      std::shared_ptr<phoneme_writer> &writer,
	                      bool resolve_say_as_entries);

	void formatDictionary(tts::dictionary &dict,
	                      std::shared_ptr<dictionary_formatter> &formatter,
	                      std::shared_ptr<phoneme_reader> &reader,
	                      std::shared_ptr<phoneme_writer> &writer);
}}

#endif
