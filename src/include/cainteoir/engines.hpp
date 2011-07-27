/* Text-to-Speech Engine API.
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

#ifndef CAINTEOIR_ENGINE_ENGINES_HPP
#define CAINTEOIR_ENGINE_ENGINES_HPP

#include "buffer.hpp"
#include "audio.hpp"
#include "metadata.hpp"
#include "document.hpp"
#include <map>

namespace cainteoir { namespace tts
{
	struct engine;

	struct speech
	{
		virtual ~speech() {}

		virtual bool is_speaking() const = 0;

		virtual void stop() = 0;
		virtual void wait() = 0;

		virtual double elapsedTime() const = 0;
		virtual double totalTime() const = 0;

		virtual double completed() const = 0;

		virtual size_t position() const = 0;
	};

	struct parameter
	{
		virtual ~parameter() {}

		virtual const char *name() const = 0;

		virtual const char *units() const = 0;

		virtual int minimum() const = 0;

		virtual int maximum() const = 0;

		virtual int default_value() const = 0;

		virtual int value() const = 0;

		virtual bool set_value(int value) = 0;

		enum type
		{
			rate, /** @brief How fast the voice speaks. */
			volume, /** @brief The volume of the voice audio. */
			pitch, /** @brief The base pitch to render the voice at. */
			pitch_range, /** @brief How varied the pitch is due to prosody variations. */
			word_gap, /** @brief How long to pause between each word. */
			number_of_parameters
		};
	};

	class engines
	{
	public:
		engines(rdf::graph &metadata);
		~engines();

		bool select_voice(const rdf::graph &aMetadata, const rdf::uri &aVoice);

		const rdf::uri & voice() const { return *selectedVoice; }

		std::tr1::shared_ptr<speech>
		speak(const std::tr1::shared_ptr<document> &doc,
		      std::tr1::shared_ptr<audio> out,
		      size_t offset = 0);

		std::tr1::shared_ptr<speech>
		speak(const std::tr1::shared_ptr<document> &doc,
		      std::tr1::shared_ptr<audio> out,
		      cainteoir::document::const_iterator from,
		      cainteoir::document::const_iterator to);

		std::tr1::shared_ptr<cainteoir::tts::parameter>
		parameter(cainteoir::tts::parameter::type aType);
	private:
		std::map<std::string, engine *> enginelist;
		engine *active;
		const rdf::uri *selectedVoice;
	};
}}

#endif
