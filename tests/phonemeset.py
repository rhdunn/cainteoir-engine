#!/usr/bin/python

# Copyright (C) 2013 Reece H. Dunn
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
	test.run({'name': 'IPA-Based Phoneme Set', 'groups': [
		{'name': 'ASCII-IPA', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/ascii-ipa.features', 'result': 'phonemeset/ascii-ipa.txt', 'from': 'features', 'to': 'ascii-ipa'},
			{'test': 'phonemeset/ascii-ipa.txt', 'result': 'phonemeset/ascii-ipa.features', 'from': 'ascii-ipa', 'to': 'features'},
		]},
		{'name': 'IPA', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/ipa.features', 'result': 'phonemeset/ipa.txt', 'from': 'features', 'to': 'ipa'},
			{'test': 'phonemeset/ipa.txt', 'result': 'phonemeset/ipa.features', 'from': 'ipa', 'to': 'features'},
		]},
		{'name': 'X-SAMPA', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/x-sampa.features', 'result': 'phonemeset/x-sampa.txt', 'from': 'features', 'to': 'x-sampa'},
			{'test': 'phonemeset/x-sampa.txt', 'result': 'phonemeset/x-sampa.features', 'from': 'x-sampa', 'to': 'features'},
		]},
		{'name': 'Conlang X-SAMPA', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/x-sampa.features', 'result': 'phonemeset/cxs.txt', 'from': 'features', 'to': 'cxs'},
			{'test': 'phonemeset/cxs.txt', 'result': 'phonemeset/x-sampa.features', 'from': 'cxs', 'to': 'features'},
		]},
	]})
	test.run({'name': 'English', 'groups': [
		{'name': 'Arpabet', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/en/ipa.txt', 'result': 'phonemeset/en/arpabet.txt', 'from': 'ipa', 'to': 'arpabet'},
			{'test': 'phonemeset/en/arpabet.txt', 'result': 'phonemeset/en/arpabet.ipa', 'from': 'arpabet', 'to': 'ipa'},
		]},
		{'name': 'SAMPA (en)', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/en/ipa.txt', 'result': 'phonemeset/en/sampa.en.txt', 'from': 'ipa', 'to': 'sampa/en'},
		]},
		{'name': 'SAMPA (en-US)', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/en/ipa.txt', 'result': 'phonemeset/en/sampa.en-US.txt', 'from': 'ipa', 'to': 'sampa/en-US'},
		]},
		{'name': 'eSpeak (en)', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/en/ipa.txt', 'result': 'phonemeset/en/espeak.en.txt', 'from': 'ipa', 'to': 'espeak/en'},
		]},
	]})
	test.run({'name': 'SAMPA-Based Phoneme Set', 'groups': [
		{'name': 'English', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/sampa/en.features', 'result': 'phonemeset/sampa/en.txt', 'from': 'features', 'to': 'sampa/en'},
			{'test': 'phonemeset/sampa/en.txt', 'result': 'phonemeset/sampa/en.features', 'from': 'sampa/en', 'to': 'features'},
		]},
		{'name': 'US English', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/sampa/en-US.features', 'result': 'phonemeset/sampa/en-US.txt', 'from': 'features', 'to': 'sampa/en-US'},
			{'test': 'phonemeset/sampa/en-US.txt', 'result': 'phonemeset/sampa/en-US.features', 'from': 'sampa/en-US', 'to': 'features'},
		]},
	]})
	test.summary()
