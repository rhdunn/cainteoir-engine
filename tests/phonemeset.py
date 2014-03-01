#!/usr/bin/python

# Copyright (C) 2013-2014 Reece H. Dunn
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
	test = harness.TestSuite('phonemeset', sys.argv)
	test.run({'name': 'Feature-Based', 'groups': [
		{'name': 'Features', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/phonemes.features', 'result': 'phonemeset/phonemes.features', 'from': 'features', 'to': 'features'},
		]},
	]})
	test.run({'name': 'IPA-Based', 'groups': [
		{'name': 'IPA', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/phonemes.features', 'result': 'phonemeset/phonemes.ipa', 'from': 'features', 'to': 'ipa'},
			{'test': 'phonemeset/phonemes.ipa', 'result': 'phonemeset/phonemes.features', 'from': 'ipa', 'to': 'features'},
		]},
	]})
	test.summary()
