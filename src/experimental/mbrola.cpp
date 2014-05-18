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
#include <fcntl.h>

namespace tts = cainteoir::tts;
namespace rdf = cainteoir::rdf;

namespace cainteoir { namespace tts
{
	struct synthesizer : public audio_info
	{
	};
}}

#if defined(HAVE_MBROLA)

struct pipe_t
{
	enum fd_t
	{
		read_fd  = 0,
		write_fd = 1,
	};

	pipe_t()
		: fds{ -1, -1 }
	{
		if (pipe(fds) == -1)
			throw std::runtime_error(strerror(errno));
	}

	~pipe_t()
	{
		::close(fds[read_fd]);
		::close(fds[write_fd]);
	}

	void dup(fd_t type, int fd)
	{
		if (dup2(fds[type], fd) == -1)
			throw std::runtime_error(strerror(errno));
		close(type == read_fd ? write_fd : read_fd);
	}

	void close(fd_t type)
	{
		::close(fds[type]);
		fds[type] = -1;
	}

	ssize_t read(void *buf, size_t count);

	FILE *open(const char *mode)
	{
		FILE *ret = fdopen(fds[(mode[0] == 'r') ? read_fd : write_fd], mode);
		if (!ret) throw std::runtime_error(strerror(errno));
		fds[(mode[0] == 'r') ? read_fd : write_fd] = -1;
		return ret;
	}

	void set_flags(fd_t type, int flags)
	{
		fcntl(fds[type], F_SETFL, fcntl(fds[type], F_GETFL) | flags);
	}

	void clear_flags(fd_t type, int flags)
	{
		fcntl(fds[type], F_SETFL, fcntl(fds[type], F_GETFL) & ~flags);
	}
private:
	int fds[2];
};

ssize_t pipe_t::read(void *buf, size_t count)
{
	for (;;)
	{
		ssize_t ret = ::read(fds[read_fd], buf, count);
		if (ret < 0) switch (errno)
		{
		case EINTR:
			continue; // normal... try again
		case EAGAIN:
			{
				fd_set rset;
				FD_ZERO(&rset);
				FD_SET(fds[read_fd], &rset);
				timeval timeout = { 60, 0 }; // 1 min
				int ret = select(fds[read_fd] + 1, &rset, nullptr, nullptr, &timeout);
				if (ret < 0) // error
					throw std::runtime_error(strerror(errno));
				if (ret == 0)
					return 0; // timeout... no data
			}
			break;
		default:
			throw std::runtime_error(strerror(errno));
		}
		else
			return ret;
	}
}

struct mbrola_synthesizer : public tts::synthesizer
{
	mbrola_synthesizer(const char *database);

	~mbrola_synthesizer();

	int channels() const { return 1; }

	int frequency() const { return sample_rate; }

	const rdf::uri &format() const { return sample_format; }
private:
	void flush();

	pid_t pid;
	FILE *pho;

	int sample_rate;
	rdf::uri sample_format;
};

mbrola_synthesizer::mbrola_synthesizer(const char *database)
	: pho(nullptr)
	, sample_rate(0)
	, sample_format(rdf::tts("s16le"))
{
	pipe_t input;
	pipe_t audio;
	pipe_t error;

	pid = fork();
	if (pid == -1) throw std::runtime_error(strerror(errno));
	if (pid == 0)
	{
		input.dup(pipe_t::read_fd, STDIN_FILENO);
		audio.dup(pipe_t::write_fd, STDOUT_FILENO);
		error.dup(pipe_t::write_fd, STDERR_FILENO);

		execlp("mbrola", "mbrola",
		       "-e",     // ignore fatal errors on unknown diphones
		       database, // voice file database
		       "-",      // pho file input (stdin)
		       "-.wav",  // audio output (stdout)
                       nullptr);
		_exit(1);
	}

	input.close(pipe_t::read_fd);
	audio.close(pipe_t::write_fd);
	error.close(pipe_t::write_fd);

	input.set_flags(pipe_t::write_fd, O_NONBLOCK);
	audio.set_flags(pipe_t::read_fd,  O_NONBLOCK);
	error.set_flags(pipe_t::read_fd,  O_NONBLOCK);

	pho = input.open("w");

	flush();
	uint8_t header[44];
	if (audio.read(header, sizeof(header)) != sizeof(header))
		throw std::runtime_error("mbrola did not return a WAV header.");

	if (memcmp(header, "RIFF", 4) != 0 || memcmp(header + 8, "WAVEfmt ", 8) != 0)
		throw std::runtime_error("mbrola did not return a WAV header.");

	sample_rate = header[24] + (header[25] << 8) + (header[26] << 16) + (header[27] << 24);
}

mbrola_synthesizer::~mbrola_synthesizer()
{
	fclose(pho);
}

void mbrola_synthesizer::flush()
{
	fputs("#\n", pho);
	fflush(pho);
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

		fprintf(stdout, "channels    : %d\n", voice->channels());
		fprintf(stdout, "format      : %s\n", voice->format().str().c_str());
		fprintf(stdout, "sample rate : %d\n", voice->frequency());
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
