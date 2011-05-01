/* Espeak Text-to-Speech Engine.
 *
 * Copyright (C) 2010-2011 Reece H. Dunn
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
#include <cainteoir/document.hpp>
#include <cainteoir/audio.hpp>
#include <cainteoir/platform.hpp>
#include "tts_engine.hpp"
#include <stdexcept>

#define USE_GETTIMEOFDAY 1

#if USE_GETTIMEOFDAY
	#include <sys/time.h>

	double timeofday()
	{
		timeval tv;
		gettimeofday(&tv, NULL);

		return tv.tv_sec + (double(tv.tv_usec) / 1000000.0);
	}

	class stopwatch
	{
	public:
		stopwatch() { mStart = timeofday(); }

		double elapsed() const { return timeofday() - mStart; }
	private:
		double mStart;
	};
#else
	#include <ctime>

	class stopwatch
	{
	public:
		stopwatch() { mStart = time(NULL); }

		double elapsed() const { return difftime(time(NULL), mStart); }
	private:
		time_t mStart;
	};
#endif

namespace rdf = cainteoir::rdf;
namespace rql = cainteoir::rdf::query;
namespace tts = cainteoir::tts;

inline double percentageof(size_t a, size_t b)
{
	return (double(a) / b) * 100.0;
}

struct speech_impl : public tts::speech , public tts::engine_callback
{
	tts::engine *engine;
	cainteoir::audio *audio;
	std::tr1::shared_ptr<cainteoir::document> doc;

	tts::state speechState;
	pthread_t threadId;

	stopwatch mTimer;    /**< @brief The time taken to read the document. */
	double mElapsedTime; /**< @brief The amount of time elapsed since |mStartTime|. */
	double mTotalTime;   /**< @brief The (estimated) total amount of time to read the document. */

	double mCompleted; /**< @brief The percentage of the document read from the starting position. */
	double mProgress;  /**< @brief The percentage of the document read from the beginning. */

	size_t currentOffset; /**< @brief The current offset from the beginning to the current block being read. */
	size_t speakingPos;   /**< @brief The position within the block where the speaking is upto. */
	size_t speakingLen;   /**< @brief The length of the word/fragment being spoken. */
	size_t mOffset;       /**< @brief The offset to the position where speaking is to start. */

	speech_impl(tts::engine *aEngine, cainteoir::audio *aAudio, const std::tr1::shared_ptr<cainteoir::document> &aDoc, size_t aOffset);
	~speech_impl();

	void started();
	void progress(size_t n);
	void finished();

	/** @name tts::speech */
	//@{

	bool is_speaking() const;

	void stop();
	void wait();

	double elapsedTime() const;
	double totalTime() const;

	double completed() const;

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
	speak->progress(speak->mOffset);

	size_t n = 0;
	size_t offset = speak->mOffset;
	foreach_iter(node, speak->doc->children())
	{
		size_t len = (*node)-> size();

		if (len <= offset)
		{
			n += len;
			offset -= len;
		}
		else
		{
			if (offset > 0)
			{
				n += offset;
				len -= offset;
				speak->progress(n);
			}

			speak->engine->speak(node->get(), offset, speak);
			offset = 0;

			if (speak->state() == tts::stopped)
				break;

			n += len;
			speak->progress(n);
		}
	}

	speak->audio->close();
	speak->finished();
	return NULL;
}

speech_impl::speech_impl(tts::engine *aEngine, cainteoir::audio *aAudio, const std::tr1::shared_ptr<cainteoir::document> &aDoc, size_t aOffset)
	: engine(aEngine)
	, audio(aAudio)
	, doc(aDoc)
	, speechState(cainteoir::tts::speaking)
	, speakingPos(0)
	, speakingLen(0)
	, mOffset(aOffset)
{
	started();
	int ret = pthread_create(&threadId, NULL, speak_tts_thread, (void *)this);
}

speech_impl::~speech_impl()
{
}

void speech_impl::started()
{
	mElapsedTime = 0.0;
	mTotalTime = 0.0;
	mCompleted = 0.0;
	mProgress = 0.0;
	currentOffset = 0;
}

void speech_impl::progress(size_t n)
{
	currentOffset = n;
	onspeaking(0, 0);
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

double speech_impl::elapsedTime() const
{
	if (is_speaking())
		const_cast<speech_impl *>(this)->mElapsedTime = mTimer.elapsed();

	if (mOffset == 0)
		return mElapsedTime;
	return (mTotalTime * mProgress) / 100.0;
}

double speech_impl::totalTime() const
{
	return mTotalTime;
}

double speech_impl::completed() const
{
	return mProgress;
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

	size_t actualPos = currentOffset + speakingPos;

	mElapsedTime = mTimer.elapsed();
	mProgress = percentageof(actualPos, doc->text_length());

	if (mElapsedTime > 0.1 && actualPos >= mOffset)
	{
		mCompleted = percentageof(actualPos - mOffset, doc->text_length());
		mTotalTime = (mElapsedTime / mCompleted) * 100.0;
	}
}

///////////////////////////////////////////////////////////////////////////////

tts::engines::engines(rdf::graph &metadata) : selectedVoice(NULL)
{
	std::string uri;
	active = tts::create_espeak_engine(metadata, uri);
	if (active)
	{
		enginelist[uri] = active;
		if (!select_voice(metadata, rdf::uri(uri, "default")))
			throw std::runtime_error(_("default voice not found."));
	}

	if (!active)
		throw std::runtime_error(_("no text-to-speech voices found."));

	cainteoir::supportedDocumentFormats(metadata);
	cainteoir::supportedAudioFormats(metadata);
}

tts::engines::~engines()
{
	foreach_iter(engine, enginelist)
		delete engine->second;
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
		selectedVoice = &aVoice;
		return true;
	}

	return false;
}

std::shared_ptr<tts::speech> tts::engines::speak(const std::tr1::shared_ptr<cainteoir::document> &doc, audio *out, size_t offset)
{
	return std::shared_ptr<tts::speech>(new speech_impl(active, out, doc, offset));
}
