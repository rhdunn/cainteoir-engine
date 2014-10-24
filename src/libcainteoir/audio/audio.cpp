/* Audio Output.
 *
 * Copyright (C) 2011-2013 Reece H. Dunn
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

#include <cainteoir/audio.hpp>
#include <cainteoir/sigproc.hpp>
#include <cainteoir/mimetype.hpp>
#include <stdexcept>
#include <string.h>

namespace rdf = cainteoir::rdf;
namespace rql = cainteoir::rdf::query;
namespace mime = cainteoir::mime;

std::shared_ptr<cainteoir::audio>
create_wav_file(const char *filename, const rdf::uri &format, int channels, int frequency, float quality, const rdf::graph &aMetadata, const rdf::uri &aDocument);

std::shared_ptr<cainteoir::audio>
create_ogg_file(const char *filename, const rdf::uri &format, int channels, int frequency, float quality, const rdf::graph &aMetadata, const rdf::uri &aDocument);

std::shared_ptr<cainteoir::audio>
create_alsa_device(const char *device, const rdf::uri &format, int channels, int frequency, const rdf::graph &aMetadata, const rdf::uri &aDocument);

std::shared_ptr<cainteoir::audio>
create_pulseaudio_device(const char *device, const rdf::uri &format, int channels, int frequency, const rdf::graph &aMetadata, const rdf::uri &aDocument);

void cainteoir::supported_audio_formats(rdf::graph &metadata)
{
	std::string baseuri = "http://rhdunn.github.com/cainteoir/formats/audio";

	mime::wav.metadata(metadata, baseuri, rdf::tts("AudioFormat"));
	mime::ogg.metadata(metadata, baseuri, rdf::tts("AudioFormat"));
}

struct audio_info_t : public cainteoir::audio_info
{
	int channels() const { return mChannels; }

	int frequency() const { return mFrequency; }

	const rdf::uri &format() const { return mFormat; }

	audio_info_t(const rdf::uri &aFormat,
	             int aChannels,
	             int aFrequency)
		: mFormat(aFormat)
		, mChannels(aChannels)
		, mFrequency(aFrequency)
	{
	}
private:
	rdf::uri mFormat;
	int mChannels;
	int mFrequency;
};

std::shared_ptr<cainteoir::audio_info>
cainteoir::create_audio_info(
	const rdf::uri &aFormat,
	int aChannels,
	int aFrequency)
{
	return std::make_shared<audio_info_t>(aFormat, aChannels, aFrequency);
}

std::shared_ptr<cainteoir::audio>
cainteoir::create_audio_file(
	const char *filename,
	const char *type,
	float quality,
	const rdf::graph &aDocMetadata,
	const rdf::uri &aDocument,
	const rdf::graph &aVoiceMetadata,
	const rdf::uri &aVoice)
{
	rql::results data = rql::select(aVoiceMetadata, rql::subject == aVoice);
	int channels  = rql::select_value<int>(data, rql::predicate == rdf::tts("channels"));
	int frequency = rql::select_value<int>(data, rql::predicate == rdf::tts("frequency"));
	const rdf::uri &format = rql::object(rql::select(data, rql::predicate == rdf::tts("audio-format")).front());

	return create_audio_file(filename, type, quality, aDocMetadata, aDocument, format, channels, frequency);
}

std::shared_ptr<cainteoir::audio>
cainteoir::create_audio_file(
	const char *filename,
	const char *type,
	float quality,
	const rdf::graph &aDocMetadata,
	const rdf::uri &aDocument,
	const rdf::uri &aFormat,
	int aChannels,
	int aFrequency)
{
	if (filename && !strcmp(filename, "-"))
		filename = nullptr;

	if (!strcmp(type, "wav"))
		return create_wav_file(filename, aFormat, aChannels, aFrequency, quality, aDocMetadata, aDocument);
	if (!strcmp(type, "ogg"))
		return create_ogg_file(filename, aFormat, aChannels, aFrequency, quality, aDocMetadata, aDocument);

	return std::shared_ptr<cainteoir::audio>();
}

std::shared_ptr<cainteoir::audio>
cainteoir::open_audio_device(
	const char *device,
	const rdf::graph &aDocMetadata,
	const rdf::uri &aDocument,
	const rdf::graph &aVoiceMetadata,
	const rdf::uri &aVoice)
{
	rql::results data = rql::select(aVoiceMetadata, rql::subject == aVoice);
	int channels  = rql::select_value<int>(data, rql::predicate == rdf::tts("channels"));
	int frequency = rql::select_value<int>(data, rql::predicate == rdf::tts("frequency"));
	const rdf::uri &format = rql::object(rql::select(data, rql::predicate == rdf::tts("audio-format")).front());

	return open_audio_device(device, aDocMetadata, aDocument, format, channels, frequency);
}

std::shared_ptr<cainteoir::audio>
cainteoir::open_audio_device(
	const char *device,
	const rdf::graph &aDocMetadata,
	const rdf::uri &aDocument,
	const rdf::uri &aFormat,
	int aChannels,
	int aFrequency)
{
	std::shared_ptr<cainteoir::audio> audio;
	try
	{
		audio = create_pulseaudio_device(device, aFormat, aChannels, aFrequency, aDocMetadata, aDocument);
	}
	catch (const std::runtime_error &e)
	{
		// Failed to create a pulseaudio stream. This is usually when pulseaudio is
		// not running (i.e. it throws a |connection refused| error).
	}

	if (!audio.get())
		audio = create_alsa_device(device, aFormat, aChannels, aFrequency, aDocMetadata, aDocument);
	return audio;
}

cainteoir::audio_data<short>
cainteoir::read_s16_samples(const std::shared_ptr<cainteoir::buffer> &aData,
                            const css::time &aStart,
                            const css::time &aEnd,
                            int aChannel,
                            int aFrequency)
{
	auto player = cainteoir::create_media_reader(aData);
	if (!player)
		throw std::runtime_error("unable to read the audio file");

	cainteoir::audio_data<short> data;
	data.info = cainteoir::create_audio_info(
		rdf::tts("s16le"),
		player->channels(),
		(aFrequency == -1) ? player->frequency() : aFrequency);

	player->set_interval(aStart, aEnd);
	player->set_target(data.info);
	while (player->read())
	{
		const short *current = (const short *)player->data.begin();
		const short *last    = (const short *)player->data.end();
		int channels = player->channels();
		switch (channels)
		{
		case 1:
			data.samples.insert(data.samples.end(), current, last);
			break;
		default:
			{
				uint32_t n = 0;
				for (; current != last; ++current)
				{
					if (n % channels == aChannel)
						data.samples.push_back(*current);
					++n;
				}
			}
			break;
		}
	}

	return data;
}
