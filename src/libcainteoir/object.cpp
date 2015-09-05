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

const cainteoir::object::reference_t cainteoir::object::reference;

cainteoir::object::object(const std::shared_ptr<cainteoir::buffer> &aValue)
{
	if (aValue)
	{
		mType = cainteoir::object_type::buffer;
		new (&mBufferVal)buffer_t(aValue);
	}
	else
	{
		mType = cainteoir::object_type::null;
		mStringVal = {};
	}
}

cainteoir::object::object(const cainteoir::range<uint32_t> &aValue)
	: mType(cainteoir::object_type::range)
{
	new (&mRangeVal)range_t(aValue);
}

cainteoir::object::object(const object_type aType)
	: mType(aType)
{
	switch (aType)
	{
	case object_type::range:
		new (&mRangeVal)range_t(0, 0);
		break;
	case object_type::dictionary:
		new (&mDictionaryVal)dictionary_t(std::make_shared<dictionary_t::element_type>());
		break;
	case object_type::phoneme:
		mPhonemeVal = {};
		break;
	case object_type::string: // set an empty string to the null object type
	case object_type::buffer: // set an empty buffer to the null object type
	case object_type::buffer_ref: // set an empty buffer reference to the null object type
	case object_type::dictionary_ref: // set an empty dictionary reference to the null object type
		mType = object_type::null;
		mStringVal = {};
		break;
	default:
		mStringVal = {};
		break;
	}
}

cainteoir::object::object(const object &o)
{
	copy(o, nullptr);
}

cainteoir::object::object(const object &o, const reference_t &ref)
{
	copy(o, &ref);
}

cainteoir::object::~object()
{
	clear();
}

cainteoir::object &
cainteoir::object::operator=(const object &o)
{
	clear();
	copy(o, nullptr);
	return *this;
}

const cainteoir::object &
cainteoir::object::get(const char *aKey) const
{
	static const object nullobj;

	switch (mType)
	{
	case object_type::dictionary:
		{
			auto match = mDictionaryVal->find(aKey);
			if (match != mDictionaryVal->end())
				return (*match).second;
		}
		break;
	case object_type::dictionary_ref:
		{
			auto ptr = mDictionaryRef.lock();
			if (ptr)
			{
				auto match = ptr->find(aKey);
				if (match != ptr->end())
					return (*match).second;
			}
		}
		break;
	}
	return nullobj;
}

bool
cainteoir::object::put(const char *aKey, const object &aValue)
{
	switch (mType)
	{
	case object_type::dictionary:
		{
			(*mDictionaryVal)[aKey] = aValue;
			return true;
		}
		break;
	case object_type::dictionary_ref:
		{
			auto ptr = mDictionaryRef.lock();
			if (ptr)
			{
				(*ptr)[aKey] = aValue;
				return true;
			}
		}
		break;
	}
	return false;
}

std::size_t
cainteoir::object::size() const
{
	switch (mType)
	{
	case object_type::dictionary:
		return mDictionaryVal->size();
	case object_type::dictionary_ref:
		return mDictionaryRef.lock()->size();
	}
	return 0;
}

void
cainteoir::object::clear()
{
	switch (mType)
	{
	case object_type::buffer:
		(&mBufferVal)->~buffer_t();
		break;
	case object_type::buffer_ref:
		(&mBufferRef)->~buffer_ref_t();
		break;
	case object_type::range:
		(&mRangeVal)->~range_t();
		break;
	case object_type::dictionary:
		(&mDictionaryVal)->~dictionary_t();
		break;
	case object_type::dictionary_ref:
		(&mDictionaryRef)->~dictionary_ref_t();
		break;
	default:
		break;
	}
}

void
cainteoir::object::copy(const object &o, const reference_t *ref)
{
	switch (o.mType)
	{
	case object_type::buffer:
		if (ref)
		{
			mType = object_type::buffer_ref;
			new (&mBufferRef)buffer_ref_t(o.mBufferVal);
		}
		else
		{
			mType = o.mType;
			new (&mBufferVal)buffer_t(o.mBufferVal);
		}
		break;
	case object_type::buffer_ref:
		mType = o.mType;
		new (&mBufferRef)buffer_ref_t(o.mBufferRef);
		break;
	case object_type::range:
		mType = o.mType;
		new (&mRangeVal)range_t(o.mRangeVal);
		break;
	case object_type::dictionary:
		if (ref)
		{
			mType = object_type::dictionary_ref;
			new (&mDictionaryRef)dictionary_ref_t(o.mDictionaryVal);
		}
		else
		{
			mType = o.mType;
			new (&mDictionaryVal)dictionary_t(o.mDictionaryVal);
		}
		break;
	case object_type::dictionary_ref:
		mType = o.mType;
		new (&mDictionaryRef)dictionary_ref_t(o.mDictionaryRef);
		break;
	case object_type::phoneme:
		mType = o.mType;
		mPhonemeVal = o.mPhonemeVal;
		break;
	default:
		mType = o.mType;
		mStringVal = o.mStringVal;
		break;
	}
}
