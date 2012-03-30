/* Encoding Conversion Support.
 *
 * Copyright (C) 2011 Reece H. Dunn
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

#ifndef CAINTEOIR_ENGINE_ENCODING_HPP
#define CAINTEOIR_ENGINE_ENCODING_HPP

#include "buffer.hpp"

namespace cainteoir
{
	struct decoder
	{
		virtual std::shared_ptr<cainteoir::buffer> decode(const cainteoir::buffer &data) const = 0;

		virtual ~decoder() {}
	};

	class encoding
	{
	public:
		encoding(int aCodepage);

		encoding(const char *aEncoding);

		/** @brief Set the character encoding to the specified Windows codepage.
		  *
		  * @param aCodepage The Windows codepage to change to.
		  */
		void set_encoding(int aCodepage);

		/** @brief Set the character encoding.
		  *
		  * @param aEncoding The encoding to change to.
		  */
		void set_encoding(const char *aEncoding);

		/** @brief Lookup the single-byte character.
		  *
		  * @param c The character to lookup.
		  *
		  * @return The utf-8 representation of c.
		  */
		std::shared_ptr<cainteoir::buffer> lookup(uint8_t c) const
		{
			return mDecoder->decode(cainteoir::buffer((char *)&c, (char *)&c + 1));
		}

		/** @brief Convert the data buffer to utf-8.
		  *
		  * @param data The character buffer to convert.
		  *
		  * @return The utf-8 representation of data.
		  */
		std::shared_ptr<cainteoir::buffer> decode(const cainteoir::buffer &data) const
		{
			return mDecoder->decode(data);
		}
	private:
		std::shared_ptr<decoder> mDecoder;
	};
}

#endif
