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

#include <cstdint>

namespace cainteoir { namespace tts
{
	enum class feature : uint8_t
	{
		unspecified,

		voiced,
		voiceless,

		bilabial,
		labiodental,
		dental,
		alveolar,
		retroflex,
		palato_alveolvar,
		palatal,
		velar,
		labio_velar,
		uvular,
		pharyngeal,
		glottal,

		vowel,
		lateral,

		plosive,
		fricative,
		nasal,
		oral,
		approximant,
		central,
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
		low,
		front,
		center,
		back,

		unrounded,
		rounded,

		aspirated,
		unexploded,
		syllabic,
		murmured,
		long_,
		velarized,
		labialized,
		palatalized,
		rhoticized,
		nasalized,
		pharyngealized,
	};

	class phoneme
	{
		feature features[4];
	public:
		phoneme(const feature a,
		        const feature b,
		        const feature c,
		        const feature d = feature::unspecified);

		bool contains(const feature f) const;
	};
}}

#endif
