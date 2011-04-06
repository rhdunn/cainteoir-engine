/* Audio Output.
 *
 * Copyright (C) 2011 Reece H. Dunn
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

#include <cainteoir/audio.hpp>
#include <string.h>

namespace rdf = cainteoir::rdf;

std::auto_ptr<cainteoir::audio> create_wav_file(const char *filename, cainteoir::audio_format format, int channels, int frequency, float quality, const rdf::graph &aMetadata, const rdf::uri &aDocument);

std::auto_ptr<cainteoir::audio> create_ogg_file(const char *filename, cainteoir::audio_format format, int channels, int frequency, float quality, const rdf::graph &aMetadata, const rdf::uri &aDocument);

std::auto_ptr<cainteoir::audio> create_pulseaudio_device(const char *device, cainteoir::audio_format format, int channels, int frequency, float quality, const rdf::graph &aMetadata, const rdf::uri &aDocument);


void cainteoir::supportedAudioFormats(rdf::graph &metadata)
{
	std::string baseuri = "http://rhdunn.github.com/cainteoir/formats/audio";

	rdf::uri wav = rdf::uri(baseuri, "wav");
	metadata.push_back(rdf::statement(wav, rdf::rdf("type"), rdf::tts("AudioFormat")));
	metadata.push_back(rdf::statement(wav, rdf::dc("title"), rdf::literal("wave audio")));
	metadata.push_back(rdf::statement(wav, rdf::tts("mimetype"), rdf::literal("audio/vnd.wav")));
	metadata.push_back(rdf::statement(wav, rdf::tts("mimetype"), rdf::literal("audio/wav")));
	metadata.push_back(rdf::statement(wav, rdf::tts("mimetype"), rdf::literal("audio/wave")));
	metadata.push_back(rdf::statement(wav, rdf::tts("mimetype"), rdf::literal("audio/x-wav")));
	metadata.push_back(rdf::statement(wav, rdf::tts("extension"), rdf::literal("*.wav")));

	rdf::uri ogg = rdf::uri(baseuri, "ogg");
	metadata.push_back(rdf::statement(ogg, rdf::rdf("type"), rdf::tts("AudioFormat")));
	metadata.push_back(rdf::statement(ogg, rdf::dc("title"), rdf::literal("ogg+vorbis audio")));
	metadata.push_back(rdf::statement(ogg, rdf::tts("mimetype"), rdf::literal("application/ogg")));
	metadata.push_back(rdf::statement(ogg, rdf::tts("mimetype"), rdf::literal("audio/ogg")));
	metadata.push_back(rdf::statement(ogg, rdf::tts("extension"), rdf::literal("*.ogg")));
}

std::auto_ptr<cainteoir::audio> cainteoir::create_audio_file(const char *filename, const char *type, cainteoir::audio_format format, int channels, int frequency, float quality, const rdf::graph &aMetadata, const rdf::uri &aDocument)
{
	if (filename && !strcmp(filename, "-"))
		filename = NULL;

	if (!strcmp(type, "wav"))
		return create_wav_file(filename, format, channels, frequency, quality, aMetadata, aDocument);
	if (!strcmp(type, "ogg"))
		return create_ogg_file(filename, format, channels, frequency, quality, aMetadata, aDocument);

	return std::auto_ptr<cainteoir::audio>();
}

std::auto_ptr<cainteoir::audio> cainteoir::open_audio_device(const char *device, const char *type, cainteoir::audio_format format, int channels, int frequency, float quality, const rdf::graph &aMetadata, const rdf::uri &aDocument)
{
	if (!strcmp(type, "pulse"))
		return create_pulseaudio_device(device, format, channels, frequency, quality, aMetadata, aDocument);

	return std::auto_ptr<cainteoir::audio>();
}

