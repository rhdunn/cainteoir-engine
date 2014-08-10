/* Lexer for Cainteoir file formats.
 *
 * Copyright (C) 2014 Reece H. Dunn
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

#ifndef CAINTEOIR_ENGINE_INTERNAL_CAINTEOIR_FILE_READER_HPP
#define CAINTEOIR_ENGINE_INTERNAL_CAINTEOIR_FILE_READER_HPP

#include <cainteoir/buffer.hpp>
#include <cainteoir/path.hpp>

struct cainteoir_file_reader
{
	enum token_type
	{
		end_of_file,
		directive,
		directive_contents,
		text,
		phonemes,
	};

	cainteoir_file_reader(const cainteoir::path &aFilePath);

	bool read();

	const token_type type() const { return mType; }

	const cainteoir::buffer &match() const { return mMatch; }
private:
	std::shared_ptr<cainteoir::buffer> mData;
	const char *mCurrent;
	const char *mLast;
	int mState;

	token_type mType;
	cainteoir::buffer mMatch;
};

#endif
