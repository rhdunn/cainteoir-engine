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
	test = harness.TestSuite('phoneme-stress', sys.argv)
	test.run({'name': 'Syllabification', 'type': 'phonemes', 'args': ['--syllables'], 'from': 'cxs', 'to': 'cxs', 'tests': [
		{'test': 'phoneme_stress/syllable-stressed.cxs', 'result': 'phoneme_stress/syllable-stressed.syl'},
		{'test': 'phoneme_stress/syllable-stressed-cv-nosep.cxs', 'result': 'phoneme_stress/syllable-stressed-cv.syl'},
		{'test': 'phoneme_stress/syllable-stressed-cvc-nosep.cxs', 'result': 'phoneme_stress/syllable-stressed-cvc.syl'},
		{'test': 'phoneme_stress/syllable-stressed-cvc-afr-nosep.cxs', 'result': 'phoneme_stress/syllable-stressed-cvc-afr.syl'},
		{'test': 'phoneme_stress/syllable-stressed-cvcc-nosep.cxs', 'result': 'phoneme_stress/syllable-stressed-cvcc.syl'},
		{'test': 'phoneme_stress/vowel-stressed.cxs', 'result': 'phoneme_stress/vowel-stressed.syl'},
		{'test': 'phoneme_stress/vowel-stressed-cv-nosep.cxs', 'result': 'phoneme_stress/vowel-stressed-cv.syl'},
		{'test': 'phoneme_stress/vowel-stressed-cvc-nosep.cxs', 'result': 'phoneme_stress/vowel-stressed-cvc.syl'},
		{'test': 'phoneme_stress/vowel-stressed-cvc-afr-nosep.cxs', 'result': 'phoneme_stress/vowel-stressed-cvc-afr.syl'},
		{'test': 'phoneme_stress/vowel-stressed-cvcc-nosep.cxs', 'result': 'phoneme_stress/vowel-stressed-cvcc.syl'},
	]})
	test.run({'name': 'As Transcribed', 'type': 'phonemes', 'args': [], 'from': 'cxs', 'to': 'cxs', 'tests': [
		{'test': 'phoneme_stress/syllable-stressed.cxs', 'result': 'phoneme_stress/syllable-stressed.cxs'},
		{'test': 'phoneme_stress/syllable-stressed-cv-nosep.cxs', 'result': 'phoneme_stress/syllable-stressed-cv-nosep.cxs'},
		{'test': 'phoneme_stress/syllable-stressed-cvc-nosep.cxs', 'result': 'phoneme_stress/syllable-stressed-cvc-nosep.cxs'},
		{'test': 'phoneme_stress/syllable-stressed-cvc-afr-nosep.cxs', 'result': 'phoneme_stress/syllable-stressed-cvc-afr-nosep.cxs'},
		{'test': 'phoneme_stress/syllable-stressed-cvcc-nosep.cxs', 'result': 'phoneme_stress/syllable-stressed-cvcc-nosep.cxs'},
		{'test': 'phoneme_stress/vowel-stressed.cxs', 'result': 'phoneme_stress/vowel-stressed.cxs'},
		{'test': 'phoneme_stress/vowel-stressed-cv-nosep.cxs', 'result': 'phoneme_stress/vowel-stressed-cv-nosep.cxs'},
		{'test': 'phoneme_stress/vowel-stressed-cvc-nosep.cxs', 'result': 'phoneme_stress/vowel-stressed-cvc-nosep.cxs'},
		{'test': 'phoneme_stress/vowel-stressed-cvc-afr-nosep.cxs', 'result': 'phoneme_stress/vowel-stressed-cvc-afr-nosep.cxs'},
		{'test': 'phoneme_stress/vowel-stressed-cvcc-nosep.cxs', 'result': 'phoneme_stress/vowel-stressed-cvcc-nosep.cxs'},
	]})
	test.run({'name': 'Vowel Stress', 'type': 'phonemes', 'args': ['--vowel-stress'], 'from': 'cxs', 'to': 'cxs', 'tests': [
		{'test': 'phoneme_stress/syllable-stressed.cxs', 'result': 'phoneme_stress/vowel-stressed.cxs'},
		{'test': 'phoneme_stress/syllable-stressed-cv-nosep.cxs', 'result': 'phoneme_stress/vowel-stressed-cv-nosep.cxs'},
		{'test': 'phoneme_stress/syllable-stressed-cvc-nosep.cxs', 'result': 'phoneme_stress/vowel-stressed-cvc-nosep.cxs'},
		{'test': 'phoneme_stress/syllable-stressed-cvc-afr-nosep.cxs', 'result': 'phoneme_stress/vowel-stressed-cvc-afr-nosep.cxs'},
		{'test': 'phoneme_stress/syllable-stressed-cvcc-nosep.cxs', 'result': 'phoneme_stress/vowel-stressed-cvcc-nosep.cxs'},
		{'test': 'phoneme_stress/vowel-stressed.cxs', 'result': 'phoneme_stress/vowel-stressed.cxs'},
		{'test': 'phoneme_stress/vowel-stressed-cv-nosep.cxs', 'result': 'phoneme_stress/vowel-stressed-cv-nosep.cxs'},
		{'test': 'phoneme_stress/vowel-stressed-cvc-nosep.cxs', 'result': 'phoneme_stress/vowel-stressed-cvc-nosep.cxs'},
		{'test': 'phoneme_stress/vowel-stressed-cvc-afr-nosep.cxs', 'result': 'phoneme_stress/vowel-stressed-cvc-afr-nosep.cxs'},
		{'test': 'phoneme_stress/vowel-stressed-cvcc-nosep.cxs', 'result': 'phoneme_stress/vowel-stressed-cvcc-nosep.cxs'},
	]})
	test.run({'name': 'Syllable Stress', 'type': 'phonemes', 'args': ['--syllable-stress'], 'from': 'cxs', 'to': 'cxs', 'tests': [
		{'test': 'phoneme_stress/syllable-stressed.cxs', 'result': 'phoneme_stress/syllable-stressed.cxs'},
		{'test': 'phoneme_stress/syllable-stressed-cv-nosep.cxs', 'result': 'phoneme_stress/syllable-stressed-cv-nosep.cxs'},
		{'test': 'phoneme_stress/syllable-stressed-cvc-nosep.cxs', 'result': 'phoneme_stress/syllable-stressed-cvc-nosep.cxs'},
		{'test': 'phoneme_stress/syllable-stressed-cvc-afr-nosep.cxs', 'result': 'phoneme_stress/syllable-stressed-cvc-afr-nosep.cxs'},
		{'test': 'phoneme_stress/syllable-stressed-cvcc-nosep.cxs', 'result': 'phoneme_stress/syllable-stressed-cvcc-nosep.cxs'},
		{'test': 'phoneme_stress/vowel-stressed.cxs', 'result': 'phoneme_stress/syllable-stressed.cxs'},
		{'test': 'phoneme_stress/vowel-stressed-cv-nosep.cxs', 'result': 'phoneme_stress/syllable-stressed-cv-nosep.cxs'},
		{'test': 'phoneme_stress/vowel-stressed-cvc-nosep.cxs', 'result': 'phoneme_stress/syllable-stressed-cvc-nosep.cxs'},
		{'test': 'phoneme_stress/vowel-stressed-cvc-afr-nosep.cxs', 'result': 'phoneme_stress/syllable-stressed-cvc-afr-nosep.cxs'},
		{'test': 'phoneme_stress/vowel-stressed-cvcc-nosep.cxs', 'result': 'phoneme_stress/syllable-stressed-cvcc-nosep.cxs'},
	]})
	test.summary()
