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
#include <vector>

namespace cainteoir { namespace ipa
{
	typedef uint64_t feature_t;

	#if __WORDSIZE == 64
	#   define FEATURE_C(c) c ## UL
	#else
	#   define FEATURE_C(c) c ## ULL
	#endif

	constexpr feature_t main                   = FEATURE_C(0x00000000001FFFFF);
	constexpr feature_t diacritics             = FEATURE_C(0x00000FFFFFE00000);
	constexpr feature_t reserved               = FEATURE_C(0x000EF00000000000);
	constexpr feature_t suprasegmentals        = FEATURE_C(0xFFF1000000000000);

	// features

	// main ------------------------------------------------ 00000000001FFFFF
	constexpr feature_t phoneme_type           = FEATURE_C(0x000000000000000F);
	constexpr feature_t unit_value             = FEATURE_C(0x00000000000FFFF0);
	// ... consonants -------------------------------------- 0000000000003FF0
	constexpr feature_t place_of_articulation  = FEATURE_C(0x00000000000000F0);
	constexpr feature_t manner_of_articulation = FEATURE_C(0x0000000000000700);
	constexpr feature_t lateral                = FEATURE_C(0x0000000000000800);
	constexpr feature_t sibilant               = FEATURE_C(0x0000000000001000);
	constexpr feature_t voiced                 = FEATURE_C(0x0000000000002000);
	// ... vowels ------------------------------------------ 00000000001FC000
	constexpr feature_t rounded                = FEATURE_C(0x0000000000004000);
	constexpr feature_t high                   = FEATURE_C(0x0000000000008000);
	constexpr feature_t mid                    = FEATURE_C(0x0000000000010000);
	constexpr feature_t low                    = FEATURE_C(0x0000000000020000);
	constexpr feature_t lax                    = FEATURE_C(0x0000000000040000);
	constexpr feature_t front                  = FEATURE_C(0x0000000000080000);
	constexpr feature_t back                   = FEATURE_C(0x0000000000100000);
	// diacritics ------------------------------------------ 00000FFFFFE00000
	constexpr feature_t ejective               = FEATURE_C(0x0000000000200000);
	constexpr feature_t release                = FEATURE_C(0x0000000001C00000);
	constexpr feature_t phonation              = FEATURE_C(0x000000000E000000);
	constexpr feature_t articulation           = FEATURE_C(0x00000000F0000000);
	constexpr feature_t rounding               = FEATURE_C(0x0000000300000000);
	constexpr feature_t coarticulation         = FEATURE_C(0x0000001C00000000);
	constexpr feature_t rhoticized             = FEATURE_C(0x0000002000000000);
	constexpr feature_t tongue_root            = FEATURE_C(0x000000C000000000);
	constexpr feature_t joined_to_next_phoneme = FEATURE_C(0x0000010000000000);
	constexpr feature_t syllabicity            = FEATURE_C(0x0000060000000000);
	constexpr feature_t rhotic                 = FEATURE_C(0x0000080000000000);
	// reserved (unassigned) ------------------------------- 000EF00000000000
	// suprasegmentals ------------------------------------- FFF1000000000000
	constexpr feature_t tone_start             = FEATURE_C(0x0031000000000000);
	constexpr feature_t tone_middle            = FEATURE_C(0x01C0000000000000);
	constexpr feature_t tone_end               = FEATURE_C(0x0E00000000000000);
	constexpr feature_t stress                 = FEATURE_C(0x3000000000000000);
	constexpr feature_t length                 = FEATURE_C(0xC000000000000000);

	// phoneme type

	constexpr feature_t consonant        = FEATURE_C(0x0000000000000000);
	constexpr feature_t vowel            = FEATURE_C(0x0000000000000001);
	constexpr feature_t separator        = FEATURE_C(0x0000000000000002);
	constexpr feature_t syllable_break   = FEATURE_C(0x0000000000000003);
	constexpr feature_t linking          = FEATURE_C(0x0000000000000004);
	constexpr feature_t foot_break       = FEATURE_C(0x0000000000000005);
	constexpr feature_t intonation_break = FEATURE_C(0x0000000000000006);
	constexpr feature_t global_rise      = FEATURE_C(0x0000000000000007);
	constexpr feature_t global_fall      = FEATURE_C(0x0000000000000008);
	constexpr feature_t upstep           = FEATURE_C(0x0000000000000009);
	constexpr feature_t downstep         = FEATURE_C(0x000000000000000A);
	constexpr feature_t unit             = FEATURE_C(0x000000000000000E);
	constexpr feature_t unspecified      = FEATURE_C(0x000000000000000F);

	// place of articulation

	constexpr feature_t bilabial        = FEATURE_C(0x0000000000000000);
	constexpr feature_t labio_dental    = FEATURE_C(0x0000000000000010);
	constexpr feature_t dental          = FEATURE_C(0x0000000000000020);
	constexpr feature_t alveolar        = FEATURE_C(0x0000000000000030);
	constexpr feature_t palato_alveolar = FEATURE_C(0x0000000000000040);
	constexpr feature_t retroflex       = FEATURE_C(0x0000000000000050);
	constexpr feature_t alveolo_palatal = FEATURE_C(0x0000000000000060);
	constexpr feature_t palatal         = FEATURE_C(0x0000000000000070);
	constexpr feature_t labio_palatal   = FEATURE_C(0x0000000000000080);
	constexpr feature_t velar           = FEATURE_C(0x0000000000000090);
	constexpr feature_t labio_velar     = FEATURE_C(0x00000000000000A0);
	constexpr feature_t uvular          = FEATURE_C(0x00000000000000B0);
	constexpr feature_t pharyngeal      = FEATURE_C(0x00000000000000C0);
	constexpr feature_t epiglottal      = FEATURE_C(0x00000000000000D0);
	constexpr feature_t glottal         = FEATURE_C(0x00000000000000E0);

	// manner of articulation

	constexpr feature_t plosive     = FEATURE_C(0x0000000000000000);
	constexpr feature_t fricative   = FEATURE_C(0x0000000000000100);
	constexpr feature_t nasal       = FEATURE_C(0x0000000000000200);
	constexpr feature_t approximant = FEATURE_C(0x0000000000000300);
	constexpr feature_t trill       = FEATURE_C(0x0000000000000400);
	constexpr feature_t flap        = FEATURE_C(0x0000000000000500);
	constexpr feature_t click       = FEATURE_C(0x0000000000000600);
	constexpr feature_t implosive   = FEATURE_C(0x0000000000000700);

	// vowel height

	constexpr feature_t vowel_height = high | mid | low | lax;
	//                              = high
	constexpr feature_t semi_high    = high | lax;
	constexpr feature_t upper_mid    = high | mid;
	//                              = mid
	constexpr feature_t lower_mid    = low  | mid;
	constexpr feature_t semi_low     = low  | lax;
	//                              = low

	// vowel backness

	constexpr feature_t vowel_backness = front | back;
	//                                = front
	constexpr feature_t center         = front | back;
	//                                = back

	// diacritics -- release

	constexpr feature_t aspirated       = FEATURE_C(0x0000000000400000);
	constexpr feature_t unexploded      = FEATURE_C(0x0000000000800000);
	constexpr feature_t nasal_release   = FEATURE_C(0x0000000000C00000);
	constexpr feature_t lateral_release = FEATURE_C(0x0000000001400000);

	// diacritics -- phonation

	constexpr feature_t breathy_voice = FEATURE_C(0x0000000002000000);
	constexpr feature_t slack_voice   = FEATURE_C(0x0000000004000000);
	constexpr feature_t modal_voice   = FEATURE_C(0x0000000006000000);
	constexpr feature_t stiff_voice   = FEATURE_C(0x0000000008000000);
	constexpr feature_t creaky_voice  = FEATURE_C(0x000000000A000000);
	constexpr feature_t murmured      = breathy_voice; // alias

	// diacritics -- articulation

	constexpr feature_t dentalized      = FEATURE_C(0x0000000010000000);
	constexpr feature_t linguolabial    = FEATURE_C(0x0000000020000000);
	constexpr feature_t apical          = FEATURE_C(0x0000000030000000);
	constexpr feature_t laminal         = FEATURE_C(0x0000000040000000);
	constexpr feature_t advanced        = FEATURE_C(0x0000000050000000);
	constexpr feature_t retracted       = FEATURE_C(0x0000000060000000);
	constexpr feature_t centralized     = FEATURE_C(0x0000000070000000);
	constexpr feature_t mid_centralized = FEATURE_C(0x0000000080000000);
	constexpr feature_t raised          = FEATURE_C(0x0000000090000000);
	constexpr feature_t lowered         = FEATURE_C(0x00000000A0000000);

	// diacritics -- rounding (co-articulation)

	constexpr feature_t more_rounded = FEATURE_C(0x0000000100000000);
	constexpr feature_t less_rounded = FEATURE_C(0x0000000200000000);

	// diacritics -- co-articulation

	constexpr feature_t labialized                  = FEATURE_C(0x0000000400000000);
	constexpr feature_t palatalized                 = FEATURE_C(0x0000000800000000);
	constexpr feature_t velarized                   = FEATURE_C(0x0000000C00000000);
	constexpr feature_t pharyngealized              = FEATURE_C(0x0000001000000000);
	constexpr feature_t velarized_or_pharyngealized = FEATURE_C(0x0000001400000000);
	constexpr feature_t nasalized                   = FEATURE_C(0x0000001800000000);

	// diacritics -- tongue root (co-articulation)

	constexpr feature_t advanced_tongue_root  = FEATURE_C(0x0000004000000000);
	constexpr feature_t retracted_tongue_root = FEATURE_C(0x0000008000000000);

	// diacritics -- syllabicity

	constexpr feature_t syllabic               = FEATURE_C(0x0000020000000000);
	constexpr feature_t non_syllabic           = FEATURE_C(0x0000040000000000);

	// suprasegmentals -- tone

	constexpr feature_t tone_start_top     = FEATURE_C(0x0001000000000000);
	constexpr feature_t tone_start_high    = FEATURE_C(0x0010000000000000);
	constexpr feature_t tone_start_mid     = FEATURE_C(0x0011000000000000);
	constexpr feature_t tone_start_low     = FEATURE_C(0x0020000000000000);
	constexpr feature_t tone_start_bottom  = FEATURE_C(0x0021000000000000);

	constexpr feature_t tone_middle_top    = FEATURE_C(0x0040000000000000);
	constexpr feature_t tone_middle_high   = FEATURE_C(0x0080000000000000);
	constexpr feature_t tone_middle_mid    = FEATURE_C(0x00C0000000000000);
	constexpr feature_t tone_middle_low    = FEATURE_C(0x0100000000000000);
	constexpr feature_t tone_middle_bottom = FEATURE_C(0x0140000000000000);

	constexpr feature_t tone_end_top       = FEATURE_C(0x0200000000000000);
	constexpr feature_t tone_end_high      = FEATURE_C(0x0400000000000000);
	constexpr feature_t tone_end_mid       = FEATURE_C(0x0600000000000000);
	constexpr feature_t tone_end_low       = FEATURE_C(0x0800000000000000);
	constexpr feature_t tone_end_bottom    = FEATURE_C(0x0A00000000000000);

	// suprasegmentals -- stress

	constexpr feature_t unstressed       = FEATURE_C(0x0000000000000000);
	constexpr feature_t primary_stress   = FEATURE_C(0x1000000000000000);
	constexpr feature_t secondary_stress = FEATURE_C(0x2000000000000000);
	constexpr feature_t extra_stress     = FEATURE_C(0x3000000000000000);

	// suprasegmentals -- length

	constexpr feature_t extra_short = FEATURE_C(0x4000000000000000);
	constexpr feature_t short_      = FEATURE_C(0x0000000000000000);
	constexpr feature_t half_long   = FEATURE_C(0xC000000000000000);
	constexpr feature_t long_       = FEATURE_C(0x8000000000000000);

	struct phoneme
	{
		typedef feature_t value_type;

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

		bool operator==(const value_type &rhs) const { return mValue == rhs; }
		bool operator!=(const value_type &rhs) const { return mValue != rhs; }

		bool operator<(const phoneme &rhs) const { return mValue < rhs.mValue; }
		bool operator>(const phoneme &rhs) const { return mValue > rhs.mValue; }
	private:
		value_type mValue;
	};

	inline bool operator==(const phoneme::value_type &lhs, const phoneme &rhs)
	{
		return rhs == lhs;
	}

	inline bool operator!=(const phoneme::value_type &lhs, const phoneme &rhs)
	{
		return rhs != lhs;
	}

	typedef std::list<phoneme> phonemes;
}}

namespace cainteoir { namespace tts
{
	struct phoneme_error : public std::runtime_error
	{
		phoneme_error(const std::string &msg) : std::runtime_error(msg)
		{
		}
	};

	struct phoneme_parser
	{
		virtual void initialize() = 0;

		virtual bool parse(const char * &aCurrent, const char *aEnd, ipa::phoneme &aPhoneme) = 0;

		virtual ~phoneme_parser() {}
	};

	std::shared_ptr<tts::phoneme_parser> createPhonemeParser(const char *aPhonemeSet);

	struct phoneme_reader : public ipa::phoneme
	{
		virtual void reset(const std::shared_ptr<buffer> &aBuffer) = 0;

		virtual bool read() = 0;

		virtual ~phoneme_reader() {}
	};

	std::shared_ptr<phoneme_reader> createPhonemeReader(const char *aPhonemeSet);

	std::shared_ptr<phoneme_reader>
	createPhonemeToPhonemeConverter(const char *aPhonemeToPhonemeRules,
	                                const std::shared_ptr<phoneme_reader> &aPhonemes);

	std::shared_ptr<phoneme_reader>
	createAccentConverter(const char *aAccent,
	                      const std::shared_ptr<phoneme_reader> &aPhonemes);

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

	void make_stressed(ipa::phonemes &aPhonemes, stress_type aType);

	enum class initial_stress
	{
		as_transcribed,
		primary,
		secondary,
		unstressed,
	};

	void make_stressed(const ipa::phonemes &aPhonemes,
	                   ipa::phonemes &aOutput,
	                   initial_stress aType);

	struct unit_t
	{
		const char *name;
		uint8_t phoneme_start;
		uint8_t unit_start;
		uint8_t unit_end;
	};

	std::shared_ptr<phoneme_writer>
	create_unit_writer(const std::vector<unit_t> &aUnits);
}}

#endif
