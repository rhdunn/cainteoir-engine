/* PulseAudio Support.
 *
 * Copyright (C) 2010-2012 Reece H. Dunn
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
#include "i18n.h"

#include <cainteoir/audio.hpp>

namespace rdf = cainteoir::rdf;

#ifdef HAVE_PULSEAUDIO

#include <stdexcept>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <string.h>
#include <stdio.h>

class pulse_audio : public cainteoir::audio
{
	pa_simple *pa;
	pa_sample_spec ss;
	const char *m_device;
public:
	pulse_audio(const char *device, const rdf::uri &format, int channels, int frequency)
		: pa(nullptr)
		, m_device(device)
	{
		if (format == rdf::tts("s16le"))
			ss.format = PA_SAMPLE_S16LE;
		else if (format == rdf::tts("float32le"))
			ss.format = PA_SAMPLE_FLOAT32LE;
		else
			throw std::runtime_error(i18n("pulseaudio: unsupported audio format."));

		ss.channels = channels;
		ss.rate = frequency;
	}

	~pulse_audio()
	{
		close();
	}

	void open()
	{
		if (pa) return;

		int error = 0;
		pa = pa_simple_new(m_device, i18n("Cainteoir Text-to-Speech"), PA_STREAM_PLAYBACK, nullptr, "Music", &ss, nullptr, nullptr, &error);
		if (!pa)
			throw std::runtime_error(std::string(i18n("pulseaudio: ")) + pa_strerror(error));
	}

	void close()
	{
		if (pa)
		{
			pa_simple_free(pa);
			pa = nullptr;
		}
	}

	uint32_t write(const char *data, uint32_t len)
	{
		return pa_simple_write(pa, data, len, nullptr);
	}
};

std::shared_ptr<cainteoir::audio>
create_pulseaudio_device(const char *device, const rdf::uri &format, int channels, int frequency, const rdf::graph &aMetadata, const rdf::uri &aDocument)
{
	return std::make_shared<pulse_audio>(device, format, channels, frequency);
}

#else

std::shared_ptr<cainteoir::audio>
create_pulseaudio_device(const char *device, const rdf::uri &format, int channels, int frequency, const rdf::graph &aMetadata, const rdf::uri &aDocument)
{
	return {};
}

#endif
