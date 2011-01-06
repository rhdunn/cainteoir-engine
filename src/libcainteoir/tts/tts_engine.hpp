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

#include <cainteoir/buffer.hpp>
#include <cainteoir/audio.hpp>
#include <cainteoir/metadata.hpp>
#include <memory>

namespace cainteoir { namespace tts
{
	struct engine
	{
		virtual ~engine() {}

		virtual int get_channels() const = 0;
		virtual int get_frequency() const = 0;
		virtual cainteoir::audio_format get_audioformat() const = 0;

		virtual bool select_voice(const char *voicename) = 0;

		virtual void speak(buffer *text, audio *out) = 0;
	};

	engine *create_espeak_engine(rdf::graph &aMetadata, std::string &uri);
}}

#endif
