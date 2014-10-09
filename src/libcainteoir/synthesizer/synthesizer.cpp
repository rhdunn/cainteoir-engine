/* Text-to-Speech Synthesizers.
 *
 * Copyright (C) 2014 Reece H. Dunn
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

#include "synth.hpp"

#include <cainteoir/path.hpp>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

namespace rdf = cainteoir::rdf;
namespace rql = cainteoir::rdf::query;
namespace tts = cainteoir::tts;

static void
read_cainteoir_voices(const cainteoir::path &aPath, rdf::graph &aMetadata)
{
	DIR *dir = opendir(aPath.str().c_str());
	struct dirent *ent = nullptr;
	if (dir) while ((ent = readdir(dir)) != nullptr)
	{
		if (ent->d_name[0] == '.') continue;

		auto path = aPath / ent->d_name;

		struct stat st;
		if (lstat(path.str().c_str(), &st) == -1) continue;

		if (S_ISDIR(st.st_mode))
		{
			read_cainteoir_voices(path, aMetadata);
			continue;
		}

		char *ext = strstr(ent->d_name, ".vdb");
		if (ext == nullptr) continue;

		uint8_t data[512] = { 0 };
		FILE *f = fopen(path.str().c_str(), "rb");
		size_t n = f ? fread(data, 1, sizeof(data), f) : 0;
		fclose(f);

		if (n == 0) continue;

		cainteoir::native_endian_buffer header{ data, data + n };

		// magic
		if (header.u8() != 'V') continue;
		if (header.u8() != 'O') continue;
		if (header.u8() != 'I') continue;
		if (header.u8() != 'C') continue;
		if (header.u8() != 'E') continue;
		if (header.u8() != 'D') continue;
		if (header.u8() != 'B') continue;
		if (header.u16() != 0x3031) continue; // endianness

		const rdf::uri synth{ header.pstr(), {}}; // rdfns
		const rdf::uri voice{ synth.ns, header.pstr() }; // voice id
		const char *voice_name = header.pstr(); // name
		const char *synth_name = header.pstr(); // synthesizer
		const char *voice_author = header.pstr();
		const char *locale = header.pstr();
		uint8_t gender = header.u8();
		float volume_scale = header.f8_8();
		uint16_t frequency = header.u16();
		int channels = header.u8();
		const char *sample_format = header.pstr();

#ifdef HAVE_MBROLA
		if (strcmp(synth_name, "MBROLA") == 0)
		{
			// Special check for MBROLA voices to check if the MBROLA data file
			// is present...

			char database[256];
			snprintf(database, sizeof(database), MBROLA_DIR "/%s/%s", voice.ref.c_str(), voice.ref.c_str());

			if (access(database, R_OK) != 0)
				continue;
		}
#endif

		aMetadata.statement(synth, rdf::rdf("type"), rdf::tts("Synthesizer"));
		aMetadata.statement(synth, rdf::tts("name"), rdf::literal(synth_name));

		aMetadata.statement(voice, rdf::rdf("type"), rdf::tts("Voice"));
		aMetadata.statement(voice, rdf::tts("data"), rdf::literal(path.str()));
		aMetadata.statement(voice, rdf::tts("name"), rdf::literal(voice_name));
		aMetadata.statement(voice, rdf::dc("creator"), rdf::literal(voice_author));
		aMetadata.statement(voice, rdf::dc("language"), rdf::literal(locale));
		switch (gender)
		{
		case 'M':
			aMetadata.statement(voice, rdf::tts("gender"), rdf::tts("male"));
			break;
		case 'F':
			aMetadata.statement(voice, rdf::tts("gender"), rdf::tts("female"));
			break;
		}
		aMetadata.statement(voice, rdf::tts("volumeScale"), rdf::literal(volume_scale));
		aMetadata.statement(voice, rdf::tts("frequency"), rdf::literal(frequency, rdf::tts("hertz")));
		aMetadata.statement(voice, rdf::tts("channels"),  rdf::literal(channels, rdf::xsd("int")));
		aMetadata.statement(voice, rdf::tts("audioFormat"),  rdf::tts(sample_format));

		aMetadata.statement(voice, rdf::tts("voiceOf"),  synth);
		aMetadata.statement(synth, rdf::tts("hasVoice"), voice);
	}
	closedir(dir);
}

void
tts::read_voice_metadata(rdf::graph &aMetadata)
{
	auto voices = cainteoir::get_data_path() / "voices";
	read_cainteoir_voices(voices, aMetadata);
}

std::shared_ptr<tts::voice>
tts::create_voice(rdf::graph &aMetadata, const rdf::uri *aVoice)
{
	if (!aVoice) return {};

	const auto voice = rql::select(aMetadata, rql::subject == *aVoice);
	const std::string database = rql::select_value<std::string>(voice, rql::predicate == rdf::tts("data"));

	auto data = cainteoir::make_file_buffer(database.c_str());
	if (!data) return {};

	const char *header = data->begin();

	if (strncmp(header, "VOICEDB", 7) != 0 || *(const uint16_t *)(header + 7) != 0x3031)
		return {};

	const char *synth_name = (const char *)header + *(const uint16_t *)(header + 21); // synthesizer
#ifdef HAVE_MBROLA
	if (strcmp(synth_name, "MBROLA") == 0)
		return create_mbrola_voice(data, aMetadata, *aVoice);
#endif

	return {};
}
