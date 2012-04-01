/* Audio Output.
 *
 * Copyright (C) 2011-2012 Reece H. Dunn
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
#include <cainteoir/mimetype.hpp>
#include <string.h>

namespace rdf = cainteoir::rdf;
namespace rql = cainteoir::rdf::query;
namespace mime = cainteoir::mime;

std::shared_ptr<cainteoir::audio>
create_wav_file(const char *filename, const rdf::uri &format, int channels, int frequency, float quality, const rdf::graph &aMetadata, const rdf::uri &aDocument);

std::shared_ptr<cainteoir::audio>
create_ogg_file(const char *filename, const rdf::uri &format, int channels, int frequency, float quality, const rdf::graph &aMetadata, const rdf::uri &aDocument);

std::shared_ptr<cainteoir::audio>
create_pulseaudio_device(const char *device, const rdf::uri &format, int channels, int frequency, float quality, const rdf::graph &aMetadata, const rdf::uri &aDocument);

void cainteoir::supportedAudioFormats(rdf::graph &metadata)
{
	std::string baseuri = "http://rhdunn.github.com/cainteoir/formats/audio";

	mime::wav.metadata(metadata, baseuri, rdf::tts("AudioFormat"));
	mime::ogg.metadata(metadata, baseuri, rdf::tts("AudioFormat"));
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
	if (filename && !strcmp(filename, "-"))
		filename = nullptr;

	rql::results data = rql::select(aVoiceMetadata, rql::matches(rql::subject, aVoice));
	int channels  = rql::select_value<int>(data, rql::matches(rql::predicate, rdf::tts("channels")));
	int frequency = rql::select_value<int>(data, rql::matches(rql::predicate, rdf::tts("frequency")));
	const rdf::uri &format = rql::object(rql::select(data, rql::matches(rql::predicate, rdf::tts("audio-format"))).front());

	if (!strcmp(type, "wav"))
		return create_wav_file(filename, format, channels, frequency, quality, aDocMetadata, aDocument);
	if (!strcmp(type, "ogg"))
		return create_ogg_file(filename, format, channels, frequency, quality, aDocMetadata, aDocument);

	return std::shared_ptr<cainteoir::audio>();
}

std::shared_ptr<cainteoir::audio>
cainteoir::open_audio_device(
	const char *device,
	const char *type,
	float quality,
	const rdf::graph &aDocMetadata,
	const rdf::uri &aDocument,
	const rdf::graph &aVoiceMetadata,
	const rdf::uri &aVoice)
{
	rql::results data = rql::select(aVoiceMetadata, rql::matches(rql::subject, aVoice));
	int channels  = rql::select_value<int>(data, rql::matches(rql::predicate, rdf::tts("channels")));
	int frequency = rql::select_value<int>(data, rql::matches(rql::predicate, rdf::tts("frequency")));
	const rdf::uri &format = rql::object(rql::select(data, rql::matches(rql::predicate, rdf::tts("audio-format"))).front());

	if (!strcmp(type, "pulse"))
		return create_pulseaudio_device(device, format, channels, frequency, quality, aDocMetadata, aDocument);

	return std::shared_ptr<cainteoir::audio>();
}

