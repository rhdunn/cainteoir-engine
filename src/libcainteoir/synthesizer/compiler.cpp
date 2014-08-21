/* Voice Database File Builder.
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

#include <cainteoir/synthesizer.hpp>
#include "../cainteoir_file_reader.hpp"

namespace tts = cainteoir::tts;

template <typename T>
void write(FILE *out, const T &data)
{
	fwrite(&data, sizeof(data), 1, out);
}

void
tts::compile_voice(const char *aFileName, FILE *aOutput)
{
	if (!aOutput) return;

	auto reader = cainteoir_file_reader(cainteoir::path(aFileName));

	fputs("VOICEDB", aOutput);
	write(aOutput, 0x3031); // endianness
}
