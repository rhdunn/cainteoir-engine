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
		virtual const char * lookup(uint8_t c) const = 0;

		virtual ~decoder() {}
	};

	class encoding
	{
	public:
		encoding(int aCodepage);

		void set_encoding(int aCodepage);

		const char * lookup(uint8_t c) const { return mDecoder->lookup(c); }
	private:
		std::tr1::shared_ptr<decoder> mDecoder;
	};
}

#endif
