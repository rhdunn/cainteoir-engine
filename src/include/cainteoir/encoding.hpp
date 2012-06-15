/* Encoding Conversion Support.
 *
 * Copyright (C) 2011-2012 Reece H. Dunn
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
		virtual void decode(const cainteoir::buffer &data, cainteoir::rope &decoded) const = 0;

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
		  *
		  * @return true if the encoding was changed, false if the encoding is unchanged.
		  */
		bool set_encoding(int aCodepage);

		/** @brief Set the character encoding.
		  *
		  * @param aEncoding The encoding to change to.
		  *
		  * @return true if the encoding was changed, false if the encoding is unchanged.
		  */
		bool set_encoding(const char *aEncoding);

		/** @brief Lookup the single-byte character.
		  *
		  * @param c The character to lookup.
		  *
		  * @return The utf-8 representation of c.
		  */
		std::shared_ptr<cainteoir::buffer> lookup(uint8_t c) const;

		/** @brief Convert the data buffer to utf-8.
		  *
		  * @param data The character buffer to convert.
		  *
		  * @return The utf-8 representation of data.
		  */
		std::shared_ptr<cainteoir::buffer> decode(const std::shared_ptr<cainteoir::buffer> &data) const;

		/** @brief Convert the data buffer to utf-8.
		  *
		  * @param data The character buffer to convert.
		  * @param decoded The rope to add the utf-8 representation of data to.
		  */
		void decode(const std::shared_ptr<cainteoir::buffer> &data, cainteoir::rope &decoded) const;
	private:
		std::shared_ptr<decoder> mDecoder;
		std::string mEncoding;
	};
}

#endif
