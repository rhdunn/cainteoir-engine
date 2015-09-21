/* Text Processing API.
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

#ifndef CAINTEOIR_ENGINE_TEXT_HPP
#define CAINTEOIR_ENGINE_TEXT_HPP

#include "object.hpp"
#include "document.hpp"
#include "locale.hpp"
#include "dictionary.hpp"
#include "phoneme.hpp"

#include <list>

namespace cainteoir { namespace tts
{
	enum event_type
	{
		error,
		word_uppercase,
		word_lowercase,
		word_capitalized,
		word_mixedcase,
		word_script,
		number,
		ordinal_number,
		punctuation,
		symbol,
		paragraph,
		en_dash,
	};

	struct text_callback
	{
		virtual ~text_callback() {}

		virtual void onevent(const document_item &item) = 0;
	};

	struct text_reader
	{
		object token;

		virtual void reset(const std::shared_ptr<cainteoir::document_reader> &aReader) = 0;

		virtual bool read() = 0;

		virtual ~text_reader() {}
	};

	std::shared_ptr<text_reader>
	create_text_reader(text_callback *aCallback = nullptr);
}}

#endif
