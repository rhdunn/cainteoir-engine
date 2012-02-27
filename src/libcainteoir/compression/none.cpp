/* Compressed Stream (No Compression)
 *
 * Copyright (C) 2010 Reece H. Dunn
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

#include <cainteoir/buffer.hpp>

std::shared_ptr<cainteoir::buffer> cainteoir::copy(const cainteoir::buffer &compressed, uint32_t uncompressed)
{
	std::shared_ptr<cainteoir::buffer> data = std::make_shared<cainteoir::data_buffer>(compressed.size());
	memcpy((char *)data->begin(), compressed.begin(), compressed.size());
	return data;
}
