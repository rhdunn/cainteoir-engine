/* Text-to-Speech Engine API.
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

#ifndef CAINTEOIR_ENGINE_TTS_ENGINE_HPP
#define CAINTEOIR_ENGINE_TTS_ENGINE_HPP

#include "buffer.hpp"
#include "audio.hpp"
#include "metadata.hpp"
#include <memory>

namespace cainteoir
{
	struct tts_engine : public metadata
	{
		virtual ~tts_engine() {}

		virtual bool set_voice_by_name(const char *name) = 0;

		virtual void speak(buffer *text) = 0;
	};

	std::auto_ptr<tts_engine> create_espeak_engine();
}

#endif
