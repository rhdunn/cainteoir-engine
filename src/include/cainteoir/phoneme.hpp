/* Phoneme Model API.
 *
 * Copyright (C) 2013 Reece H. Dunn
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

namespace cainteoir { namespace tts
{
	enum class feature : uint8_t
	{
		unspecified,

		voiced,
		voiceless,

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

		vowel,
		rising_diphthong,
		falling_diphthong,
		lateral,

		plosive,
		fricative,
		affricate,
		nasal,
		approximant,
		trill,
		flap,
		click,
		ejective,
		implosive,

		high,
		semi_high,
		upper_mid,
		mid,
		lower_mid,
		semi_low,
		low,

		front,
		center,
		back,

		unrounded,
		rounded,

		primary_stress,
		secondary_stress,

		syllable_break,
		silent_pause,

		extra_short,
		half_long,
		long_,

		sibilant,
		aspirated,
		unexploded,
		syllabic,
		murmured,
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

		virtual ~phoneme_writer() {}
	};

	std::shared_ptr<phoneme_writer> createExplicitFeaturePhonemeWriter();

	std::shared_ptr<phoneme_writer> createPhonemeWriter(const char *aPhonemeSet);
}}

#endif
