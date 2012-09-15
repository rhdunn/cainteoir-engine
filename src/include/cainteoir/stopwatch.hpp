/* StopWatch Timer API.
 *
 * Copyright (C) 2012 Reece H. Dunn
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

#ifndef CAINTEOIR_ENGINE_STOPWATCH_HPP
#define CAINTEOIR_ENGINE_STOPWATCH_HPP

#define HAVE_GETTIMEOFDAY 1

#if HAVE_GETTIMEOFDAY

#include <sys/time.h>

namespace cainteoir
{
	class stopwatch
	{
	public:
		stopwatch() { mStart = timeofday(); }

		double elapsed() const { return timeofday() - mStart; }
	private:
		double mStart;

		static double timeofday()
		{
			timeval tv;
			gettimeofday(&tv, nullptr);

			return tv.tv_sec + (double(tv.tv_usec) / 1000000.0);
		}
	};
}

#else

#include <ctime>

namespace cainteoir
{
	class stopwatch
	{
	public:
		stopwatch() { mStart = time(nullptr); }

		double elapsed() const { return difftime(time(nullptr), mStart); }
	private:
		time_t mStart;
	};
}

#endif
#endif
