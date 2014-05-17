/* Prosodic Model API.
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

#ifndef CAINTEOIR_ENGINE_PROSODY_HPP
#define CAINTEOIR_ENGINE_PROSODY_HPP

#include "phoneme.hpp"
#include "content.hpp"
#include "audio.hpp"

namespace cainteoir { namespace tts
{
	struct envelope_t
	{
		int offset;
		css::frequency pitch;
	};

	struct prosody
	{
		ipa::phoneme phoneme1;
		ipa::phoneme phoneme2;
		css::time duration;
		std::vector<envelope_t> envelope;
	};

	struct prosody_reader : public prosody
	{
		virtual void reset(const std::shared_ptr<buffer> &aBuffer) = 0;

		virtual bool read() = 0;

		virtual ~prosody_reader() {}
	};

	std::shared_ptr<prosody_reader> createPhoReader(const std::shared_ptr<phoneme_reader> &aPhonemeSet);

	struct prosody_writer
	{
		virtual void reset(FILE *aOutput) = 0;

		virtual bool write(const prosody &aProsody) = 0;

		virtual ~prosody_writer() {}
	};

	std::shared_ptr<prosody_writer> createPhoWriter(const std::shared_ptr<phoneme_writer> &aPhonemeSet);
}}

#endif
