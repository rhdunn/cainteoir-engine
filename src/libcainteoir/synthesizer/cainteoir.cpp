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

struct diphone_entry
{
	uint16_t start;
	uint16_t mid;
	uint16_t end;
};

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

	std::map<std::string, diphone_entry> mDiphones;
	uint16_t mFrames;
	uint8_t  mLpcOrder;
	uint32_t mCoefficientMin;
	uint32_t mCoefficientRange;
	const uint32_t *mLpcResidualOffsets;
	const uint8_t  *mLpcResidualSizes;
	const uint16_t *mLpcCoefficients;
	const uint8_t  *mLpcResidualData;
};

cainteoir_synthesizer::cainteoir_synthesizer(const std::shared_ptr<cainteoir::buffer> &aData,
                                             const std::shared_ptr<tts::phoneme_reader> &aPhonemeSet)
	: mData(aData)
	, mPhonemeSet(aPhonemeSet)
	, mSampleFormat(rdf::tts("s16le"))
	, mFrames(0)
	, mLpcOrder(0)
	, mCoefficientMin(0)
	, mCoefficientRange(0)
	, mLpcResidualOffsets(nullptr)
	, mLpcResidualSizes(nullptr)
	, mLpcCoefficients(nullptr)
	, mLpcResidualData(nullptr)
{
	const uint8_t *data = (const uint8_t *)mData->begin() + 13; // skip to sample-rate
	mSampleRate = *(const uint16_t *)data;
	data += 2;

	while (*data != 0) ++data; // name
	++data;

	while (*data != 0) ++data; // company
	++data;

	while (*data != 0) ++data; // locale
	++data;

	while (*data != 0) ++data; // phonemeset
	++data;

	data += 2; // gender & age

	const uint8_t *end = (const uint8_t *)mData->end();
	uint16_t entries = 0;
	while (data < end)
	{
		if (data[0] == 'I' && data[1] == 'D' && data[2] == 'X') switch (data[3])
		{
		case 0: // Diphone Index
			data += 4;
			entries = *(const uint16_t *)data;
			data += 2;
			for (; entries > 0; --entries)
			{
				const char *name = (const char *)data;
				while (*data != 0) ++data;
				++data;

				uint16_t start = *(const uint16_t *)data;
				uint16_t mid   = start + data[2];
				uint16_t end   = mid   + data[3];
				data += 4;

				mDiphones[name] = { start, mid, end };
			}
			break;
		case 1: // LPC Residual Index
			data += 4;
			entries = *(const uint16_t *)data;
			data += 2;
			mLpcResidualOffsets = (const uint32_t *)data;
			data += entries * sizeof(uint32_t);
			break;
		case 2: // LPC Residual Sizes
			data += 4;
			entries = *(const uint16_t *)data;
			data += 2;
			mLpcResidualSizes = (const uint8_t *)data;
			data += entries;
			break;
		default: // Unknown
			data += 4;
			break;
		}
		else if (data[0] == 'D' && data[1] == 'A' && data[2] == 'T') switch (data[3])
		{
		case 0: // Residual Excited LPC (ulaw)
		case 1: // Residual Excited LPC (G.721)
			data += 4;
			mFrames = *(const uint16_t *)data;
			mLpcOrder = *(const uint8_t *)(data + 2);
			mCoefficientMin = *(const uint32_t *)(data + 3);
			mCoefficientRange = *(const uint32_t *)(data + 7);
			data += 11;
			break;
		default: // Unknown
			data += 4;
			break;
		}
		else if (data[0] == 'L' && data[1] == 'P' && data[2] == 'C')
		{
			data += 3;
			mLpcCoefficients = (const uint16_t *)data;
			data += (mFrames * mLpcOrder * sizeof(uint16_t));
		}
		else if (data[0] == 'R' && data[1] == 'E' && data[2] == 'S')
		{
			data += 3;
			mLpcResidualData = data;
			data = end;
		}
		else
			++data;
	}
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

	if (strncmp(header, "VOICEDB", 7) != 0 ||
	    *(const uint16_t *)(header + 7) != 0x3031)
		return {}; // magic signature mismatched

	auto phonemes = tts::createPhonemeReader(phonemeset.c_str());
	if (!phonemes) return {};

	return std::make_shared<cainteoir_synthesizer>(data, phonemes);
}
