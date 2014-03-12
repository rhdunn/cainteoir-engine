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
			{'test': 'phonemeset/ipa/phonemes.features', 'result': 'phonemeset/ipa/phonemes.features', 'from': 'features', 'to': 'features', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/tied-phonemes.features', 'result': 'phonemeset/ipa/tied-phonemes.features', 'from': 'features', 'to': 'features', 'args': ['--separate', '--no-pauses']},
		]},
	]})
	test.run({'name': 'IPA-Based', 'groups': [
		{'name': 'IPA', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/ipa/phonemes.features', 'result': 'phonemeset/ipa/phonemes.ipa', 'from': 'features', 'to': 'ipa', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/phonemes.ipa', 'result': 'phonemeset/ipa/phonemes.features', 'from': 'ipa', 'to': 'features', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/tied-phonemes.features', 'result': 'phonemeset/ipa/tied-phonemes.ipa', 'from': 'features', 'to': 'ipa', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/tied-phonemes.ipa', 'result': 'phonemeset/ipa/tied-phonemes.features', 'from': 'ipa', 'to': 'features', 'args': ['--separate', '--no-pauses']},
		]},
		{'name': 'X-SAMPA', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/ipa/phonemes.features', 'result': 'phonemeset/ipa/phonemes.x-sampa', 'from': 'features', 'to': 'x-sampa', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/phonemes.x-sampa', 'result': 'phonemeset/ipa/phonemes.features', 'from': 'x-sampa', 'to': 'features', 'args': ['--separate', '--no-pauses']},
			# NOTE: X-SAMPA has ambiguous transcriptions for diacritics vs ties, so these fail to round-trip
			{'test': 'phonemeset/ipa/tied-phonemes.features', 'result': 'phonemeset/ipa/tied-phonemes.x-sampa', 'from': 'features', 'to': 'x-sampa', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/tied-phonemes.x-sampa', 'result': 'phonemeset/ipa/tied-phonemes.x-sampa-features', 'from': 'x-sampa', 'to': 'features', 'args': ['--separate', '--no-pauses']},
		]},
		{'name': 'CXS', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/ipa/phonemes.features', 'result': 'phonemeset/ipa/phonemes.cxs', 'from': 'features', 'to': 'cxs', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/phonemes.cxs', 'result': 'phonemeset/ipa/phonemes.features', 'from': 'cxs', 'to': 'features', 'args': ['--separate', '--no-pauses']},
			# NOTE: X-SAMPA has ambiguous transcriptions for diacritics vs ties, so these fail to round-trip
			{'test': 'phonemeset/ipa/tied-phonemes.features', 'result': 'phonemeset/ipa/tied-phonemes.cxs', 'from': 'features', 'to': 'cxs', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/tied-phonemes.cxs', 'result': 'phonemeset/ipa/tied-phonemes.cxs-features', 'from': 'cxs', 'to': 'features', 'args': ['--separate', '--no-pauses']},
		]},
		{'name': 'ASCII-IPA', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/ipa/phonemes.features', 'result': 'phonemeset/ipa/phonemes.ascii-ipa', 'from': 'features', 'to': 'ascii-ipa', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/phonemes.ascii-ipa', 'result': 'phonemeset/ipa/phonemes.features', 'from': 'ascii-ipa', 'to': 'features', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/tied-phonemes.features', 'result': 'phonemeset/ipa/tied-phonemes.ascii-ipa', 'from': 'features', 'to': 'ascii-ipa', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/tied-phonemes.ascii-ipa', 'result': 'phonemeset/ipa/tied-phonemes.features', 'from': 'ascii-ipa', 'to': 'features', 'args': ['--separate', '--no-pauses']},
		]},
	]})
	test.run({'name': 'Arpabet-Based', 'groups': [
		{'name': 'CMU', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/arpabet/cmu-phonemes.ipa', 'result': 'phonemeset/arpabet/cmu-phonemes.cmu', 'from': 'ipa', 'to': 'cmu'},
			{'test': 'phonemeset/arpabet/cmu-phonemes.cmu', 'result': 'phonemeset/arpabet/cmu-phonemes.ipa', 'from': 'cmu', 'to': 'ipa'},
		]},
	]})
	test.summary()
