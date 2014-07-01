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
	test = harness.TestSuite('prosody', sys.argv)
	test.run({'name': 'Prosody', 'phonemeset': 'cxs', 'dictionary': 'dict/cainteoir/words.dict', 'groups': [
		{'name': 'Phoneme Grouping', 'type': 'prosody', 'fixed-duration': '80ms', 'tests': [
			{'test': 'prosody/grouping/simple-phonemes.txt', 'result': 'prosody/grouping/simple-phonemes.pho'},
			{'test': 'prosody/grouping/multiple-words.txt', 'result': 'prosody/grouping/multiple-words.pho'},
			{'test': 'prosody/grouping/affricates.txt', 'result': 'prosody/grouping/affricates.pho'},
			{'test': 'prosody/grouping/lexical-sets.txt', 'result': 'prosody/grouping/lexical-sets.pho'},
		]},
		{'name': 'Duration Model', 'type': 'prosody', 'tests': [
			{'test': 'prosody/grouping/lexical-sets.txt', 'result': 'prosody/grouping/lexical-sets-duration-model.pho', 'duration-model': '../data/durations/en/default.dur'},
		]},
	]})
	test.summary()
