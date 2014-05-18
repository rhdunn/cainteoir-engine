/* Experimental interface to the MBROLA synthesizer.
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

#include "config.h"
#include "compatibility.hpp"
#include "i18n.h"
#include "options.hpp"

#include <cainteoir/prosody.hpp>
#include <cainteoir/buffer.hpp>
#include <cainteoir/audio.hpp>
#include <stdexcept>

#include <unistd.h>

namespace tts = cainteoir::tts;

namespace cainteoir { namespace tts
{
	struct synthesizer
	{
		virtual ~synthesizer() {}
	};
}}

#if defined(HAVE_MBROLA)

struct mbrola_synthesizer : public tts::synthesizer
{
	mbrola_synthesizer(const char *database);
private:
	pid_t pid;
};

mbrola_synthesizer::mbrola_synthesizer(const char *database)
{
	pid = fork();
	if (pid == -1) throw std::runtime_error(strerror(errno));
	if (pid == 0)
	{
		execlp("mbrola", "mbrola",
		       "-e",     // ignore fatal errors on unknown diphones
		       database, // voice file database
		       "-",      // pho file input (stdin)
		       "-.wav",  // audio output (stdout)
                       nullptr);
		_exit(1);
	}
}

std::shared_ptr<tts::synthesizer> create_mbrola_voice(const char *voice)
{
	char database[256];
	snprintf(database, sizeof(database), MBROLA_DIR "/%s/%s", voice, voice);

	return std::make_shared<mbrola_synthesizer>(database);
}

#else

std::shared_ptr<tts::synthesizer> create_mbrola_voice(const char *voice)
{
	return {};
}

#endif

int main(int argc, char **argv)
{
	try
	{
		if (argc != 2)
		{
			fprintf(stderr, "usage: mbrola VOICE_NAME\n");
			return 0;
		}

		auto voice = create_mbrola_voice(argv[1]);
		if (!voice) throw std::runtime_error("cannot find the specified MBROLA voice");
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
