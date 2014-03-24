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
	constexpr uint64_t main                   = UINT64_C(0x00000000001FFFFF);
	constexpr uint64_t diacritics             = UINT64_C(0x000007FFFFE00000);
	constexpr uint64_t reserved               = UINT64_C(0x000EF80000000000);
	constexpr uint64_t suprasegmentals        = UINT64_C(0xFFF1000000000000);

	// features

	// main ---------------------------------------------- 00000000001FFFFF
	constexpr uint64_t phoneme_type           = UINT64_C(0x000000000000000F);
	// ... consonants ------------------------------------ 0000000000003FF0
	constexpr uint64_t place_of_articulation  = UINT64_C(0x00000000000000F0);
	constexpr uint64_t manner_of_articulation = UINT64_C(0x0000000000000700);
	constexpr uint64_t lateral                = UINT64_C(0x0000000000000800);
	constexpr uint64_t sibilant               = UINT64_C(0x0000000000001000);
	constexpr uint64_t voiced                 = UINT64_C(0x0000000000002000);
	// ... vowels ---------------------------------------- 00000000001FC000
	constexpr uint64_t rounded                = UINT64_C(0x0000000000004000);
	constexpr uint64_t high                   = UINT64_C(0x0000000000008000);
	constexpr uint64_t mid                    = UINT64_C(0x0000000000010000);
	constexpr uint64_t low                    = UINT64_C(0x0000000000020000);
	constexpr uint64_t lax                    = UINT64_C(0x0000000000040000);
	constexpr uint64_t front                  = UINT64_C(0x0000000000080000);
	constexpr uint64_t back                   = UINT64_C(0x0000000000100000);
	// diacritics ---------------------------------------- 000007FFFFE00000
	constexpr uint64_t ejective               = UINT64_C(0x0000000000200000);
	constexpr uint64_t release                = UINT64_C(0x0000000001C00000);
	constexpr uint64_t phonation              = UINT64_C(0x000000000E000000);
	constexpr uint64_t articulation           = UINT64_C(0x00000000F0000000);
	constexpr uint64_t rounding               = UINT64_C(0x0000000300000000);
	constexpr uint64_t coarticulation         = UINT64_C(0x0000001C00000000);
	constexpr uint64_t rhoticized             = UINT64_C(0x0000002000000000);
	constexpr uint64_t tongue_root            = UINT64_C(0x000000C000000000);
	constexpr uint64_t joined_to_next_phoneme = UINT64_C(0x0000010000000000);
	constexpr uint64_t syllabicity            = UINT64_C(0x0000060000000000);
	// reserved (unassigned) ----------------------------- 000EF80000000000
	// suprasegmentals ----------------------------------- FFF1000000000000
	constexpr uint64_t tone_start             = UINT64_C(0x0031000000000000);
	constexpr uint64_t tone_middle            = UINT64_C(0x01C0000000000000);
	constexpr uint64_t tone_end               = UINT64_C(0x0E00000000000000);
	constexpr uint64_t stress                 = UINT64_C(0x3000000000000000);
	constexpr uint64_t length                 = UINT64_C(0xC000000000000000);

	// phoneme type

	constexpr uint64_t consonant        = UINT64_C(0x0000000000000000);
	constexpr uint64_t vowel            = UINT64_C(0x0000000000000001);
	constexpr uint64_t pause            = UINT64_C(0x0000000000000002);
	constexpr uint64_t syllable_break   = UINT64_C(0x0000000000000003);
	constexpr uint64_t linking          = UINT64_C(0x0000000000000004);
	constexpr uint64_t foot_break       = UINT64_C(0x0000000000000005);
	constexpr uint64_t intonation_break = UINT64_C(0x0000000000000006);
	constexpr uint64_t global_rise      = UINT64_C(0x0000000000000007);
	constexpr uint64_t global_fall      = UINT64_C(0x0000000000000008);
	constexpr uint64_t upstep           = UINT64_C(0x0000000000000009);
	constexpr uint64_t downstep         = UINT64_C(0x000000000000000A);

	// place of articulation

	constexpr uint64_t bilabial        = UINT64_C(0x0000000000000000);
	constexpr uint64_t labio_dental    = UINT64_C(0x0000000000000010);
	constexpr uint64_t dental          = UINT64_C(0x0000000000000020);
	constexpr uint64_t alveolar        = UINT64_C(0x0000000000000030);
	constexpr uint64_t palato_alveolar = UINT64_C(0x0000000000000040);
	constexpr uint64_t retroflex       = UINT64_C(0x0000000000000050);
	constexpr uint64_t alveolo_palatal = UINT64_C(0x0000000000000060);
	constexpr uint64_t palatal         = UINT64_C(0x0000000000000070);
	constexpr uint64_t labio_palatal   = UINT64_C(0x0000000000000080);
	constexpr uint64_t velar           = UINT64_C(0x0000000000000090);
	constexpr uint64_t labio_velar     = UINT64_C(0x00000000000000A0);
	constexpr uint64_t uvular          = UINT64_C(0x00000000000000B0);
	constexpr uint64_t pharyngeal      = UINT64_C(0x00000000000000C0);
	constexpr uint64_t epiglottal      = UINT64_C(0x00000000000000D0);
	constexpr uint64_t glottal         = UINT64_C(0x00000000000000E0);

	// manner of articulation

	constexpr uint64_t plosive     = UINT64_C(0x0000000000000000);
	constexpr uint64_t fricative   = UINT64_C(0x0000000000000100);
	constexpr uint64_t nasal       = UINT64_C(0x0000000000000200);
	constexpr uint64_t approximant = UINT64_C(0x0000000000000300);
	constexpr uint64_t trill       = UINT64_C(0x0000000000000400);
	constexpr uint64_t flap        = UINT64_C(0x0000000000000500);
	constexpr uint64_t click       = UINT64_C(0x0000000000000600);
	constexpr uint64_t implosive   = UINT64_C(0x0000000000000700);

	// vowel height

	constexpr uint64_t vowel_height = high | mid | low | lax;
	//                              = high
	constexpr uint64_t semi_high    = high | lax;
	constexpr uint64_t upper_mid    = high | mid;
	//                              = mid
	constexpr uint64_t lower_mid    = low  | mid;
	constexpr uint64_t semi_low     = low  | lax;
	//                              = low

	// vowel backness

	constexpr uint64_t vowel_backness = front | back;
	//                                = front
	constexpr uint64_t center         = front | back;
	//                                = back

	// diacritics -- release

	constexpr uint64_t aspirated       = UINT64_C(0x0000000000400000);
	constexpr uint64_t unexploded      = UINT64_C(0x0000000000800000);
	constexpr uint64_t nasal_release   = UINT64_C(0x0000000000C00000);
	constexpr uint64_t lateral_release = UINT64_C(0x0000000001400000);

	// diacritics -- phonation

	constexpr uint64_t breathy_voice = UINT64_C(0x0000000002000000);
	constexpr uint64_t slack_voice   = UINT64_C(0x0000000004000000);
	constexpr uint64_t modal_voice   = UINT64_C(0x0000000006000000);
	constexpr uint64_t stiff_voice   = UINT64_C(0x0000000008000000);
	constexpr uint64_t creaky_voice  = UINT64_C(0x000000000A000000);
	constexpr uint64_t murmured      = breathy_voice; // alias

	// diacritics -- articulation

	constexpr uint64_t dentalized      = UINT64_C(0x0000000010000000);
	constexpr uint64_t linguolabial    = UINT64_C(0x0000000020000000);
	constexpr uint64_t apical          = UINT64_C(0x0000000030000000);
	constexpr uint64_t laminal         = UINT64_C(0x0000000040000000);
	constexpr uint64_t advanced        = UINT64_C(0x0000000050000000);
	constexpr uint64_t retracted       = UINT64_C(0x0000000060000000);
	constexpr uint64_t centralized     = UINT64_C(0x0000000070000000);
	constexpr uint64_t mid_centralized = UINT64_C(0x0000000080000000);
	constexpr uint64_t raised          = UINT64_C(0x0000000090000000);
	constexpr uint64_t lowered         = UINT64_C(0x00000000A0000000);

	// diacritics -- rounding (co-articulation)

	constexpr uint64_t more_rounded = UINT64_C(0x0000000100000000);
	constexpr uint64_t less_rounded = UINT64_C(0x0000000200000000);

	// diacritics -- co-articulation

	constexpr uint64_t labialized                  = UINT64_C(0x0000000400000000);
	constexpr uint64_t palatalized                 = UINT64_C(0x0000000800000000);
	constexpr uint64_t velarized                   = UINT64_C(0x0000000C00000000);
	constexpr uint64_t pharyngealized              = UINT64_C(0x0000001000000000);
	constexpr uint64_t velarized_or_pharyngealized = UINT64_C(0x0000001400000000);
	constexpr uint64_t nasalized                   = UINT64_C(0x0000001800000000);

	// diacritics -- tongue root (co-articulation)

	constexpr uint64_t advanced_tongue_root  = UINT64_C(0x0000004000000000);
	constexpr uint64_t retracted_tongue_root = UINT64_C(0x0000008000000000);

	// diacritics -- syllabicity

	constexpr uint64_t syllabic               = UINT64_C(0x0000020000000000);
	constexpr uint64_t non_syllabic           = UINT64_C(0x0000040000000000);

	// suprasegmentals -- tone

	constexpr uint64_t tone_start_top     = UINT64_C(0x0001000000000000);
	constexpr uint64_t tone_start_high    = UINT64_C(0x0010000000000000);
	constexpr uint64_t tone_start_mid     = UINT64_C(0x0011000000000000);
	constexpr uint64_t tone_start_low     = UINT64_C(0x0020000000000000);
	constexpr uint64_t tone_start_bottom  = UINT64_C(0x0021000000000000);

	constexpr uint64_t tone_middle_top    = UINT64_C(0x0040000000000000);
	constexpr uint64_t tone_middle_high   = UINT64_C(0x0080000000000000);
	constexpr uint64_t tone_middle_mid    = UINT64_C(0x00C0000000000000);
	constexpr uint64_t tone_middle_low    = UINT64_C(0x0100000000000000);
	constexpr uint64_t tone_middle_bottom = UINT64_C(0x0140000000000000);

	constexpr uint64_t tone_end_top       = UINT64_C(0x0200000000000000);
	constexpr uint64_t tone_end_high      = UINT64_C(0x0400000000000000);
	constexpr uint64_t tone_end_mid       = UINT64_C(0x0600000000000000);
	constexpr uint64_t tone_end_low       = UINT64_C(0x0800000000000000);
	constexpr uint64_t tone_end_bottom    = UINT64_C(0x0A00000000000000);

	// suprasegmentals -- stress

	constexpr uint64_t unstressed       = UINT64_C(0x0000000000000000);
	constexpr uint64_t primary_stress   = UINT64_C(0x1000000000000000);
	constexpr uint64_t secondary_stress = UINT64_C(0x2000000000000000);
	constexpr uint64_t extra_stress     = UINT64_C(0x3000000000000000);

	// suprasegmentals -- length

	constexpr uint64_t extra_short = UINT64_C(0x4000000000000000);
	constexpr uint64_t short_      = UINT64_C(0x0000000000000000);
	constexpr uint64_t half_long   = UINT64_C(0xC000000000000000);
	constexpr uint64_t long_       = UINT64_C(0x8000000000000000);

	struct phoneme
	{
		typedef uint64_t value_type;

		phoneme(value_type value = 0)
			: mValue(value)
		{
		}

		value_type get(value_type mask) const { return mValue & mask; }

		bool get(const char *feature) const;

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

		bool operator<(const phoneme &rhs) const { return mValue < rhs.mValue; }
		bool operator>(const phoneme &rhs) const { return mValue > rhs.mValue; }
	private:
		value_type mValue;
	};
}}

namespace cainteoir { namespace tts
{
	struct phoneme_error : public std::runtime_error
	{
		phoneme_error(const std::string &msg) : std::runtime_error(msg)
		{
		}
	};

	struct phoneme_reader : public ipa::phoneme
	{
		virtual void reset(const std::shared_ptr<buffer> &aBuffer) = 0;

		virtual bool read() = 0;

		virtual ~phoneme_reader() {}
	};

	std::shared_ptr<phoneme_reader> createPhonemeReader(const char *aPhonemeSet);

	struct phoneme_writer
	{
		virtual void reset(FILE *aOutput) = 0;

		virtual bool write(const ipa::phoneme &aPhoneme) = 0;

		virtual void flush();

		virtual const char *name() const = 0;

		virtual ~phoneme_writer() {}
	};

	std::shared_ptr<phoneme_writer> createPhonemeWriter(const char *aPhonemeSet);

	enum class stress_type
	{
		as_transcribed,
		vowel,
		syllable,
	};

	void make_stressed(std::list<ipa::phoneme> &aPhonemes, stress_type aType);
}}

#endif
