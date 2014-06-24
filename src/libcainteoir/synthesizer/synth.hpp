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
	// MBROLA

	void read_mbrola_voices(rdf::graph &aMetadata);

	std::shared_ptr<synthesizer> create_mbrola_synthesizer(rdf::graph &aMetadata, const rdf::uri &aVoice);

	// Cainteoir Text-to-Speech

	void read_cainteoir_voices(rdf::graph &aMetadata);

	std::shared_ptr<synthesizer> create_cainteoir_synthesizer(rdf::graph &aMetadata, const rdf::uri &aVoice);
}}

#endif