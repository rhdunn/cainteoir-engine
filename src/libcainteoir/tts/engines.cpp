/* Espeak Text-to-Speech Engine.
 *
 * Copyright (C) 2010 Reece H. Dunn
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

#include <cainteoir/engines.hpp>
#include "tts_engine.hpp"
#include <stdexcept>

namespace rdf = cainteoir::rdf;
namespace rql = cainteoir::rdf::query;
namespace tts = cainteoir::tts;

struct speech_impl : public tts::speech , public tts::engine_callback
{
	tts::engine *engine;
	cainteoir::audio *audio;
	std::tr1::shared_ptr<cainteoir::document> doc;

	tts::state speechState;
	pthread_t threadId;

	time_t startTime;
	double elapsedTime;

	size_t currentOffset;
	size_t speakingPos;
	size_t speakingLen;

	speech_impl(tts::engine *aEngine, cainteoir::audio *aAudio, const std::tr1::shared_ptr<cainteoir::document> &aDoc);
	~speech_impl();

	void started();
	void progress(size_t n);
	void finished();

	/** @name tts::speech */
	//@{

	bool is_speaking() const;

	void stop();
	void wait();

	double elapsed() const;

	size_t position() const;

	//@}
	/** @name tts::callback */
	//@{

	tts::state state() const;

	void onaudiodata(short *data, int nsamples);

	void onspeaking(size_t pos, size_t len);

	//@}
};

void * speak_tts_thread(void *data)
{
	speech_impl *speak = (speech_impl *)data;

	speak->audio->open();
	speak->started();

	size_t n = 0;
	foreach_iter(node, speak->doc->children())
	{
		if (speak->state() == tts::stopped)
			break;

		speak->engine->speak(node->get(), speak);

		n += (*node)->size();
		speak->progress(n);
	}

	speak->audio->close();
	speak->finished();
	return NULL;
}

speech_impl::speech_impl(tts::engine *aEngine, cainteoir::audio *aAudio, const std::tr1::shared_ptr<cainteoir::document> &aDoc)
	: engine(aEngine)
	, audio(aAudio)
	, doc(aDoc)
	, speechState(cainteoir::tts::speaking)
	, speakingPos(0)
	, speakingLen(0)
{
	started();
	int ret = pthread_create(&threadId, NULL, speak_tts_thread, (void *)this);
}

speech_impl::~speech_impl()
{
}

void speech_impl::started()
{
	startTime = time(NULL);
	elapsedTime = 0.0;
	currentOffset = 0;
}

void speech_impl::progress(size_t n)
{
	currentOffset = n;
}

void speech_impl::finished()
{
	speechState = cainteoir::tts::stopped;
}

bool speech_impl::is_speaking() const
{
	return state() == cainteoir::tts::speaking;
}

void speech_impl::stop()
{
	finished();
	pthread_join(threadId, NULL);
}

void speech_impl::wait()
{
	pthread_join(threadId, NULL);
	finished();
}

double speech_impl::elapsed() const
{
	if (is_speaking())
		const_cast<speech_impl *>(this)->elapsedTime = difftime(time(NULL), startTime);
	return elapsedTime;
}

size_t speech_impl::position() const
{
	return currentOffset + speakingPos;
}

tts::state speech_impl::state() const
{
	return speechState;
}

void speech_impl::onaudiodata(short *data, int nsamples)
{
	audio->write((const char *)data, nsamples*2);
}

void speech_impl::onspeaking(size_t pos, size_t len)
{
	speakingPos = pos;
	speakingLen = len;
}

///////////////////////////////////////////////////////////////////////////////

tts::engines::engines(rdf::graph &metadata)
{
	std::string uri;
	active = tts::create_espeak_engine(metadata, uri);
	enginelist[uri] = active;

	if (!active)
		throw std::runtime_error("no text-to-speech voices found.");
}

tts::engines::~engines()
{
	foreach_iter(engine, enginelist)
		delete engine->second;
}

int tts::engines::get_channels() const
{
	return active->get_channels();
}

int tts::engines::get_frequency() const
{
	return active->get_frequency();
}

cainteoir::audio_format tts::engines::get_audioformat() const
{
	return active->get_audioformat();
}

bool tts::engines::select_voice(const rdf::graph &aMetadata, const rdf::uri &aVoice)
{
	engine *engine = NULL;
	std::string voice;

	foreach_iter(statement, rql::select(aMetadata, rql::subject, aVoice))
	{
		if (rql::predicate(*statement) == rdf::tts("name"))
			voice = rql::value(rql::object(*statement));
		else if (rql::predicate(*statement) == rdf::tts("voiceOf"))
		{
			const rdf::uri *uri = rql::object(*statement);
			if (uri)
				engine = enginelist[ uri->str() ];
		}
	}

	if (engine && !voice.empty() && engine->select_voice(voice.c_str()))
	{
		active = engine;
		return true;
	}

	return false;
}

std::auto_ptr<tts::speech> tts::engines::speak(const std::tr1::shared_ptr<cainteoir::document> &doc, audio *out)
{
	return std::auto_ptr<tts::speech>(new speech_impl(active, out, doc));
}
