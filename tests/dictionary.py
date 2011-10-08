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


class Word:
	def __init__(self, word, attributes=[]):
		self.word = word
		self.attributes = attributes

	def __str__(self):
		return self.word

	def __hash__(self):
		return self.word.__hash__()

	def __eq__(self, other): return self.word == other.word
	def __ne__(self, other): return self.word != other.word
	def __lt__(self, other): return self.word <  other.word
	def __le__(self, other): return self.word <= other.word
	def __gt__(self, other): return self.word >  other.word
	def __ge__(self, other): return self.word >= other.word


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
			('-', ''),
			('ː', ':'),
			('ˈ', '\''),
			('ˌ', ','),
			('ɡ', 'g'),
			('ʃ', 'S'),
			('ʒ', 'Z'),
			('ð', 'D'),
			('θ', 'T'),
			('ŋ', 'N'),
			('ɹ', 'r'),
			('ɬ', 'l#'),
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

		w = str(word)
		if w == w.upper(): # all upper case
			w = w.lower() # ... espeak uses lower case for matching abbreviations (NATO, USA, UK, ...)

		wordlist = []
		prev = ' '
		for i, c in enumerate(w):
			if c >= 'A' and c <= 'Z' and i != 0:
				if prev != '\'': # for A'B, e.g. in the name "O'Connell"
					wordlist.append(' ')
				wordlist.append(c.lower())
			else:
				wordlist.append(c)
			prev = c
		w = ''.join(wordlist)

		if '-' in w or ' ' in w:
			print '(%s)%30s%s %s' % (w, ' ', pronunciation.replace('/', ''), ' '.join(['$%s' % a for a in word.attributes]))
		else:
			print '%s%30s%s %s' %   (w, ' ', pronunciation.replace('/', ''), ' '.join(['$%s' % a for a in word.attributes]))

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
					word, pronunciation, attributes = line.split('"')
					word = Word(' '.join(word.split()), attributes.split())
					pronunciation = ' '.join(pronunciation.split())
					aliases[word] = pronunciation
				else:
					expr, pronunciation, attributes = line.split('/')
					pronunciation = ' '.join(pronunciation.split())
					for word in expand(' '.join(expr.split()), refs):
						word = Word(word, attributes.split())
						if word in data.keys() and data[word]['pronunciation'] != pronunciation:
							raise Exception('Mismatched pronunciation for duplicate word "%s"' % word)
						data[word] = { 'word': word, 'pronunciation': pronunciation }
	for expr, alias in aliases.items():
		pronunciation = []
		for sub in alias.split(' '):
			pron = []
			for subword in sub.split('-'):
				try:
					if subword.startswith('\''): # retain stress (with)
						sw = Word(subword.replace('\'', ''))
						p = data[sw]['pronunciation']
					elif subword.startswith(','): # invert stress (against)
						sw = Word(subword.replace(',', ''))
						p = data[sw]['pronunciation'].replace('ˈ', ',').replace('ˌ', 'ˈ').replace(',', 'ˌ')
					else: # no stress
						sw = Word(subword)
						p = data[sw]['pronunciation'].replace('ˈ', '').replace('ˌ', '')
				except KeyError:
					raise Exception('No entry found for "%s" when expanding the pronunciation "%s"' % (sw.word, alias))
				pron.append(p)
			pronunciation.append('-'.join(pron))

		for word in expand(' '.join(expr.word.split()), refs):
			word = Word(word)
			data[word] = { 'word': word, 'pronunciation': ' '.join(pronunciation) }
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

			actual = actual.replace('əL',   'l̩') # espeak --ipa does not map '@L' correctly, so use the syllabic form (different to 'əl')
			actual = actual.replace('ɪˈɑː', 'iˈɑː') # espeak does not differ in sound, but preserve the /i/ vs /ɪ/ distinction
			actual = actual.replace('ɪ/', 'i/') # espeak does not differ in sound, but preserve the /i/ vs /ɪ/ distinction
			actual = actual.replace('ɪ ', 'i ') # espeak does not differ in sound, but preserve the /i/ vs /ɪ/ distinction
			actual = actual.replace('ai', 'aɪ') # ... but correct 'aɪ' usage
			actual = actual.replace('ei', 'eɪ') # ... and 'eɪ' usage
			actual = actual.replace('ɔi', 'ɔɪ') # ... and 'ɔɪ' usage
			actual = actual.replace('əʊi', 'əʊɪ') # ... and 'əʊɪ' usage
			actual = actual.replace('/ˈə/', '/ə/') # espeak stresses the schwa which is typically unstressed

			expected = expected.replace('i-', 'ɪ') # espeak does not differ in sound, but preserve the /i/ vs /ɪ/ distinction

			expected = expected.replace('.', '') # espeak does not support syllabic annotations, so ignore
			expected = expected.replace('-', '') # ignore run-on expressions for "say-as" pronunciations

			if expected == actual:
				if not generate_exception_dictionary:
					print '%s %s ... pass' % (word, '/%s/' % data['pronunciation'])
				self.passed = self.passed + 1
			else:
				if generate_exception_dictionary:
					print_exception(word, '/%s/' % data['pronunciation'], ipa=ipa)
				else:
					print '%s %s expected: %s got: %s ... fail' % (word, '/%s/' % data['pronunciation'], expected, actual)
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
