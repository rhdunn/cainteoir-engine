/* Cainteoir Text-to-Speech Voice Database.
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
#include "i18n.h"
#include "compatibility.hpp"

#include "synthesizer.hpp"
#include <cainteoir/engines.hpp>
#include <cainteoir/path.hpp>

#include <sys/types.h>
#include <dirent.h>

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;
namespace css = cainteoir::css;

void
tts::read_cainteoir_voices(rdf::graph &aMetadata)
{
	std::string baseuri = "http://reecedunn.co.uk/tts/synthesizer/cainteoir#";

	rdf::uri synth = rdf::uri(baseuri, std::string());
	aMetadata.statement(synth, rdf::rdf("type"), rdf::tts("Synthesizer"));
	aMetadata.statement(synth, rdf::tts("name"), rdf::literal("Cainteoir"));

	auto voices = cainteoir::get_data_path() / "voices";

	DIR *dir = opendir(voices.str().c_str());
	struct dirent *ent = nullptr;
	if (dir) while ((ent = readdir(dir)) != nullptr)
	{
		if (ent->d_name[0] == '.') continue;

		char *ext = strstr(ent->d_name, ".vdb");
		if (ext == nullptr) continue;

		auto vdb = voices / ent->d_name;
		*ext = 0;

		uint8_t data[512] = { 0 };
		FILE *f = fopen(vdb.str().c_str(), "rb");
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

		// header version
		if (*header++ != 0) continue;

		rdf::uri voice = rdf::uri(baseuri, ent->d_name);
		aMetadata.statement(voice, rdf::rdf("type"), rdf::tts("Voice"));
		aMetadata.statement(voice, rdf::tts("data"), rdf::literal(vdb.str()));

		header += 3; // voice version

		// sample rate
		aMetadata.statement(voice, rdf::tts("frequency"), rdf::literal(*(const uint16_t *)header, rdf::tts("hertz")));
		aMetadata.statement(voice, rdf::tts("channels"),  rdf::literal(1, rdf::xsd("int")));
		aMetadata.statement(voice, rdf::tts("audioFormat"),  rdf::tts("s16le"));
		header += 2;

		// name
		aMetadata.statement(voice, rdf::tts("name"), rdf::literal((const char *)header));
		while (*header != 0) ++header;
		++header;

		// company
		while (*header != 0) ++header;
		++header;

		// locale
		aMetadata.statement(voice, rdf::dc("language"), rdf::literal((const char *)header));
		while (*header != 0) ++header;
		++header;

		// phonemeset
		aMetadata.statement(voice, rdf::tts("phonemeset"), rdf::literal((const char *)header));
		while (*header != 0) ++header;
		++header;

		// gender
		aMetadata.statement(voice, rdf::tts("gender"), rdf::tts(*header++ == 'F' ? "female" : "male"));

		aMetadata.statement(voice, rdf::tts("voiceOf"), synth);
		aMetadata.statement(synth, rdf::tts("hasVoice"), voice);
	}
	closedir(dir);
}

std::shared_ptr<tts::synthesizer>
tts::create_cainteoir_synthesizer(rdf::graph &aMetadata, const rdf::uri &aVoice)
{
	return {};
}
