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
	static constexpr uint16_t VOICEDB_HEADER_SIZE = 29;
	static constexpr uint16_t STRING_TABLE_HEADER_SIZE = 5;

	std::shared_ptr<voice>
	create_mbrola_voice(const std::shared_ptr<cainteoir::buffer> &aData,
	                    const rdf::graph &aMetadata,
	                    const rdf::uri &aVoice);
}}

#endif
