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

#ifndef CAINTEOIR_ENGINE_OBJECT_HPP
#define CAINTEOIR_ENGINE_OBJECT_HPP

namespace cainteoir
{
	enum object_type : uint16_t
	{
		null,
		boolean,
		integer,
		real,
	};

	struct object
	{
#if (INTPTR_MAX == INT64_MAX) // 64-bit
		typedef int64_t intval_t;
		typedef double  floatval_t;
#else // 32-bit
		typedef int32_t intval_t;
		typedef float   floatval_t;
#endif

		object()
			: mType(object_type::null)
			, mIntVal(0)
		{
		}

		object(bool aValue)
			: mType(object_type::boolean)
			, mBoolVal(aValue)
		{
		}

		object(int32_t aValue)
			: mType(object_type::integer)
			, mIntVal(aValue)
		{
		}

#if (INTPTR_MAX == INT64_MAX) // 64-bit
		object(int64_t aValue)
			: mType(object_type::integer)
			, mIntVal(aValue)
		{
		}
#endif

		object(float aValue)
			: mType(object_type::real)
			, mFloatVal(aValue)
		{
		}

#if (INTPTR_MAX == INT64_MAX) // 64-bit
		object(double aValue)
			: mType(object_type::real)
			, mFloatVal(aValue)
		{
		}
#endif

		const object_type type() const { return mType; }

		bool boolean() const { return mBoolVal; }

		intval_t integer() const { return mIntVal; }

		floatval_t real() const { return mFloatVal; }
	private:
		object_type mType;
		union
		{
			bool mBoolVal;
			intval_t mIntVal;
			floatval_t mFloatVal;
		};
	};
}

#endif
