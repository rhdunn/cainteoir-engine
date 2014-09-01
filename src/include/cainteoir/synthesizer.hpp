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

	typedef float (*probability_distribution)();

	float zero_distribution();

	float normal_distribution();

	template <typename UnitT>
	struct zscore
	{
		UnitT mean;
		UnitT sdev;

		zscore()
		{
		}

		zscore(const UnitT &aMean, const UnitT &aStdDev)
			: mean(aMean)
			, sdev(aStdDev.as(aMean.units()))
		{
		}

		UnitT value(float aScore) const
		{
			return { mean.value() + (aScore * sdev.value()), mean.units() };
		}
	};

	typedef zscore<css::time> duration;

	struct duration_model
	{
		virtual css::time
		lookup(const tts::phone &p, tts::probability_distribution d) const = 0;

		virtual ~duration_model() {}
	};

	std::shared_ptr<duration_model>
	createFixedDurationModel(css::time aDuration);

	std::shared_ptr<duration_model>
	createDurationModel(const std::shared_ptr<buffer> &aDurationModel);

	struct envelope_t
	{
		int offset;
		css::frequency pitch;
	};

	typedef zscore<css::frequency> pitch;

	struct pitch_model
	{
		pitch_model(const css::frequency &aBase,
		            const css::frequency &aStep,
		            const css::frequency &aSdev);

		const tts::pitch &tone(ipa::phoneme::value_type aTone) const;

		std::vector<envelope_t>
		envelope(ipa::phoneme aTones,
		         tts::probability_distribution aProbabilityDistribution) const;

		tts::pitch top;
		tts::pitch high;
		tts::pitch mid;
		tts::pitch low;
		tts::pitch bottom;
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
	createProsodyReader(const std::shared_ptr<phoneme_reader> &aPhonemes,
	                    const std::shared_ptr<duration_model> &aDurationModel,
	                    const std::shared_ptr<tts::pitch_model> &aPitchModel,
	                    tts::probability_distribution aProbabilityDistribution);

	#pragma pack(1)
	struct phoneme_units
	{
		ipa::phoneme phoneme1;
		ipa::phoneme phoneme2;
		uint16_t first_unit;
		uint8_t num_units;
	};
	#pragma pack(0)

	std::shared_ptr<prosody_reader>
	create_unit_reader(const std::shared_ptr<prosody_reader> &aProsody,
	                   const std::vector<unit_t> &aUnits,
	                   const range<const phoneme_units *> &aPhonemes);

	struct prosody_writer
	{
		virtual void reset(FILE *aOutput) = 0;

		virtual bool write(const prosody &aProsody) = 0;

		virtual ~prosody_writer() {}
	};

	std::shared_ptr<prosody_writer> createPhoWriter(const std::shared_ptr<phoneme_writer> &aPhonemeSet);

	enum state_t
	{
		stopped,
		speaking,
		paused,
	};

	struct synthesis_callback
	{
		virtual ~synthesis_callback() {}

		virtual state_t state() const = 0;

		virtual void onaudiodata(short *data, int nsamples) = 0;

		virtual void ontextrange(size_t pos, size_t len) = 0;
	};

	struct synthesizer : public audio_info
	{
		virtual void bind(const std::shared_ptr<prosody_reader> &aProsody) = 0;

		virtual bool synthesize(audio *out) = 0;
	};

	struct voice
	{
		virtual ~voice() {}

		virtual std::shared_ptr<tts::synthesizer>
		synthesizer() = 0;

		virtual std::shared_ptr<tts::duration_model>
		durations() = 0;

		virtual std::shared_ptr<tts::pitch_model>
		pitch_model() = 0;

		virtual std::shared_ptr<tts::prosody_reader>
		unit_reader(const std::shared_ptr<tts::prosody_reader> &aProsody) = 0;

		virtual std::shared_ptr<tts::prosody_writer>
		unit_writer() = 0;
	};

	void read_voice_metadata(rdf::graph &aMetadata);

	std::shared_ptr<voice> create_voice(rdf::graph &aMetadata, const rdf::uri *voice);

	void compile_voice(const char *aFileName, FILE *aOutput);
}}

#endif
