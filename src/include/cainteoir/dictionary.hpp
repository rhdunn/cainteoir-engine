/* Pronunciation Dictionary API.
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

#ifndef CAINTEOIR_ENGINE_DICTIONARY_HPP
#define CAINTEOIR_ENGINE_DICTIONARY_HPP

#include "phoneme.hpp"

#include <list>
#include <unordered_map>

namespace cainteoir { namespace tts
{
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
			std::list<phoneme> phonemes;
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

		void add_entry(const key_type &aEntry,
		               entry_type aType,
		               std::shared_ptr<phoneme_reader> &phonemeset,
		               const std::shared_ptr<buffer> &aDefinition);

		const entry &lookup(const key_type &aEntry) const;

		std::size_t size()  const { return mEntries.size();  }
		bool        empty() const { return mEntries.empty(); }

		const_iterator begin() const { return mEntries.begin(); }
		const_iterator end()   const { return mEntries.end();   }

		const std::list<phoneme> &pronounce(const std::shared_ptr<buffer> &aText)
		{
			return pronounce(aText, 0);
		}
	private:
		const std::list<phoneme> &pronounce(const std::shared_ptr<buffer> &aText, int depth);

		storage_type mEntries;
	};

	bool parseCainteoirDictionary(tts::dictionary &dict,
	                              const char *aDictionaryPath);

	void writeCainteoirDictionary(tts::dictionary &dict,
	                              FILE *out,
	                              std::shared_ptr<tts::phoneme_writer> &writer,
	                              bool resolve_say_as_entries);
}}

#endif
