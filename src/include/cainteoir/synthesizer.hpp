/* PHoneme/Prosody Synthesizer API.
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

#ifndef CAINTEOIR_ENGINE_SYNTHESIZER_HPP
#define CAINTEOIR_ENGINE_SYNTHESIZER_HPP

#include "phoneme.hpp"
#include "content.hpp"
#include "text.hpp"
#include "audio.hpp"

namespace cainteoir { namespace tts
{
	struct phone
	{
		ipa::phoneme phoneme1;
		ipa::phoneme phoneme2;
		css::time duration;

		phone(ipa::phoneme aPhoneme1, ipa::phoneme aPhoneme2, css::time aDuration)
			: phoneme1(aPhoneme1)
			, phoneme2(aPhoneme2)
			, duration(aDuration)
		{
		}

		phone()
			: phoneme1(ipa::unspecified)
			, phoneme2(ipa::unspecified)
		{
		}
	};

	struct duration
	{
		css::time mean;
		css::time sdev;

		duration()
		{
		}

		duration(css::time aMean, css::time aStdDev)
			: mean(aMean)
			, sdev(aStdDev.as(aMean.units()))
		{
		}

		css::time value(float zscore) const
		{
			return { mean.value() + (zscore * sdev.value()), mean.units() };
		}
	};

	struct envelope_t
	{
		int offset;
		css::frequency pitch;
	};

	struct prosody
	{
		phone first;
		phone second;
		std::vector<envelope_t> envelope;

		prosody(const phone &aFirst,
		        const phone &aSecond,
		        const std::initializer_list<envelope_t> &aEnvelope)
			: first(aFirst)
			, second(aSecond)
			, envelope(aEnvelope)
		{
		}

		prosody()
		{
		}
	};

	bool
	write_diphone(const tts::prosody &aProsody,
	              const std::shared_ptr<tts::phoneme_writer> &aPhonemeSet,
	              FILE *aOutput);

	struct prosody_reader : public prosody
	{
		virtual bool read() = 0;

		virtual ~prosody_reader() {}
	};

	std::shared_ptr<prosody_reader>
	createPhoReader(const std::shared_ptr<phoneme_parser> &aPhonemeSet,
	                const std::shared_ptr<buffer> &aBuffer);

	std::shared_ptr<prosody_reader>
	createDiphoneReader(const std::shared_ptr<prosody_reader> &aProsody);

	std::shared_ptr<prosody_reader>
	createProsodyReader(const std::shared_ptr<text_reader> &aTextReader);

	struct prosody_writer
	{
		virtual void reset(FILE *aOutput) = 0;

		virtual bool write(const prosody &aProsody) = 0;

		virtual ~prosody_writer() {}
	};

	std::shared_ptr<prosody_writer> createPhoWriter(const std::shared_ptr<phoneme_writer> &aPhonemeSet);

	struct synthesizer : public audio_info
	{
		virtual void bind(const std::shared_ptr<prosody_reader> &aProsody) = 0;

		virtual bool synthesize(audio *out) = 0;
	};

	void read_voice_metadata(rdf::graph &aMetadata);

	std::shared_ptr<synthesizer> create_voice_synthesizer(rdf::graph &aMetadata, const rdf::uri *voice);
}}

#endif
