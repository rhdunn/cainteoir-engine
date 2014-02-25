/* Phoneme Model API.
 *
 * Copyright (C) 2013-2014 Reece H. Dunn
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

#ifndef CAINTEOIR_ENGINE_PHONEME_HPP
#define CAINTEOIR_ENGINE_PHONEME_HPP

#include "buffer.hpp"

#include <stdexcept>
#include <cstdint>

namespace cainteoir { namespace ipa
{
	enum feature
	{
		// features

		place_of_articulation  = UINT64_C(0x000000000000000F),
		manner_of_articulation = UINT64_C(0x00000000000000F0),
		high                   = UINT64_C(0x0000000000000100),
		mid                    = UINT64_C(0x0000000000000200),
		low                    = UINT64_C(0x0000000000000400),
		lax                    = UINT64_C(0x0000000000001000),
		front                  = UINT64_C(0x0000000000002000),
		back                   = UINT64_C(0x0000000000004000),
		length                 = UINT64_C(0x0000000000018000),
		stress                 = UINT64_C(0x0000000000600000),
		unexploded             = UINT64_C(0x0000000000080000),
		syllabic               = UINT64_C(0x0000000000100000),
		murmured               = UINT64_C(0x0000000000200000),
		aspirated              = UINT64_C(0x0000000000400000),
		velarized              = UINT64_C(0x0000000000800000),
		labialized             = UINT64_C(0x0000000001000000),
		palatalized            = UINT64_C(0x0000000002000000),
		rhoticized             = UINT64_C(0x0000000004000000),
		nasalized              = UINT64_C(0x0000000008000000),
		pharyngealized         = UINT64_C(0x0000000010000000),
		rounded                = UINT64_C(0x0000000020000000),
		lateral                = UINT64_C(0x0000000040000000),
		voiced                 = UINT64_C(0x0000000080000000),

		// place of articulation

		bilabial        = UINT64_C(0x0000000000000000),
		labio_dental    = UINT64_C(0x0000000000000001),
		dental          = UINT64_C(0x0000000000000002),
		alveolar        = UINT64_C(0x0000000000000003),
		palato_alveolar = UINT64_C(0x0000000000000004),
		retroflex       = UINT64_C(0x0000000000000005),
		alveolo_palatal = UINT64_C(0x0000000000000006),
		palatal         = UINT64_C(0x0000000000000007),
		labio_palatal   = UINT64_C(0x0000000000000008),
		velar           = UINT64_C(0x0000000000000009),
		labio_velar     = UINT64_C(0x000000000000000A),
		uvular          = UINT64_C(0x000000000000000B),
		pharyngeal      = UINT64_C(0x000000000000000C),
		epiglottal      = UINT64_C(0x000000000000000D),
		glottal         = UINT64_C(0x000000000000000E),

		// manner of articulation

		plosive     = UINT64_C(0x0000000000000000),
		fricative   = UINT64_C(0x0000000000000010),
		nasal       = UINT64_C(0x0000000000000020),
		approximant = UINT64_C(0x0000000000000030),
		vowel       = UINT64_C(0x0000000000000040),
		trill       = UINT64_C(0x0000000000000050),
		flap        = UINT64_C(0x0000000000000060),
		click       = UINT64_C(0x0000000000000070),
		ejective    = UINT64_C(0x0000000000000080),
		implosive   = UINT64_C(0x0000000000000090),

		// vowel height

		vowel_height = high | mid | low | lax,
		//           = high
		semi_high    = high | lax,
		upper_mid    = high | mid,
		//           = mid
		lower_mid    = low  | mid,
		semi_low     = low  | lax,
		//           = low

		// vowel backness

		vowel_backness = front | back,
		//             = front
		center         = front | back,
		//             = back

		// stress

		unstressed       = UINT64_C(0x0000000000000000),
		primary_stress   = UINT64_C(0x0000000000200000),
		secondary_stress = UINT64_C(0x0000000000400000),
		extra_stress     = UINT64_C(0x0000000000600000),

		// length

		extra_short = UINT64_C(0x0000000000010000),
		short_      = UINT64_C(0x0000000000000000),
		half_long   = UINT64_C(0x0000000000018000),
		long_       = UINT64_C(0x0000000000008000),
	};

	struct phoneme
	{
		typedef uint64_t value_type;

		phoneme(value_type value = 0)
			: mValue(value)
		{
		}

		value_type get(value_type mask) const { return mValue & mask; }

		phoneme &set(value_type value)
		{
			mValue |= value;
			return *this;
		}

		phoneme &set(value_type value, value_type mask)
		{
			mValue = (mValue & ~mask) | (value & mask);
			return *this;
		}

		phoneme &set(const char *feature);

		phoneme &clear(value_type value)
		{
			mValue &= ~value;
			return *this;
		}

		bool operator==(const phoneme &rhs) const { return mValue == rhs.mValue; }
		bool operator!=(const phoneme &rhs) const { return mValue != rhs.mValue; }
	private:
		value_type mValue;
	};
}}

namespace cainteoir { namespace tts
{
	enum class feature : uint8_t
	{
		unspecified,

		vowel,

		// phonation

		voiced,
		voiceless,
		murmured,

		// place of articulation

		bilabial,
		labio_dental,
		dental,
		alveolar,
		retroflex,
		palato_alveolar,
		palatal,
		velar,
		uvular,
		pharyngeal,
		epiglottal,
		glottal,

		// manner of articulation

		lateral,
		sibilant,

		plosive,
		fricative,
		nasal,
		approximant,
		trill,
		flap,
		click,
		ejective,
		implosive,

		// vowel height

		high,
		semi_high,
		upper_mid,
		mid,
		lower_mid,
		semi_low,
		low,

		// vowel backness

		front,
		center,
		back,

		// vowel rounding

		unrounded,
		rounded,

		// stress

		primary_stress,
		secondary_stress,

		// prosody

		syllable_break,
		silent_pause,

		// length

		extra_short,
		half_long,
		long_,

		// consonant release

		aspirated,
		unreleased,

		// syllabicity

		syllabic,
		non_syllabic,

		// co-articulation

		velarized,
		labialized,
		palatalized,
		rhoticized,
		nasalized,
		pharyngealized,
	};

	class phoneme
	{
		feature features[8];
	public:
		phoneme();

		phoneme(const feature a,
		        const feature b,
		        const feature c,
		        const feature d = feature::unspecified,
		        const feature e = feature::unspecified,
		        const feature f = feature::unspecified,
		        const feature g = feature::unspecified,
		        const feature h = feature::unspecified);

		bool contains(const feature f) const;

		bool remove(const feature f);

		bool add(const feature f);

		bool operator==(const phoneme &rhs) const;

		bool operator!=(const phoneme &rhs) const
		{
			return !operator==(rhs);
		}

		const feature *begin() const { return features; }
		const feature *end()   const { return features + 8; }
	};

	std::pair<bool, feature> get_feature_id(const char *abbreviation);

	const char *get_feature_abbreviation(const feature f);

	const char *get_feature_name(const feature f);

	struct phoneme_error : public std::runtime_error
	{
		phoneme_error(const std::string &msg) : std::runtime_error(msg)
		{
		}
	};

	struct phoneme_reader : public phoneme
	{
		virtual void reset(const std::shared_ptr<buffer> &aBuffer) = 0;

		virtual bool read() = 0;

		virtual ~phoneme_reader() {}
	};

	std::shared_ptr<phoneme_reader> createExplicitFeaturePhonemeReader();

	std::shared_ptr<phoneme_reader> createPhonemeReader(const char *aPhonemeSet);

	struct phoneme_writer
	{
		virtual void reset(FILE *aOutput) = 0;

		virtual bool write(const phoneme &aPhoneme) = 0;

		virtual const char *name() const = 0;

		virtual ~phoneme_writer() {}
	};

	std::shared_ptr<phoneme_writer> createExplicitFeaturePhonemeWriter();

	std::shared_ptr<phoneme_writer> createPhonemeWriter(const char *aPhonemeSet);

	enum class stress_type
	{
		as_transcribed,
		vowel,
		syllable,
	};

	void make_stressed(std::list<phoneme> &aPhonemes, stress_type aType);
}}

#endif
