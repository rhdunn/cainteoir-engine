/* Espeak Text-to-Speech Engine.
 *
 * Copyright (C) 2010-2013 Reece H. Dunn
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

#include "tts_engine.hpp"

namespace rdf = cainteoir::rdf;
namespace rql = cainteoir::rdf::query;
namespace tts = cainteoir::tts;

#if defined(HAVE_PICOAPI_H)

#include <picoapi.h>
#include <cainteoir/path.hpp>

#define PICO_MEM_SIZE        2500000
#define N_LINGWARE_RESOURCES 2
#define MAX_OUTBUF_SIZE      128

struct voice_data
{
	const char *language;
	const char *lingware[N_LINGWARE_RESOURCES];
};

/* supported voices */
static const voice_data voices[] = {
	{ "de-DE", { "de-DE_ta.bin", "de-DE_gl0_sg.bin" }},
	{ "en-US", { "en-US_ta.bin", "en-US_lh0_sg.bin" }},
	{ "en-GB", { "en-GB_ta.bin", "en-GB_kh0_sg.bin" }},
	{ "es-ES", { "es-ES_ta.bin", "es-ES_zl0_sg.bin" }},
	{ "fr-FR", { "fr-FR_ta.bin", "fr-FR_nk0_sg.bin" }},
	{ "it-IT", { "it-IT_ta.bin", "it-IT_cm0_sg.bin" }},
};

class pico_parameter : public tts::parameter
{
public:
	pico_parameter(const char *aName, const char *aUnits, int aMinimum, int aMaximum, int aValue)
		: mName(aName)
		, mUnits(aUnits)
		, mMinimum(aMinimum)
		, mMaximum(aMaximum)
		, mValue(aValue)
	{
	}

	const char *name() const
	{
		return mName;
	}

	const char *units() const
	{
		return mUnits;
	}

	int minimum() const
	{
		return mMinimum;
	}

	int maximum() const
	{
		return mMaximum;
	}

	int default_value() const
	{
		return mValue;
	}

	int value() const
	{
		return mValue;
	}

	bool set_value(int value)
	{
		return false;
	}
private:
	const char *mName;
	const char *mUnits;
	int mMinimum;
	int mMaximum;
	int mValue;
};

class pico_engine : public tts::engine
{
public:
	pico_engine(rdf::graph &metadata, std::string &baseuri, std::string &default_voice);
	~pico_engine();

	/** @name cainteoir::tts_engine */
	//@{

	bool select_voice(const char *voicename, const std::string &phonemeset);

	void speak(cainteoir::buffer *text, size_t offset, tts::synthesis_callback *callback);

	std::shared_ptr<tts::phoneme_reader> pronunciation();

	std::shared_ptr<tts::parameter>
	parameter(tts::parameter::type aType);

	//@}
private:
	void set_voice(const voice_data *data);
	void free_voice();

	bool speak_text(const char *text, pico_Int16 length, tts::synthesis_callback *callback);

	void check_return(int ret);

	std::shared_ptr<tts::parameter> mRate;
	std::shared_ptr<tts::parameter> mVolume;
	std::shared_ptr<tts::parameter> mPitch;
	std::shared_ptr<tts::parameter> mPitchRange;

	pico_System mSystem;
	void *mMemory;
	pico_Resource mResources[N_LINGWARE_RESOURCES];
	pico_Engine mEngine;
	const voice_data *mSelectedVoice;
};

pico_engine::pico_engine(rdf::graph &metadata, std::string &baseuri, std::string &default_voice)
	: mRate(std::make_shared<pico_parameter>(i18n("Speed"), "wpm", 80, 450, 180)) // based on recording
	, mVolume(std::make_shared<pico_parameter>(i18n("Volume"), "%", 0, 200, 100))
	, mPitch(std::make_shared<pico_parameter>(i18n("Base Pitch"), "", 0, 100, 50))
	, mPitchRange(std::make_shared<pico_parameter>(i18n("Pitch Variation"), "", 0, 100, 50))
	, mSystem(nullptr)
	, mEngine(nullptr)
{
	baseuri = "http://rhdunn.github.com/cainteoir/engines/pico";
	default_voice = "en-US";

	for (int i = 0; i < N_LINGWARE_RESOURCES; ++i)
		mResources[i] = nullptr;

	mMemory = malloc(PICO_MEM_SIZE);
	check_return(pico_initialize(mMemory, PICO_MEM_SIZE, &mSystem));

	rdf::uri pico = rdf::uri(baseuri, std::string());

	metadata.statement(pico, rdf::rdf("type"), rdf::tts("Engine"));
	metadata.statement(pico, rdf::tts("name"), rdf::literal("Pico"));

	for (const auto &data : voices)
	{
		rdf::uri voice = rdf::uri(baseuri, data.language);
		metadata.statement(voice, rdf::rdf("type"), rdf::tts("Voice"));
		metadata.statement(voice, rdf::dc("language"), rdf::literal(data.language));
		metadata.statement(voice, rdf::tts("name"), rdf::literal(data.language));
		metadata.statement(voice, rdf::tts("gender"), rdf::tts("female"));
		metadata.statement(voice, rdf::tts("phonemeset"), rdf::literal("ipa"));

		metadata.statement(voice, rdf::tts("frequency"), rdf::literal(16000, rdf::tts("hertz")));
		metadata.statement(voice, rdf::tts("channels"),  rdf::literal(1, rdf::xsd("int")));
		metadata.statement(voice, rdf::tts("audio-format"),  rdf::tts("s16le"));

		metadata.statement(voice, rdf::tts("voiceOf"), pico);
		metadata.statement(pico, rdf::tts("hasVoice"), voice);
	}
}

pico_engine::~pico_engine()
{
	free_voice();
	if (mSystem)
	{
		pico_terminate(&mSystem);
		free(mMemory);
	}
}

bool pico_engine::select_voice(const char *voicename, const std::string &phonemeset)
{
	for (const auto &data : voices)
	{
		if (!strcmp(data.language, voicename))
		{
			set_voice(&data);
			return true;
		}
	}
	return false;
}

void pico_engine::speak(cainteoir::buffer *text, size_t offset, tts::synthesis_callback *callback)
{
	if (speak_text(text->begin(), text->size(), callback))
	{
		// Pico will only process the current sentence if it finds the next sentence
		// break or a NULL character. Therefore, we force Pico to read the rest of the
		// current buffer:
		speak_text("\0", 1, callback);
	}
}

std::shared_ptr<tts::phoneme_reader> pico_engine::pronunciation()
{
	return std::shared_ptr<tts::phoneme_reader>();
}

std::shared_ptr<tts::parameter>
pico_engine::parameter(tts::parameter::type aType)
{
	switch (aType)
	{
	case tts::parameter::rate:        return mRate;
	case tts::parameter::volume:      return mVolume;
	case tts::parameter::pitch:       return mPitch;
	case tts::parameter::pitch_range: return mPitchRange;
	default:                          return std::shared_ptr<tts::parameter>();
	}
}

void pico_engine::set_voice(const voice_data *data)
{
	if (mSelectedVoice == data) return;
	free_voice();

	check_return(pico_createVoiceDefinition(mSystem, (const pico_Char *)data->language));
	for (int i = 0; i < N_LINGWARE_RESOURCES; ++i)
	{
		cainteoir::path filename = cainteoir::path(PICO_LANG_DIR) / data->lingware[i];
		check_return(pico_loadResource(mSystem, (const pico_Char *)(const char *)filename, &mResources[i]));

		char name[PICO_MAX_RESOURCE_NAME_SIZE];
		check_return(pico_getResourceName(mSystem, mResources[i], name));
		check_return(pico_addResourceToVoiceDefinition(mSystem, (const pico_Char *)data->language, (const pico_Char *)name));
	}
	check_return(pico_newEngine(mSystem, (const pico_Char *)data->language, &mEngine));

	mSelectedVoice = data;
}

void pico_engine::free_voice()
{
	if (mEngine)
	{
		pico_disposeEngine(mSystem, &mEngine);
		pico_releaseVoiceDefinition(mSystem, (pico_Char *)mSelectedVoice->language);
		mEngine = NULL;
	}
	for (int i = 0; i < N_LINGWARE_RESOURCES; ++i)
	{
		if (mResources[i])
		{
			pico_unloadResource(mSystem, &mResources[i]);
			mResources[i] = nullptr;
		}
	}
}

bool pico_engine::speak_text(const char *text, pico_Int16 length, tts::synthesis_callback *callback)
{
	short outbuf[MAX_OUTBUF_SIZE/2];
	pico_Int16 bytes_sent;
	pico_Int16 bytes_recv;
	pico_Int16 out_data_type;

	while (length)
	{
		check_return(pico_putTextUtf8(mEngine, (const pico_Char *)text, length, &bytes_sent));
    
		length -= bytes_sent;
		text   += bytes_sent;

		int status = PICO_STEP_BUSY;
		while (status == PICO_STEP_BUSY)
		{
			if (callback->state() == tts::stopped)
			{
				check_return(pico_resetEngine(mEngine, PICO_RESET_SOFT));
				return false;
			}

			status = pico_getData(mEngine, outbuf, MAX_OUTBUF_SIZE, &bytes_recv, &out_data_type);
			if (status != PICO_STEP_BUSY && status != PICO_STEP_IDLE)
				check_return(status);
			if (bytes_recv) callback->onaudiodata(outbuf, bytes_recv/2);
		}
	}
	return true;
}

void pico_engine::check_return(int ret)
{
	if (ret)
	{
		pico_Retstring message;
		pico_getSystemStatusMessage(mSystem, ret, message);
		throw std::runtime_error(message);
	}
}

tts::engine *tts::create_pico_engine(rdf::graph &aMetadata, std::string &uri, std::string &default_voice)
{
	return new pico_engine(aMetadata, uri, default_voice);
}

#else

tts::engine *tts::create_pico_engine(rdf::graph &aMetadata, std::string &uri, std::string &default_voice)
{
	return nullptr;
}

#endif
