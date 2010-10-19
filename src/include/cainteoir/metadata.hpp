/* MetaData API.
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

#ifndef CAINTEOIR_ENGINE_METADATA_HPP
#define CAINTEOIR_ENGINE_METADATA_HPP

namespace cainteoir
{
	enum metadata_tag
	{
		/** @brief The title of the document, chapter, voice, text-to-speech engine, etc. */
		title,

		/** @brief The number of audio channels (mono = 1, stereo = 2). */
		channels,

		/** @brief The frequency (in Hz) of the audio data. */
		frequency,

		/** @brief The format of the audio data: S16_LE, S16_BE, S32_LE, etc. */
		audio_format,
	};

	struct metadata
	{
		virtual ~metadata() {}

		virtual const char *get_metadata(metadata_tag tag) const = 0;
	};
}

#endif
