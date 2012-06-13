/* Test for extracting words, numbers and other entries from a document.
 *
 * Copyright (C) 2012 Reece H. Dunn
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

#include <cainteoir/document.hpp>
#include <stdexcept>
#include <cstdio>

namespace rdf    = cainteoir::rdf;
namespace events = cainteoir::events;

enum text_block_t
{
	word,
	number,
	punctuation,
	symbol,
	skip,
	error,
};

#define S    1 // Skip
#define P    2 // Punctuation
#define Y    3 // Symbol
#define W    4 // Word
#define N    5 // Number
#define E    6 // Error
#define N0   7 // Number (State 0)
#define _  255 // end of state machine

// 0 : Start
const uint8_t state0_transitions[256] = {
	//       00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F
	/* 0x */ S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,
	/* 1x */ S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,
	/* 2x */ S,  P,  P,  P,  Y,  P,  P,  W,  P,  P,  P,  Y,  P,  P,  P,  P,
	/* 3x */ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  P,  P,  Y,  Y,  Y,  P,
	/* 4x */ P,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,
	/* 5x */ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  P,  P,  P,  Y,  P,
	/* 6x */ Y,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,
	/* 7x */ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  P,  Y,  P,  Y,  S,
	/* 8x */ E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,
	/* 9x */ E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,
	/* Ax */ E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,
	/* Bx */ E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,
	/* Cx */ E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,
	/* Dx */ E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,
	/* Ex */ E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,
	/* Fx */ E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,
};

// 1 : Skip
const uint8_t state1_transitions[256] = {
	//       00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F
	/* 0x */ S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,
	/* 1x */ S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,
	/* 2x */ S,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 3x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 4x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 5x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 6x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 7x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  S,
	/* 8x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 9x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Ax */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Bx */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Cx */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Dx */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Ex */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Fx */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
};

// 2 : Punctuation
const uint8_t state2_transitions[256] = {
	//       00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F
	/* 0x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 1x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 2x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 3x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 4x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 5x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 6x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 7x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 8x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 9x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Ax */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Bx */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Cx */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Dx */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Ex */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Fx */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
};

// 3 : Symbol
const uint8_t state3_transitions[256] = {
	//       00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F
	/* 0x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 1x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 2x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 3x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 4x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 5x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 6x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 7x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 8x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 9x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Ax */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Bx */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Cx */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Dx */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Ex */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Fx */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
};

// 4 : Word
const uint8_t state4_transitions[256] = {
	//       00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F
	/* 0x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 1x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 2x */ _,  _,  _,  _,  _,  _,  _,  W,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 3x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 4x */ _,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,
	/* 5x */ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  _,  _,  _,  _,  _,
	/* 6x */ _,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,
	/* 7x */ W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  W,  _,  _,  _,  _,  _,
	/* 8x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 9x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Ax */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Bx */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Cx */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Dx */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Ex */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Fx */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
};

// 5 : Number
const uint8_t state5_transitions[256] = {
	//       00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F
	/* 0x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 1x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 2x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  N0, _,  _,  _,
	/* 3x */ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  _,  _,  _,  _,  _,  _,
	/* 4x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 5x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 6x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 7x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 8x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 9x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Ax */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Bx */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Cx */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Dx */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Ex */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Fx */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
};

// 6 : Error
const uint8_t state6_transitions[256] = {
	//       00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F
	/* 0x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 1x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 2x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 3x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 4x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 5x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 6x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 7x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 8x */ E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,
	/* 9x */ E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,
	/* Ax */ E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,
	/* Bx */ E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,
	/* Cx */ E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,
	/* Dx */ E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,
	/* Ex */ E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,
	/* Fx */ E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,  E,
};

// 7 : Number (State 0)
const uint8_t state7_transitions[256] = {
	//       00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F
	/* 0x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 1x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 2x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 3x */ N,  N,  N,  N,  N,  N,  N,  N,  N,  N,  _,  _,  _,  _,  _,  _,
	/* 4x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 5x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 6x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 7x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 8x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* 9x */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Ax */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Bx */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Cx */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Dx */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Ex */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
	/* Fx */ _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,  _,
};

const uint8_t *state_transitions[8] = {
	state0_transitions,
	state1_transitions,
	state2_transitions,
	state3_transitions,
	state4_transitions,
	state5_transitions,
	state6_transitions,
	state7_transitions,
};

const int state_value[] =
{
	-1, // Start
	skip,
	punctuation,
	symbol,
	word,
	number,
	error,
	-1, // Number (State 0)
};

class text_reader
{
public:
	text_reader();

	void set_buffer(const std::shared_ptr<cainteoir::buffer> &aText);

	bool read();
private:
	std::shared_ptr<cainteoir::buffer> data;
	const uint8_t *begin;
	const uint8_t *end;
};

text_reader::text_reader()
	: begin(nullptr)
	, end(nullptr)
{
}

void text_reader::set_buffer(const std::shared_ptr<cainteoir::buffer> &aText)
{
	data  = aText;
	begin = (const uint8_t *)aText->begin();
	end   = (const uint8_t *)aText->end();
}

bool text_reader::read()
{
	uint8_t state = 0;
	int match_value = -1;
	const uint8_t *match_first = begin;
	const uint8_t *match_last  = begin;

	while (begin != end && state != 255)
	{
		state = state_transitions[state][*begin++];

		int value = state_value[state];
		if (value != -1) // terminal node
		{
			match_last  = begin;
			match_value = value;
		}
	}

	begin = match_last;

	cainteoir::buffer match((const char *)match_first, (const char *)match_last);
	const char *type_name = nullptr;
	switch (match_value)
	{
	case skip:        return true;
	case symbol:      type_name = ".symbol        "; break;
	case punctuation: type_name = ".punctuation   "; break;
	case word:        type_name = ".word          "; break;
	case number:      type_name = ".number        "; break;
	case error:       type_name = ".error         "; break;
	case -1:          return false;
	}

	fputs(type_name, stdout);
	fwrite(match.begin(), 1, match.size(), stdout);
	if (match_value == error)
	{
		fputc('\t', stdout);
		for (auto first = match.begin(), last = match.end(); first != last; ++first)
			fprintf(stdout, "\\x%02X", (*first & 0xFF));
	}
	fputc('\n', stdout);

	return true;
}

int main(int argc, char ** argv)
{
	try
	{
		argc -= 1;
		argv += 1;

		if (argc == 0)
			throw std::runtime_error("no document specified");

		rdf::graph metadata;
		auto reader = cainteoir::createDocumentReader(argv[0], metadata, std::string());
		if (!reader)
		{
			fprintf(stderr, "unsupported document format for file \"%s\"\n", argv[0]);
			return 0;
		}

		text_reader text;
		while (reader->read())
		{
			if (reader->type & cainteoir::events::text)
			{
				text.set_buffer(reader->text);
				while (text.read())
				{
				}
			}
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
