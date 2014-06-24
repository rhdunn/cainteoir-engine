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
	test = harness.TestSuite('diphones', sys.argv)
	test.run({'name': 'Diphones', 'groups': [
		{'name': 'Duration', 'type': ['diphones', 'diphones-after-reset'], 'tests': [
			{'test': 'diphone/duration/no-pauses.pho', 'result': 'diphone/duration/no-pauses.dipho', 'phonemeset': 'radio'},
			{'test': 'diphone/duration/pause-before.pho', 'result': 'diphone/duration/pause-before.dipho', 'phonemeset': 'radio'},
			{'test': 'diphone/duration/pause-after.pho', 'result': 'diphone/duration/pause-after.dipho', 'phonemeset': 'radio'},
		]},
		{'name': 'Intonation (at 50%)', 'type': ['diphones', 'diphones-after-reset'], 'tests': [
			{'test': 'diphone/intonation/between-consonants-at-50-percent.pho', 'result': 'diphone/intonation/between-consonants-at-50-percent.dipho', 'phonemeset': 'radio'},
			{'test': 'diphone/intonation/multiple-vowels-at-50-percent.pho', 'result': 'diphone/intonation/multiple-vowels-at-50-percent.dipho', 'phonemeset': 'radio'},
			{'test': 'diphone/intonation/multiple-vowels-mismatched-start-end-points.pho', 'result': 'diphone/intonation/multiple-vowels-mismatched-start-end-points.dipho', 'phonemeset': 'radio'},
		]},
		{'name': 'Intonation (between 50%)', 'type': ['diphones', 'diphones-after-reset'], 'tests': [
			{'test': 'diphone/intonation/between-consonants-between-50-percent-flat.pho', 'result': 'diphone/intonation/between-consonants-between-50-percent-flat.dipho', 'phonemeset': 'radio'},
			{'test': 'diphone/intonation/between-consonants-between-50-percent-linear.pho', 'result': 'diphone/intonation/between-consonants-between-50-percent-linear.dipho', 'phonemeset': 'radio'},
		]},
	]})
	test.summary()
