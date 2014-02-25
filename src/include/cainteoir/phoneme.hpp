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

		place_of_articulation  = 0x0000000F,
		manner_of_articulation = 0x000000F0,
		high                   = 0x00000100,
		mid                    = 0x00000200,
		low                    = 0x00000400,
		lax                    = 0x00001000,
		vowel_backness         = 0x00006000,
		//                     = 0x00002000,
		//                     = 0x00004000,
		//                     = 0x00008000,
		//                     = 0x00010000,
		//                     = 0x00020000,
		aspirated              = 0x00040000,
		unexploded             = 0x00080000,
		syllabic               = 0x00100000,
		murmured               = 0x00200000,
		long_                  = 0x00400000,
		velarized              = 0x00800000,
		labialized             = 0x01000000,
		palatalized            = 0x02000000,
		rhoticized             = 0x04000000,
		nasalized              = 0x08000000,
		pharyngealized         = 0x10000000,
		rounded                = 0x20000000,
		lateral                = 0x40000000,
		voiced                 = 0x80000000,

		// place of articulation

		bilabial        = 0x00000000,
		labio_dental    = 0x00000001,
		dental          = 0x00000002,
		alveolar        = 0x00000003,
		palato_alveolar = 0x00000004,
		retroflex       = 0x00000005,
		alveolo_palatal = 0x00000006,
		palatal         = 0x00000007,
		labio_palatal   = 0x00000008,
		velar           = 0x00000009,
		labio_velar     = 0x0000000A,
		uvular          = 0x0000000B,
		pharyngeal      = 0x0000000C,
		epiglottal      = 0x0000000D,
		glottal         = 0x0000000E,

		// manner of articulation

		plosive     = 0x00000000,
		fricative   = 0x00000010,
		nasal       = 0x00000020,
		approximant = 0x00000030,
		vowel       = 0x00000040,
		trill       = 0x00000050,
		flap        = 0x00000060,
		click       = 0x00000070,
		ejective    = 0x00000080,
		implosive   = 0x00000090,

		// vowel height

		vowel_height = high | mid | low | lax,
		semi_high    = high | lax,
		upper_mid    = high | mid,
		lower_mid    = low  | mid,
		semi_low     = low  | lax,

		// vowel backness

		front  = 0x00000000,
		center = 0x00002000,
		back   = 0x00004000,
	};

	struct phoneme
	{
		typedef uint32_t value_type;

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
