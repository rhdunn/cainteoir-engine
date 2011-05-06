#!/usr/bin/python
# -*- coding: utf-8 -*-

# Copyright (C) 2011 Reece H. Dunn
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

import sys
import os

class Tester:
	def __init__(self):
		self.passed = 0
		self.failed = 0

	def test_dictionary(self, dictionary):
		data = []
		with open(os.path.join(sys.path[0], dictionary)) as f:
			for line in f:
				if line != '\n' and not line.startswith('#'):
					word, pronunciation, rest = line.split('/')
					word = ' '.join(word.split())
					pronunciation = ' '.join(pronunciation.split())
					data.append({ 'word': word, 'pronunciation': pronunciation })

		with open('/tmp/words.lst', 'w') as f:
			for item in data:
				f.write('%s\n\n' % item['word'])

		os.system('espeak -v en -xq --ipa -f /tmp/words.lst > /tmp/pronunciation.lst')

		with open('/tmp/pronunciation.lst') as f:
			espeak = [ ' '.join(x.split()) for x in f.read().split('\n') ]

		for i in range(0, len(data)):
			word     = data[i]['word']
			expected = '/%s/' % data[i]['pronunciation']
			actual   = '/%s/' % espeak[i]

			# espeak does not correctly generate the IPA for reduced vowels ...
			actual = actual.replace('əL', 'əl')

			if expected == actual or expected.replace('iː', 'ɪ') == actual:
				print '%s %s ... pass' % (word, expected)
				self.passed = self.passed + 1
			else:
				print '%s %s got: %s ... fail' % (word, expected, actual)
				self.failed = self.failed + 1

	def summary(self):
		print '%d passed %d failed %d total' % (self.passed, self.failed, self.passed + self.failed)

if __name__ == '__main__':
	test = Tester()
	test.test_dictionary('../data/dictionary/names.dict')
	test.test_dictionary('../data/dictionary/en/brands.dict')
	test.test_dictionary('../data/dictionary/en/places.dict')
	test.test_dictionary('../data/dictionary/en/words.dict')
	test.summary()
