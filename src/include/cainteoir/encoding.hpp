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
#ifndef DOXYGEN
	namespace detail
	{
		struct decoder
		{
			virtual void decode(const cainteoir::buffer &data, cainteoir::rope &decoded) const = 0;

			virtual ~decoder() {}
		};
	}
#endif

	struct encoding
	{
		encoding(int aCodepage);

		encoding(const char *aEncoding);

		bool set_encoding(int aCodepage);

		bool set_encoding(const char *aEncoding);

		std::shared_ptr<cainteoir::buffer> lookup(uint8_t c) const;

		std::shared_ptr<cainteoir::buffer> decode(const std::shared_ptr<cainteoir::buffer> &data) const;

		void decode(const std::shared_ptr<cainteoir::buffer> &data, cainteoir::rope &decoded) const;
	private:
		std::shared_ptr<detail::decoder> mDecoder;
		std::string mEncoding;
	};
}

#endif
