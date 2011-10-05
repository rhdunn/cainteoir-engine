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

			(' ', '||'),
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

		wordlist = []
		for i, c in enumerate(word):
			if c >= 'A' and c <= 'Z' and i != 0:
				wordlist.append(' ')
				wordlist.append(c.lower())
			else:
				wordlist.append(c)
		word = ''.join(wordlist)

		if word not in ['a', 'of']: # not a word which espeak annotates with properties
			if '-' in word or ' ' in word:
				print '(%s)%30s%s' % (word, ' ', pronunciation.replace('/', ''))
			else:
				print '%s%30s%s' % (word, ' ', pronunciation.replace('/', ''))

def lex_expression(expr):
	ret = []
	for c in expr:
		if c in '[]<>(|)':
			yield ''.join(ret)
			ret = []
			yield c
		else:
			ret.append(c)
	if len(ret) != 0:
		yield ''.join(ret)

EXPR_STRING     = 1 # abc
EXPR_IN_CHOICE  = 2 # (a|b|c)
EXPR_IN_ONEOF   = 3 # [abc]
EXPR_IN_REPLACE = 4 # <abc>

def expand(expr, refs):
	words = ['']
	state = EXPR_STRING
	fragment = None

	for token in lex_expression(expr):
		if state == EXPR_STRING:
			if token == '(':
				state = EXPR_IN_CHOICE
				fragment = []
			elif token == '[':
				state = EXPR_IN_ONEOF
			elif token == '<':
				state = EXPR_IN_REPLACE
			else:
				words = [word + token for word in words]
		elif state == EXPR_IN_CHOICE:
			if token == '|':
				pass
			elif token == ')':
				newwords = []
				for f in fragment:
					newwords.extend([word + f for word in words])
				words = newwords
				state = EXPR_STRING
				fragment = None
			else:
				fragment.append(token)
		elif state == EXPR_IN_ONEOF:
			if token == ']':
				newwords = []
				for f in fragment:
					newwords.extend([word + f for word in words])
				words = newwords
				state = EXPR_STRING
			else:
				fragment = token
		elif state == EXPR_IN_REPLACE:
			if token == '>':
				newwords = []
				for f in refs[fragment]:
					newwords.extend([word + f for word in words])
				words = newwords
				state = EXPR_STRING
			else:
				fragment = token

	return words

def parse_dictionaries(dictionaries):
	""" Dictionary format:
		# comment		-- an ignored line
		repl=expr		-- define the named replacement 'repl' as the word set 'expr'
		expr /phon/		-- the word set is the set of all word combinations in 'expr' pronounced using the phonemes 'phon'
		expr "say as"		-- the word set is the set of all word combinations in 'expr' pronounced the same as "say as"

		where expr is a combination of:
			text		-- use 'text' in the word
			(aa|bb|cc)	-- use 'aa' or 'bb' or 'cc' in the word
			[abc]		-- use the character 'a' or 'b' or 'c' in the word
			<repl>		-- use the result of the replacement 'repl' in the word
	"""
	data = {}
	refs = {}
	aliases = {}
	for dictionary in dictionaries:
		with open(dictionary) as f:
			for line in f:
				if line == '\n' or line.startswith('#'):
					pass
				elif '=' in line:
					ref = line.replace('\n', '').split('=')
					refs[ ref[0] ] = expand(ref[1], refs)
				elif '"' in line:
					word, pronunciation, rest = line.split('"')
					word = ' '.join(word.split())
					pronunciation = ' '.join(pronunciation.split())
					aliases[word] = pronunciation
				else:
					expr, pronunciation, rest = line.split('/')
					pronunciation = ' '.join(pronunciation.split())
					for word in expand(' '.join(expr.split()), refs):
						if word in data.keys() and data[word]['pronunciation'] != pronunciation:
							raise Exception('Mismatched pronunciation for duplicate word "%s"' % word)
						data[word] = { 'word': word, 'pronunciation': pronunciation }
	for expr, alias in aliases.items():
		pronunciation = []
		for subword in alias.replace('-', ' -').split():
			combine = subword.startswith('-')
			subword = subword.replace('-', '')

			if subword.startswith('\''): # retain stress (with)
				p = data[subword.replace('\'', '')]['pronunciation']
			elif subword.startswith(','): # invert stress (against)
				p = data[subword.replace(',', '')]['pronunciation'].replace('ˈ', ',').replace('ˌ', 'ˈ').replace(',', 'ˌ')
			else: # no stress
				p = data[subword]['pronunciation'].replace('ˈ', '').replace('ˌ', '')

			if combine:
				pronunciation.append('-%s' % p)
			else:
				pronunciation.append(p)

		for word in expand(' '.join(expr.split()), refs):
			data[word] = { 'word': word, 'pronunciation': ' '.join(pronunciation).replace(' -', '') }
	return data

class Tester:
	def __init__(self):
		self.passed = 0
		self.failed = 0

	def test_dictionary(self, words, generate_exception_dictionary=False, ipa=True):
		with open('/tmp/words.lst', 'w') as f:
			for item in sorted(words.keys()):
				f.write('%s\n\n' % item)

		os.system('espeak -v en -xq --ipa -f /tmp/words.lst > /tmp/pronunciation.lst')

		with open('/tmp/pronunciation.lst') as f:
			espeak = [ ' '.join(x.split()) for x in f.read().split('\n') ]

		for i, word in enumerate(sorted(words.keys())):
			data = words[word]
			if not 'pronunciation' in data.keys():
				data['pronunciation'] = words[ data['replacement'] ]['pronunciation']

			expected = '/%s/' % data['pronunciation']
			actual   = '/%s/' % espeak[i]

			expected = expected.replace('.', '') # espeak does not support syllabic annotations, so ignore

			actual = actual.replace('əL',   'l̩') # espeak --ipa does not map '@L' correctly, so use the syllabic form (different to 'əl')
			actual = actual.replace('ɪˈɑː', 'iˈɑː') # espeak does not differ in sound, but preserve the /i/ vs /ɪ/ distinction
			actual = actual.replace('ɪ/', 'i/') # espeak does not differ in sound, but preserve the /i/ vs /ɪ/ distinction
			actual = actual.replace('ɪ ', 'i ') # espeak does not differ in sound, but preserve the /i/ vs /ɪ/ distinction
			actual = actual.replace('ai', 'aɪ') # ... but correct 'aɪ' usage
			actual = actual.replace('ei', 'eɪ') # ... and 'eɪ' usage
			actual = actual.replace('ɔi', 'ɔɪ') # ... and 'ɔɪ' usage
			actual = actual.replace('əʊi', 'əʊɪ') # ... and 'əʊɪ' usage
			actual = actual.replace('/ˈə/', '/ə/') # espeak stresses the schwa which is typically unstressed

			if expected == actual:
				if not generate_exception_dictionary:
					print '%s %s ... pass' % (word, '/%s/' % data['pronunciation'])
				self.passed = self.passed + 1
			else:
				if generate_exception_dictionary:
					print_exception(word, '/%s/' % data['pronunciation'], ipa=ipa)
				else:
					print '%s %s got: %s ... fail' % (word, '/%s/' % data['pronunciation'], actual)
				self.failed = self.failed + 1

	def summary(self):
		print '%d passed %d failed %d total' % (self.passed, self.failed, self.passed + self.failed)

if __name__ == '__main__':
	args = [ x for x in sys.argv if not x.startswith('--') ]

	dictionarydir = args[1]
	dictionaries = [ os.path.join(dictionarydir, x) for x in os.listdir(dictionarydir) if x.endswith('.dict') ]
	words = parse_dictionaries(dictionaries)

	if '--new-words' in sys.argv:
		have_words = [ data['word'].lower() for data in words.values() ]
		#print '\n'.join(have_words)
		with open(args[2]) as f:
			for line in f:
				line = line.replace('\n', '')
				if line.lower() not in have_words:
					print line
	elif '--list-words' in sys.argv:
		for word in words.values():
			print word['word']
	else:
		test = Tester()
		test.test_dictionary(words, generate_exception_dictionary='--exception-dictionary' in sys.argv, ipa='--ipa' in sys.argv)
		if not '--exception-dictionary' in sys.argv:
			test.summary()
