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

struct format_info_t
{
	const char *format;
	pa_sample_format pa_format;
};

static const format_info_t format_info[] =
{
	{ "u8",        PA_SAMPLE_U8 },
	{ "s16le",     PA_SAMPLE_S16LE },
	{ "s16be",     PA_SAMPLE_S16BE },
	{ "s32le",     PA_SAMPLE_S32LE },
	{ "s32be",     PA_SAMPLE_S32BE },
	{ "float32le", PA_SAMPLE_FLOAT32LE },
	{ "float32be", PA_SAMPLE_FLOAT32BE },
};

class pulse_audio : public cainteoir::audio
{
	pa_simple *pa;
	pa_sample_spec ss;
	const char *m_device;
	const rdf::uri mFormat;
public:
	pulse_audio(const char *device, const rdf::uri &format, int channels, int frequency)
		: pa(nullptr)
		, m_device(device)
		, mFormat(format)
	{
		if (format.ns == rdf::tts.href) for (const auto &info : format_info)
		{
			if (format.ref == info.format)
			{
				ss.format = info.pa_format;
				ss.channels = channels;
				ss.rate = frequency;

				// Cainteoir may be built with the pulseaudio libraries, but the pulseaudio
				// server may not be running. In that case, calling |open| will fail due to
				// a |connection refused| error. Call |open| here so that error can be
				// trapped in the |create_audio| function.
				open();

				return;
			}
		}
		throw std::runtime_error(i18n("unsupported audio format."));
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
			pa_simple_drain(pa, nullptr);
			pa_simple_free(pa);
			pa = nullptr;
		}
	}

	uint32_t write(const char *data, uint32_t len)
	{
		return pa_simple_write(pa, data, len, nullptr);
	}

	int channels() const { return ss.channels; }

	int frequency() const { return ss.rate; }

	const rdf::uri &format() const { return mFormat; }
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
