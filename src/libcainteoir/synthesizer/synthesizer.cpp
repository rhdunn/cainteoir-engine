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

		const uint8_t *header = data;

		// magic
		if (*header++ != 'V') continue;
		if (*header++ != 'O') continue;
		if (*header++ != 'I') continue;
		if (*header++ != 'C') continue;
		if (*header++ != 'E') continue;
		if (*header++ != 'D') continue;
		if (*header++ != 'B') continue;

		// endianness
		if (*(const uint16_t *)header != 0x3031) continue;
		header += 2;
	}
	closedir(dir);
}

void
tts::read_voice_metadata(rdf::graph &aMetadata)
{
	auto voices = cainteoir::get_data_path() / "voices";

	read_mbrola_voices(aMetadata);
	read_cainteoir_voices(voices, aMetadata);
}

std::shared_ptr<tts::synthesizer>
tts::create_voice_synthesizer(rdf::graph &aMetadata, const rdf::uri *aVoice)
{
	if (!aVoice) return {};

	const auto voice = rql::select(aMetadata, rql::subject == *aVoice);
	const std::string database = rql::select_value<std::string>(voice, rql::predicate == rdf::tts("data"));

	auto data = cainteoir::make_file_buffer(database.c_str());
	if (!data) return {};

	const char *header = data->begin();

	if (strncmp(header, "VOICEDB", 7) == 0 && *(const uint16_t *)(header + 7) == 0x3031)
		; // Cainteoir Text-to-Speech Voice Database
	else if (aVoice->ns == "http://reecedunn.co.uk/tts/synthesizer/mbrola#")
		return create_mbrola_synthesizer(aMetadata, *aVoice);

	return {};
}
