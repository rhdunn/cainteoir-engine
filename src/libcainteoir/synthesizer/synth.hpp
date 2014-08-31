/* Text-to-Speech Synthesizer Internal API.
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

#ifndef CAINTEOIR_SYNTHESIZER_SYNTH_HPP
#define CAINTEOIR_SYNTHESIZER_SYNTH_HPP

#include <cainteoir/synthesizer.hpp>

namespace cainteoir { namespace tts
{
	// VoiceDB Data Parser Helpers /////////////////////////////////////////////////////////

	static inline constexpr uint32_t make_magic32(uint8_t a,  uint8_t b, uint8_t c)
	{
		return ((uint32_t)a << 16) | ((uint32_t)b << 8) | ((uint32_t)c);
	}

	static constexpr uint16_t VOICEDB_HEADER_SIZE = 29;
	static constexpr uint16_t PITCH_DATA_SECTION_SIZE = 11;
	static constexpr uint16_t STRING_TABLE_HEADER_SIZE = 5;
	static constexpr uint16_t TABLE_SECTION_SIZE = 5;
	static constexpr uint16_t DURATION_TABLE_ENTRY_SIZE = 18;
	static constexpr uint16_t PHONEME_UNIT_TABLE_ENTRY_SIZE = 5;
	static constexpr uint16_t PHONEME_TABLE_ENTRY_SIZE = 19;

	static constexpr uint32_t PITCH_DATA_MAGIC = make_magic32('P', 'T', 'C');
	static constexpr uint32_t STRING_TABLE_MAGIC = make_magic32('S', 'T', 'R');
	static constexpr uint32_t DURATION_TABLE_MAGIC = make_magic32('D', 'U', 'R');
	static constexpr uint32_t PHONEME_UNIT_TABLE_MAGIC = make_magic32('P', 'U', 'T');
	static constexpr uint32_t PHONEME_TABLE_MAGIC = make_magic32('P', 'H', 'O');

	std::shared_ptr<duration_model>
	createDurationModel(native_endian_buffer &aData);

	void read_phoneme_units(cainteoir::native_endian_buffer &data, std::vector<unit_t> &units);

	// Voice Synthesizers //////////////////////////////////////////////////////////////////

	std::shared_ptr<voice>
	create_mbrola_voice(const std::shared_ptr<cainteoir::buffer> &aData,
	                    const rdf::graph &aMetadata,
	                    const rdf::uri &aVoice);
}}

#endif
