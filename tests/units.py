#!/usr/bin/python

# Copyright (C) 2014 Reece H. Dunn
#
# This file is part of cainteoir-engine.
#
# cainteoir-engine is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# cainteoir-engine is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with cainteoir-engine.  If not, see <http://www.gnu.org/licenses/>.

import harness
import sys

if __name__ == '__main__':
	test = harness.TestSuite('units', sys.argv)
	test.run({'name': 'Units', 'phonemeset': 'cxs', 'groups': [
		{'name': 'Phoneme Mapping', 'type': 'units', 'fixed-duration': '100ms', 'tests': [
			{'test': 'units/simple.txt', 'result': 'units/simple.pho', 'voice': 'en1'},
			{'test': 'units/affricates.txt', 'result': 'units/affricates.pho', 'voice': 'en1'},
			{'test': 'units/diphthongs.txt', 'result': 'units/diphthongs.pho', 'voice': 'en1'},
			{'test': 'units/triphthongs.txt', 'result': 'units/triphthongs.pho', 'voice': 'en1'},
			{'test': 'units/long.txt', 'result': 'units/long.pho', 'voice': 'de1'},
			{'test': 'units/syllable-break.txt', 'result': 'units/syllable-break.pho', 'voice': 'en1'},
			{'test': 'units/stress.txt', 'result': 'units/stress.pho', 'voice': 'en1'},
			{'test': 'units/breaks.txt', 'result': 'units/breaks.pho', 'voice': 'en1'},
			{'test': 'units/unsupported.txt', 'result': 'units/unsupported.pho', 'voice': 'en1'},
		]},
		{'name': 'Unit Mapping', 'type': 'units', 'fixed-duration': '100ms', 'tests': [
			{'test': 'units/two-units.txt', 'result': 'units/two-units.pho', 'voice': 'en1'},
		]},
		{'name': 'Pitch', 'type': 'units', 'fixed-duration': '100ms', 'tests': [
			{'test': 'units/tone/level.txt', 'result': 'units/tone/level.pho', 'voice': 'en1'},
			{'test': 'units/tone/rising.txt', 'result': 'units/tone/rising.pho', 'voice': 'en1'},
			{'test': 'units/tone/falling.txt', 'result': 'units/tone/falling.pho', 'voice': 'en1'},
			{'test': 'units/tone/peaking.txt', 'result': 'units/tone/peaking.pho', 'voice': 'en1'},
			{'test': 'units/tone/dipping.txt', 'result': 'units/tone/dipping.pho', 'voice': 'en1'},
		]},
	]})
	test.summary()
