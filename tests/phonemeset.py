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
			{'test': 'phonemeset/ipa/disjoint.features', 'result': 'phonemeset/ipa/disjoint.features', 'from': 'features', 'to': 'features', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/pauses.features', 'result': 'phonemeset/ipa/pauses.features', 'from': 'features', 'to': 'features'},
		]},
	]})
	test.run({'name': 'IPA-Based', 'groups': [
		{'name': 'IPA', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/ipa/phonemes.features', 'result': 'phonemeset/ipa/phonemes.ipa', 'from': 'features', 'to': 'ipa', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/phonemes.ipa', 'result': 'phonemeset/ipa/phonemes.features', 'from': 'ipa', 'to': 'features', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/disjoint.features', 'result': 'phonemeset/ipa/disjoint.ipa', 'from': 'features', 'to': 'ipa', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/disjoint.ipa', 'result': 'phonemeset/ipa/disjoint.features', 'from': 'ipa', 'to': 'features', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/pauses.features', 'result': 'phonemeset/ipa/pauses.ipa', 'from': 'features', 'to': 'ipa'},
			{'test': 'phonemeset/ipa/pauses.ipa', 'result': 'phonemeset/ipa/pauses.features', 'from': 'ipa', 'to': 'features'},
			{'test': 'phonemeset/ipa/ipa-aliases.ipa', 'result': 'phonemeset/ipa/ipa-aliases.default', 'from': 'ipa', 'to': 'ipa', 'args': ['--separate', '--no-pauses']},
		]},
		{'name': 'X-SAMPA', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/ipa/phonemes.features', 'result': 'phonemeset/ipa/phonemes.x-sampa', 'from': 'features', 'to': 'x-sampa', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/phonemes.x-sampa', 'result': 'phonemeset/ipa/phonemes.features', 'from': 'x-sampa', 'to': 'features', 'args': ['--separate', '--no-pauses']},
			# NOTE: X-SAMPA has ambiguous transcriptions for diacritics vs ties, so these fail to round-trip
			{'test': 'phonemeset/ipa/disjoint.features', 'result': 'phonemeset/ipa/disjoint.x-sampa', 'from': 'features', 'to': 'x-sampa', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/disjoint.x-sampa', 'result': 'phonemeset/ipa/disjoint.x-sampa-features', 'from': 'x-sampa', 'to': 'features', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/pauses.features', 'result': 'phonemeset/ipa/pauses.x-sampa', 'from': 'features', 'to': 'x-sampa'},
			{'test': 'phonemeset/ipa/pauses.x-sampa', 'result': 'phonemeset/ipa/pauses.features', 'from': 'x-sampa', 'to': 'features'},
			{'test': 'phonemeset/ipa/x-sampa-aliases.x-sampa', 'result': 'phonemeset/ipa/x-sampa-aliases.default', 'from': 'x-sampa', 'to': 'x-sampa', 'args': ['--separate', '--no-pauses']},
		]},
		{'name': 'CXS', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/ipa/phonemes.features', 'result': 'phonemeset/ipa/phonemes.cxs', 'from': 'features', 'to': 'cxs', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/phonemes.cxs', 'result': 'phonemeset/ipa/phonemes.features', 'from': 'cxs', 'to': 'features', 'args': ['--separate', '--no-pauses']},
			# NOTE: X-SAMPA has ambiguous transcriptions for diacritics vs ties, so these fail to round-trip
			{'test': 'phonemeset/ipa/disjoint.features', 'result': 'phonemeset/ipa/disjoint.cxs', 'from': 'features', 'to': 'cxs', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/disjoint.cxs', 'result': 'phonemeset/ipa/disjoint.cxs-features', 'from': 'cxs', 'to': 'features', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/pauses.features', 'result': 'phonemeset/ipa/pauses.cxs', 'from': 'features', 'to': 'cxs'},
			{'test': 'phonemeset/ipa/pauses.cxs', 'result': 'phonemeset/ipa/pauses.features', 'from': 'cxs', 'to': 'features'},
		]},
		{'name': 'ASCII-IPA', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/ipa/phonemes.features', 'result': 'phonemeset/ipa/phonemes.ascii-ipa', 'from': 'features', 'to': 'ascii-ipa', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/phonemes.ascii-ipa', 'result': 'phonemeset/ipa/phonemes.features', 'from': 'ascii-ipa', 'to': 'features', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/disjoint.features', 'result': 'phonemeset/ipa/disjoint.ascii-ipa', 'from': 'features', 'to': 'ascii-ipa', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/disjoint.ascii-ipa', 'result': 'phonemeset/ipa/disjoint.ascii-ipa-features', 'from': 'ascii-ipa', 'to': 'features', 'args': ['--separate', '--no-pauses']},
			{'test': 'phonemeset/ipa/pauses.features', 'result': 'phonemeset/ipa/pauses.ascii-ipa', 'from': 'features', 'to': 'ascii-ipa'},
			{'test': 'phonemeset/ipa/pauses.ascii-ipa', 'result': 'phonemeset/ipa/pauses.features', 'from': 'ascii-ipa', 'to': 'features'},
			{'test': 'phonemeset/ipa/x-sampa-aliases.x-sampa', 'result': 'phonemeset/ipa/x-sampa-aliases.default', 'from': 'cxs', 'to': 'cxs', 'args': ['--separate', '--no-pauses']},
		]},
	]})
	test.run({'name': 'Arpabet-Based', 'groups': [
		{'name': 'CMU', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/arpabet/cmu-phonemes.ipa', 'result': 'phonemeset/arpabet/cmu-phonemes.cmu', 'from': 'ipa', 'to': 'cmu'},
			{'test': 'phonemeset/arpabet/cmu-phonemes.cmu', 'result': 'phonemeset/arpabet/cmu-phonemes.ipa', 'from': 'cmu', 'to': 'ipa'},
			{'test': 'phonemeset/arpabet/cmu-pauses.ipa', 'result': 'phonemeset/arpabet/cmu-pauses.cmu', 'from': 'ipa', 'to': 'cmu'},
			{'test': 'phonemeset/arpabet/cmu-pauses.cmu', 'result': 'phonemeset/arpabet/cmu-pauses.ipa', 'from': 'cmu', 'to': 'ipa'},
			{'test': 'phonemeset/arpabet/cmu-pauses-no-last-eol.cmu', 'result': 'phonemeset/arpabet/cmu-pauses.ipa', 'from': 'cmu', 'to': 'ipa'},
			# Here, the last phoneme is a complete phoneme and the start of a diphthong.
			# Therefore, the phoneme cannot be acted on until the next phoneme is read,
			# but there is no next phoneme.
			{'test': 'phonemeset/arpabet/cmu-ambiguous-last-phoneme.ipa', 'result': 'phonemeset/arpabet/cmu-ambiguous-last-phoneme.cmu', 'from': 'ipa', 'to': 'cmu'},
		]},
	]})
	test.run({'name': 'eSpeak-Based', 'groups': [
		{'name': 'English', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/espeak/en.ipa', 'result': 'phonemeset/espeak/en.espeak', 'from': 'ipa', 'to': 'espeak/en'},
			{'test': 'phonemeset/espeak/en.espeak', 'result': 'phonemeset/espeak/en.ipa', 'from': 'espeak/en', 'to': 'ipa'},
			{'test': 'phonemeset/espeak/en~aliases.espeak', 'result': 'phonemeset/espeak/en~aliases.ipa', 'from': 'espeak/en', 'to': 'ipa'},
		]},
		{'name': 'English (Brittish)', 'type': 'phonemeset', 'tests': [
			{'test': 'phonemeset/espeak/en.ipa', 'result': 'phonemeset/espeak/en.espeak', 'from': 'ipa', 'to': 'espeak/en-GB'},
			{'test': 'phonemeset/espeak/en.espeak', 'result': 'phonemeset/espeak/en.ipa', 'from': 'espeak/en-GB', 'to': 'ipa'},
			{'test': 'phonemeset/espeak/en~aliases.espeak', 'result': 'phonemeset/espeak/en~aliases.ipa', 'from': 'espeak/en-GB', 'to': 'ipa'},
		]},
	]})
	test.summary()
