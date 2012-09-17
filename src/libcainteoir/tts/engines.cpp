/* Espeak Text-to-Speech Engine.
 *
 * Copyright (C) 2010-2012 Reece H. Dunn
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

#include <cainteoir/engines.hpp>
#include <cainteoir/stopwatch.hpp>
#include "tts_engine.hpp"
#include <stdexcept>

static const int CHARACTERS_PER_WORD = 6;

namespace rdf = cainteoir::rdf;
namespace rql = cainteoir::rdf::query;
namespace tts = cainteoir::tts;

static inline double percentageof(size_t a, size_t b)
{
	return (double(a) / b) * 100.0;
}

struct speech_impl : public tts::speech , public tts::engine_callback
{
	tts::engine *engine;
	std::shared_ptr<cainteoir::audio> audio;
	std::shared_ptr<cainteoir::document> doc;

	cainteoir::document::const_iterator mFrom;
	cainteoir::document::const_iterator mTo;

	tts::state speechState;
	pthread_t threadId;
	std::string mErrorMessage;

	cainteoir::stopwatch mTimer; /**< @brief The time taken to read the document. */
	double mElapsedTime; /**< @brief The amount of time elapsed since |mStartTime|. */
	double mTotalTime; /**< @brief The (estimated) total amount of time to read the document. */

	double mCompleted; /**< @brief The percentage of the document read from the starting position. */
	double mProgress;  /**< @brief The percentage of the document read from the beginning. */

	size_t currentOffset; /**< @brief The current offset from the beginning to the current block being read. */
	size_t speakingPos;   /**< @brief The position within the block where the speaking is upto. */
	size_t speakingLen;   /**< @brief The length of the word/fragment being spoken. */
	size_t textLen;       /**< @brief The length of the text range being read. */
	int wordsPerMinute;   /**< @brief The speech rate of the current voice. */

	speech_impl(tts::engine *aEngine,
	            std::shared_ptr<cainteoir::audio> aAudio,
	            const std::shared_ptr<cainteoir::document> &aDoc,
	            cainteoir::document::const_iterator aFrom,
	            cainteoir::document::const_iterator aTo,
	            std::shared_ptr<tts::parameter> aRate);
	~speech_impl();

	cainteoir::document::const_iterator begin() const { return mFrom; }
	cainteoir::document::const_iterator end()   const { return mTo; }

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

	std::string error_message() const;

	//@}
	/** @name tts::callback */
	//@{

	tts::state state() const;

	void onaudiodata(short *data, int nsamples);

	void onspeaking(size_t pos, size_t len);

	//@}
};

static void * speak_tts_thread(void *data)
{
	speech_impl *speak = (speech_impl *)data;

	try
	{
		speak->audio->open();
		speak->started();

		size_t n = 0;
		size_t offset = 0;
		for (auto &node : *speak)
		{
			size_t len = node->size();

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

				speak->engine->speak(node.get(), offset, speak);
				offset = 0;

				if (speak->state() == tts::stopped)
					break;

				n += len;
				speak->progress(n);
			}
		}
	}
	catch (const std::exception &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
		speak->mErrorMessage = e.what();
	}

	speak->audio->close();
	speak->finished();
	return nullptr;
}

speech_impl::speech_impl(tts::engine *aEngine,
                         std::shared_ptr<cainteoir::audio> aAudio,
                         const std::shared_ptr<cainteoir::document> &aDoc,
                         cainteoir::document::const_iterator aFrom,
                         cainteoir::document::const_iterator aTo,
                         std::shared_ptr<tts::parameter> aRate)
	: engine(aEngine)
	, audio(aAudio)
	, doc(aDoc)
	, speechState(cainteoir::tts::speaking)
	, speakingPos(0)
	, speakingLen(0)
	, textLen(0)
	, wordsPerMinute(aRate ? aRate->value() : 170)
	, mFrom(aFrom)
	, mTo(aTo)
{
	for (auto node = aFrom; node != aTo; ++node)
		textLen += (*node)->size();

	started();
	int ret = pthread_create(&threadId, nullptr, speak_tts_thread, (void *)this);
}

speech_impl::~speech_impl()
{
}

void speech_impl::started()
{
	mElapsedTime = 0.0;
	mTotalTime = (double(textLen) / CHARACTERS_PER_WORD / wordsPerMinute * 60.0);
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
	pthread_join(threadId, nullptr);
}

void speech_impl::wait()
{
	pthread_join(threadId, nullptr);
	finished();
}

double speech_impl::elapsedTime() const
{
	if (is_speaking())
		const_cast<speech_impl *>(this)->mElapsedTime = mTimer.elapsed();

	return mElapsedTime;
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

std::string speech_impl::error_message() const
{
	return mErrorMessage;
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
	mProgress = percentageof(actualPos, textLen);

	if (mElapsedTime > 0.1)
	{
		mCompleted = percentageof(actualPos, textLen);
		if (mCompleted >= 0.1)
			mTotalTime = (mElapsedTime / mCompleted) * 100.0;
	}
}

/** @struct cainteoir::tts::engines
  * @brief  Manages the available text-to-speech engines.
  */

/** @brief Create a new text-to-speech engine manager object.
  *
  * @param[out] metadata The RDF graph to add engine and voice metadata to.
  */
tts::engines::engines(rdf::graph &metadata)
	: selectedVoice(nullptr)
{
	std::string uri;
	active = tts::create_espeak_engine(metadata, uri);
	if (active)
	{
		enginelist[uri] = active;
		if (!select_voice(metadata, rdf::uri(uri, "default")))
			throw std::runtime_error(i18n("default voice not found."));
	}

	if (!active)
		throw std::runtime_error(i18n("no text-to-speech voices found."));
}

/** @brief Clean up the engines.
  */
tts::engines::~engines()
{
	for (auto &engine : enginelist)
		delete engine.second;
}

/** @brief Change the voice used to read the documents.
  *
  * @param[in] aMetadata The RDF graph containing the engine and voice metadata.
  * @param[in] aVoice    The voice to use to read the documents.
  *
  * @retval true  If the voice was changed.
  * @retval false If the voice was not changed.
  */
bool tts::engines::select_voice(const rdf::graph &aMetadata, const rdf::uri &aVoice)
{
	engine *engine = nullptr;
	std::string voice;
	const rdf::uri * voiceUri = nullptr;

	for (auto &statement : rql::select(aMetadata, rql::subject == aVoice))
	{
		if (rql::predicate(statement) == rdf::tts("name"))
		{
			voice = rql::value(statement);
			voiceUri = &rql::subject(statement);
		}
		else if (rql::predicate(statement) == rdf::tts("voiceOf"))
		{
			const rdf::uri &uri = rql::object(statement);
			if (!uri.empty())
				engine = enginelist[ uri.str() ];
		}
	}

	if (engine && !voice.empty() && engine->select_voice(voice.c_str()))
	{
		active = engine;
		selectedVoice = voiceUri;
		return true;
	}

	return false;
}

/** @fn    const cainteoir::rdf::uri &cainteoir::tts::engines::voice() const
  * @brief Get the currently active voice.
  *
  * @return The currently active voice.
  */

/** @brief Speak the specified document.
  *
  * @param[in] doc    The document to speak.
  * @param[in] out    The audio output device (for reading) or file (for recording).
  * @param[in] offset The number of characters in the document to skip before reading.
  *
  * @return The object associated with this speech session.
  */
std::shared_ptr<tts::speech>
tts::engines::speak(const std::shared_ptr<cainteoir::document> &doc,
                    std::shared_ptr<audio> out,
                    size_t offset)
{
	auto from = doc->children().begin();
	auto end  = doc->children().end();

	size_t n = 0;
	while (from != end && n < offset)
	{
		n += (*from)->size();
		++from;
	}

	return speak(doc, out, from, end);
}

/** @brief Speak the specified document.
  *
  * @param[in] doc  The document to speak.
  * @param[in] out  The audio output device (for reading) or file (for recording).
  * @param[in] from The place in the document to start reading from.
  * @param[in] to   The place in the document to read upto.
  *
  * @return The object associated with this speech session.
  */
std::shared_ptr<tts::speech>
tts::engines::speak(const std::shared_ptr<cainteoir::document> &doc,
                    std::shared_ptr<audio> out,
                    cainteoir::document::const_iterator from,
                    cainteoir::document::const_iterator to)
{
	return std::make_shared<speech_impl>(active, out, doc, from, to, parameter(tts::parameter::rate));
}

/** @brief Get the specified parameter associated with the active engine.
  *
  * @param[in] aType The parameter to get.
  *
  * @return The specified parameter.
  */
std::shared_ptr<tts::parameter>
tts::engines::parameter(tts::parameter::type aType)
{
	if (!active)
		return std::shared_ptr<tts::parameter>();
	return active->parameter(aType);
}

/** @struct cainteoir::tts::speech
  * @brief  Manage a speech synthesis session.
  */

/** @fn    cainteoir::tts::speech::~speech()
  * @brief Clean up the speech object.
  */

/** @fn    bool cainteoir::tts::speech::is_speaking() const
  * @brief Is the session currently speaking?
  *
  * @retval true  If the session is currently speaking.
  * @retval false If the session is not currently speaking.
  */

/** @fn    void cainteoir::tts::speech::stop()
  * @brief Immediately stop the session from speaking.
  */

/** @fn    void cainteoir::tts::speech::wait()
  * @brief Wait until the session has finished speaking.
  */

/** @fn    double cainteoir::tts::speech::elapsedTime() const
  * @brief Get the time elapsed since starting this session.
  *
  * @return The time elapsed since starting this session.
  */

/** @fn    double cainteoir::tts::speech::totalTime() const
  * @brief Get the (estimated) total time for this session.
  *
  * @return The (estimated) total time for this session.
  */

/** @fn    double cainteoir::tts::speech::completed() const
  * @brief Get the percentage of this session completed.
  *
  * @return The percentage of this session completed.
  */

/** @fn    size_t cainteoir::tts::speech::position() const
  * @brief Get the current speaking position in the document for this session.
  *
  * @return The current speaking position in the document for this session.
  */

/** @fn    std::string cainteoir::tts::speech::error_message() const
  * @brief Get the error message associated with this session.
  *
  * @return The error message associated with this session.
  *
  * If there is no error message associated with this session, the return value
  * is empty.
  */

/** @enum  cainteoir::tts::parameter::type
  * @brief Identifies the parameter.
  */

/** @var   cainteoir::tts::parameter::type cainteoir::tts::parameter::rate
  * @brief How fast the voice speaks.
  */

/** @var   cainteoir::tts::parameter::type cainteoir::tts::parameter::volume
  * @brief The volume of the voice audio.
  */

/** @var   cainteoir::tts::parameter::type cainteoir::tts::parameter::pitch
  * @brief The base pitch to render the voice at.
  */

/** @var   cainteoir::tts::parameter::type cainteoir::tts::parameter::pitch_range
  * @brief How varied the pitch is due to prosody variations.
  */

/** @var   cainteoir::tts::parameter::type cainteoir::tts::parameter::word_gap
  * @brief How long to pause between each word.
  */

/** @var   cainteoir::tts::parameter::type cainteoir::tts::parameter::number_of_parameters
  * @brief The number of parameters in the parameter type enumeration.
  */

/** @class cainteoir::tts::parameter
  * @brief A configurable option of a text-to-speech engine or voice.
  */

/** @fn    cainteoir::tts::parameter::~parameter()
  * @brief Clean up the parameter object.
  */

/** @fn    const char *cainteoir::tts::parameter::name() const
  * @brief Get the name of the parameter.
  *
  * @return The name of the parameter.
  */

/** @fn    const char *cainteoir::tts::parameter::units() const
  * @brief Get the units the parameter is stored in.
  *
  * @return The units the parameter is stored in.
  */

/** @fn    int cainteoir::tts::parameter::minimum() const
  * @brief Get the minimum value for the parameter.
  *
  * @return The minimum value for the parameter.
  */

/** @fn    int cainteoir::tts::parameter::maximum() const
  * @brief Get the maximum value for the parameter.
  *
  * @return The maximum value for the parameter.
  */

/** @fn    int cainteoir::tts::parameter::default_value() const
  * @brief Get the default value for the parameter.
  *
  * @return The default value for the parameter.
  */

/** @fn    int cainteoir::tts::parameter::value() const
  * @brief Get the current value for the parameter.
  *
  * @return The current value for the parameter.
  */

/** @fn    bool cainteoir::tts::parameter::set_value(int value)
  * @brief Set the current value for the parameter.
  *
  * @param[in] value The new value of the parameter.
  *
  * @retval true  If the parameter value was changed.
  * @retval false If the parameter value was not changed.
  */
