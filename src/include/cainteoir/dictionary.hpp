/* Pronunciation Dictionary API.
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

#ifndef CAINTEOIR_ENGINE_DICTIONARY_HPP
#define CAINTEOIR_ENGINE_DICTIONARY_HPP

#include "phoneme.hpp"

#include <list>
#include <unordered_map>

namespace cainteoir { namespace tts
{
	struct multiword_entry
	{
		multiword_entry(const std::shared_ptr<cainteoir::buffer> &aText)
			: first(aText->begin())
			, last(aText->end())
			, next(first)
		{
			advance();
		}

		bool is_multiword() const { return next != last; }

		bool have_word() const { return first != last; }

		std::shared_ptr<cainteoir::buffer> next_word();
	private:
		void advance();

		const char *first;
		const char *last;
		const char *next;
	};

	struct dictionary
	{
		enum entry_type
		{
			no_match,
			say_as,
			phonemes,
		};

		struct entry
		{
			entry_type type;
			std::shared_ptr<buffer> text;
			std::list<ipa::phoneme> phonemes;

			entry()
				: type(dictionary::no_match)
			{
			}

			entry(const std::shared_ptr<buffer> &aSayAs)
				: type(dictionary::say_as)
				, text(aSayAs)
			{
			}

			entry(const std::shared_ptr<buffer> &aPhonemes, std::shared_ptr<phoneme_reader> &aPhonemeSet);
		};

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

		typedef std::unordered_map<key_type, entry, key_hash, key_equals> storage_type;
		typedef storage_type::const_iterator const_iterator;

		void add_entry(const key_type &aWord, const entry &aEntry);

		const entry &lookup(const key_type &aWord) const;

		std::size_t size()  const { return mEntries.size();  }
		bool        empty() const { return mEntries.empty(); }

		const_iterator begin() const { return mEntries.begin(); }
		const_iterator end()   const { return mEntries.end();   }

		bool pronounce(const std::shared_ptr<buffer> &aWord,
		               const std::shared_ptr<tts::phoneme_reader> &aPronunciationRules,
		               std::list<ipa::phoneme> &aPhonemes)
		{
			return pronounce(aWord, aPronunciationRules, aPhonemes, 0);
		}
	private:
		bool pronounce(const std::shared_ptr<buffer> &aWord,
		               const std::shared_ptr<tts::phoneme_reader> &aPronunciationRules,
		               std::list<ipa::phoneme> &aPhonemes,
		               int depth);

		storage_type mEntries;
	};

	struct dictionary_reader
	{
		std::shared_ptr<cainteoir::buffer> word;
		tts::dictionary::entry entry;

		virtual bool read() = 0;

		virtual ~dictionary_reader() {}
	};

	std::shared_ptr<dictionary_reader> createCainteoirDictionaryReader(const char *aDictionaryPath);

	struct dictionary_formatter
	{
		virtual void write_phoneme_entry(const std::shared_ptr<cainteoir::buffer> &word,
		                                 std::shared_ptr<tts::phoneme_writer> &writer,
		                                 const std::list<ipa::phoneme> &phonemes,
		                                 const char *line_separator = "\n") = 0;

		virtual void write_say_as_entry(const std::shared_ptr<cainteoir::buffer> &word,
                                                const std::shared_ptr<cainteoir::buffer> &say_as,
		                                const char *line_separator = "\n") = 0;

		virtual ~dictionary_formatter() {}
	};

	std::shared_ptr<dictionary_formatter> createCainteoirDictionaryFormatter(FILE *out);

	std::shared_ptr<dictionary_formatter> createEspeakDictionaryFormatter(FILE *out);

	std::shared_ptr<dictionary_formatter> createDictionaryEntryFormatter(FILE *out);

	void formatDictionary(tts::dictionary &dict,
	                      std::shared_ptr<dictionary_formatter> &formatter,
	                      std::shared_ptr<phoneme_writer> &writer,
	                      bool resolve_say_as_entries);
}}

#endif
