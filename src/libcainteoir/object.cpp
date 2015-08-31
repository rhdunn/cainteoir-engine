/* Multi-type Object API.
 *
 * Copyright (C) 2015 Reece H. Dunn
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

#include "config.h"
#include "compatibility.hpp"

#include <cainteoir/object.hpp>

cainteoir::object::object(const std::shared_ptr<cainteoir::buffer> &aValue)
	: mType(cainteoir::object_type::buffer)
{
	new (&mBufferVal)buffer_t(aValue);
}

cainteoir::object::object(const cainteoir::range<uint32_t> &aValue)
	: mType(cainteoir::object_type::range)
{
	new (&mRangeVal)range_t(aValue);
}

cainteoir::object::object(const object &o)
	: mType(o.mType)
{
	switch (o.mType)
	{
	case object_type::buffer:
		new (&mBufferVal)buffer_t(o.mBufferVal);
		break;
	case object_type::range:
		new (&mRangeVal)range_t(o.mRangeVal);
		break;
	case object_type::phoneme:
		mPhonemeVal = o.mPhonemeVal;
		break;
	default:
		mStringVal = o.mStringVal;
		break;
	}
}

cainteoir::object::~object()
{
	switch (mType)
	{
	case object_type::buffer:
		(&mBufferVal)->~buffer_t();
		break;
	case object_type::range:
		(&mRangeVal)->~range_t();
		break;
	default:
		break;
	}
}

cainteoir::object &
cainteoir::object::operator=(const object &o)
{
	switch (mType)
	{
	case object_type::buffer:
		(&mBufferVal)->~buffer_t();
		break;
	case object_type::range:
		(&mRangeVal)->~range_t();
		break;
	default:
		break;
	}

	mType = o.mType;
	switch (o.mType)
	{
	case object_type::buffer:
		new (&mBufferVal)buffer_t(o.mBufferVal);
		break;
	case object_type::range:
		new (&mRangeVal)range_t(o.mRangeVal);
		break;
	case object_type::phoneme:
		mPhonemeVal = o.mPhonemeVal;
		break;
	default:
		mStringVal = o.mStringVal;
		break;
	}
	return *this;
}
