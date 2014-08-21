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

#include "synth.hpp"
#include <cainteoir/path.hpp>

#include <sys/types.h>
#include <dirent.h>

namespace tts = cainteoir::tts;
namespace rdf = cainteoir::rdf;
namespace rql = cainteoir::rdf::query;

void
tts::read_cainteoir_voices(rdf::graph &aMetadata)
{
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
	}
	closedir(dir);
}

struct cainteoir_synthesizer : public tts::synthesizer
{
	cainteoir_synthesizer(const std::shared_ptr<cainteoir::buffer> &aData,
	                      const std::shared_ptr<tts::phoneme_reader> &aPhonemeSet);

	/** @name audio_info */
	//@{

	int channels() const { return 1; }

	int frequency() const { return mSampleRate; }

	const rdf::uri &format() const { return mSampleFormat; }

	//@}
	/** @name tts::synthesizer */
	//@{

	void bind(const std::shared_ptr<tts::prosody_reader> &aProsody);

	bool synthesize(cainteoir::audio *out);

	//@}
private:
	std::shared_ptr<cainteoir::buffer> mData;
	std::shared_ptr<tts::phoneme_reader> mPhonemeSet;
	int mSampleRate;
	rdf::uri mSampleFormat;
};

cainteoir_synthesizer::cainteoir_synthesizer(const std::shared_ptr<cainteoir::buffer> &aData,
                                             const std::shared_ptr<tts::phoneme_reader> &aPhonemeSet)
	: mData(aData)
	, mPhonemeSet(aPhonemeSet)
	, mSampleFormat(rdf::tts("s16le"))
	, mSampleRate(0)
{
}

void cainteoir_synthesizer::bind(const std::shared_ptr<tts::prosody_reader> &aProsody)
{
}

bool cainteoir_synthesizer::synthesize(cainteoir::audio *out)
{
	return false;
}

std::shared_ptr<tts::synthesizer>
tts::create_cainteoir_synthesizer(rdf::graph &aMetadata, const rdf::uri &aVoice)
{
	const auto voice = rql::select(aMetadata, rql::subject == aVoice);
	const std::string database = rql::select_value<std::string>(voice, rql::predicate == rdf::tts("data"));
	const std::string phonemeset = rql::select_value<std::string>(voice, rql::predicate == rdf::tts("phonemeset"));

	auto data = cainteoir::make_file_buffer(database.c_str());
	if (!data) return {};

	const char *header = data->begin();

	if (strncmp(header, "VOICEDB", 7) != 0 || *(const uint16_t *)(header + 7) != 0x3031)
		return {}; // magic signature mismatched

	auto phonemes = tts::createPhonemeReader(phonemeset.c_str());
	if (!phonemes) return {};

	return std::make_shared<cainteoir_synthesizer>(data, phonemes);
}
