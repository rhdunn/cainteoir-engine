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
	def accent(language, locale, name, consonants=None):
		return {'name': name.replace('... ', ''), 'type': 'phonemeset', 'tests': [
			{'test': 'accent/{0}/consonants.ipa'.format(language),
			 'result': 'accent/{0}/consonants.{1}'.format(language, consonants if consonants else locale),
			 'from': 'ipa',
			 'to': 'ipa',
			 'accent': locale},
			{'test': 'accent/{0}/phonemes.ipa'.format(language),
			 'result': 'accent/{0}/phonemes.{1}'.format(language, locale),
			 'from': 'ipa',
			 'to': 'ipa',
			 'accent': locale},
		]}

	test = harness.TestSuite('accent', sys.argv)

	test.run({'name': 'Edge Cases', 'groups': [
		{'name': 'Phoneme-to-Phoneme', 'type': 'phonemeset', 'tests': [
			{'test': 'accent/match-at-end.ipa', 'result': 'accent/match-at-end.cmu', 'from': 'ipa', 'to': 'ipa', 'accent': 'cmu'},
		]},
	]})

	test.run({'name': 'Dictionary', 'groups': [
		{'name': 'CMU Pronunciation Dictionary (General American)', 'type': 'phonemeset', 'tests': [
			{'test': 'accent/english/consonants.ipa', 'result': 'accent/english/consonants.en', 'from': 'ipa', 'to': 'ipa', 'accent': 'cmu'},
			{'test': 'accent/english/phonemes.ipa', 'result': 'accent/english/phonemes.cmu', 'from': 'ipa', 'to': 'ipa', 'accent': 'cmu'},
		]},
	]})

	en = {'name': 'English', 'groups': []} #                   English
	#                                                          ... British English
	en['groups'].append(accent('english', 'en-GB-scotland',   '... ... Scottish English (ScE)', consonants='ipa'))
	#                                                          ... ... England
	#                                                          ... ... ... Northern England
	#                                                          ... ... ... ... North West England
	en['groups'].append(accent('english', 'en-GB-x-lancastr', '... ... ... ... ... Lancastrian [Northern Lancashire]', consonants='en'))
	#                                                          ... ... ... Midlands
	#                                                          ... ... ... ... West Midlands
	en['groups'].append(accent('english', 'en-GB-x-brummie',  '... ... ... ... ... Brummie [Birmingham]', consonants='en'))
	#                                                          ... ... ... Southern England
	#                                                          ... ... ... ... South East England
	en['groups'].append(accent('english', 'en-GB-x-rp',       '... ... ... ... ... Archaic Received Pronunciation (RP)', consonants='en'))
	en['groups'].append(accent('english', 'en-US',            '... General American (GenAm) [North America]', consonants='en'))
	en['groups'].append(accent('english', 'en-029',           '... Caribbean'))
	test.run(en)

	test.summary()
