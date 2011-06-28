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

def print_exception(word, pronunciation, ipa=True):
	if ipa:
		print '%-30s %s' % (word, pronunciation)
	else:
		replacements = [
			('l̩', '@L'),

			('aʊ', 'aU'),
			('əʊ', 'oU'),

			('.', ''),
			('ː', ':'),
			('ˈ', '\''),
			('ˌ', ','),
			('ɡ', 'g'),
			('ʃ', 'S'),
			('ʒ', 'Z'),
			('θ', 'T'),
			('ŋ', 'N'),
			('ɹ', 'r'),
			('ç', 'C'),
			('ʔ', '?'),
			('ɐ', 'a#'),
			('ɑ', 'A'),
			('ɒ', '0'),
			('ɜ', '3'),
			('ɛ', 'E'),
			('ʌ', 'V'),
			('ɔ', 'O'),
			('ə', '@'),
			('ɪ', 'I'),
			('ʊ', 'U'),
		]

		for a, b in replacements:
			pronunciation = pronunciation.replace(a, b)

		if '-' in word:
			print '(%s)%30s%s' % (word, ' ', pronunciation.replace('/', ''))
		else:
			print '%s%30s%s' % (word, ' ', pronunciation.replace('/', ''))

class Tester:
	def __init__(self):
		self.passed = 0
		self.failed = 0

	def test_dictionary(self, dictionaries, generate_exception_dictionary=False, ipa=True):
		data = []
		for dictionary in dictionaries:
			with open(dictionary) as f:
				for line in f:
					if line == '\n' or line.startswith('#'):
						pass
					elif '"' in line:
						word, pronunciation, rest = line.split('"')
						word = ' '.join(word.split())
						pronunciation = ' '.join(pronunciation.split())
						data.append({ 'word': word, 'replacement': pronunciation })
					else:
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
			if not 'pronunciation' in data[i].keys():
				result = [ x['pronunciation'] for x in data if x['word'] == data[i]['replacement'] ]
				if len(result) == 0:
					raise Exception('no pronunciation for "%s" found using "%s"' % (data[i]['word'], data[i]['replacement']))
				data[i]['pronunciation'] = result[0]

			word     = data[i]['word']
			expected = '/%s/' % data[i]['pronunciation']
			actual   = '/%s/' % espeak[i]

			expected = expected.replace('.', '')    # espeak does not support syllabic annotations, so ignore

			actual = actual.replace('əL',   'l̩')   # espeak --ipa does not map '@L' correctly, so use the syllabic form (different to 'əl')
			actual = actual.replace('ɪˈɑː', 'iˈɑː') # espeak does not differ in sound, but preserve the /i/ vs /ɪ/ distinction
			actual = actual.replace('ɪ/', 'i/') # espeak does not differ in sound, but preserve the /i/ vs /ɪ/ distinction
			actual = actual.replace('ai/', 'aɪ/') # ... but correct 'aɪ' usage
			actual = actual.replace('ei/', 'eɪ/') # ... and 'eɪ' usage
			actual = actual.replace('ɔi/', 'ɔɪ/') # ... and 'ɔɪ' usage
			actual = actual.replace('əʊi/', 'əʊɪ/') # ... and 'əʊɪ' usage

			if expected == actual:
				if not generate_exception_dictionary:
					print '%s %s ... pass' % (word, '/%s/' % data[i]['pronunciation'])
				self.passed = self.passed + 1
			else:
				if generate_exception_dictionary:
					print_exception(word, '/%s/' % data[i]['pronunciation'], ipa=ipa)
				else:
					print '%s %s got: %s ... fail' % (word, '/%s/' % data[i]['pronunciation'], actual)
				self.failed = self.failed + 1

	def summary(self):
		print '%d passed %d failed %d total' % (self.passed, self.failed, self.passed + self.failed)

def list_words(files):
	words = []
	for dictionary in files:
		with open(dictionary) as f:
			for line in f:
				if line != '\n' and not line.startswith('#'):
					word, pronunciation, rest = line.split('/')
					words.append((''.join(word.split()).lower()))
	return words

if __name__ == '__main__':
	args = [ x for x in sys.argv if not x.startswith('--') ]

	dictionarydir = args[1]
	dictionaries = [ os.path.join(dictionarydir, x) for x in os.listdir(dictionarydir) if x.endswith('.dict') ]

	if '--new-words' in sys.argv:
		have_words = list_words(dictionaries)
		with open(args[2]) as f:
			for line in f:
				line = line.replace('\n', '')
				if line.lower() not in have_words:
					print line
	else:
		test = Tester()
		test.test_dictionary(dictionaries, generate_exception_dictionary='--exception-dictionary' in sys.argv, ipa='--ipa' in sys.argv)
		if not '--exception-dictionary' in sys.argv:
			test.summary()
