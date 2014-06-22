/* Text-to-Speech Synthesizers.
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

#include "config.h"
#include "compatibility.hpp"
#include "i18n.h"

#include "synthesizer.hpp"

namespace rdf = cainteoir::rdf;
namespace rql = cainteoir::rdf::query;
namespace tts = cainteoir::tts;

void
tts::read_voice_metadata(rdf::graph &aMetadata)
{
	read_mbrola_voices(aMetadata);
	read_cainteoir_voices(aMetadata);
}

std::shared_ptr<tts::synthesizer>
tts::create_voice_synthesizer(rdf::graph &aMetadata, const rdf::uri *voice)
{
	if (!voice) return {};

	const std::string &synthesizer = voice->ns;

	if (synthesizer == "http://reecedunn.co.uk/tts/synthesizer/mbrola#")
	{
		const std::string name = rql::select_value<std::string>(aMetadata,
			rql::subject == *voice && rql::predicate == rdf::tts("name"));
		return create_mbrola_synthesizer(name.c_str());
	}

	if (synthesizer == "http://reecedunn.co.uk/tts/synthesizer/cainteoir#")
	{
		const std::string data = rql::select_value<std::string>(aMetadata,
			rql::subject == *voice && rql::predicate == rdf::tts("data"));
		return create_cainteoir_synthesizer(data.c_str());
	}

	return {};
}
