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
	test = harness.TestSuite('accent', sys.argv)
	test.run({'name': 'Accents', 'groups': [
		{'name': 'Phoneme-to-Phoneme', 'type': 'phonemeset', 'tests': [
			{'test': 'accent/match-at-end.ipa', 'result': 'accent/match-at-end.cmu', 'from': 'ipa', 'to': 'ipa', 'accent': 'cmu'},
		]},
		{'name': 'English', 'type': 'phonemeset', 'tests': [
			{'test': 'accent/english/phonemes.ipa', 'result': 'accent/english/phonemes.cmu', 'from': 'ipa', 'to': 'ipa', 'accent': 'cmu'},
		]},
	]})
	test.summary()
