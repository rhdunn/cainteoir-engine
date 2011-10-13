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
import re


class Word:
	def __init__(self, word, attributes=None):
		self.word = word
		self.attributes = attributes

	def __str__(self):
		return self.word

	def __repr__(self):
		return repr((self.word, self.attributes))

	def __hash__(self):
		return self.word.__hash__()

	def __eq__(self, other):
		if self.attributes == None or other.attributes == None:
			return self.word == other.word
		return self.word == other.word and self.attributes == other.attributes

	def __lt__(self, other):
		return self.word < other.word


def print_exception(word, pronunciation, ipa=True):
	if ipa:
		print '%-30s %s' % (word, pronunciation)
	else:
		replacements = [
			(' ', '||'),
			('.', ''),
			('-', ''),
			('ː', ':'),
			('ˈ', '\''),
			('ˌ', ','),

			('aʊl̩', 'aU|@L'),
			('əʊl̩', 'oU|@L'),
			('aɪl̩', 'aI|@L'),
			('eɪl̩', 'eI|@L'),

			('l̩', '@L'),

			('aʊ', 'aU'),
			('əʊ', 'oU'),

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

		if not word.attributes:
			word.attributes = []

		w = str(word)

		if 'speakletter' in word.attributes:
			w = '_%s' % w
			word.attributes.remove('speakletter')

		if 'weak' in word.attributes:
			pronunciation = pronunciation.replace('\'', '%')
			word.attributes.remove('weak')

		if 'norcolouring' in word.attributes:
			pronunciation = pronunciation.replace('@/', '@2/')
			word.attributes.remove('norcolouring')

		if 'breathstop' in word.attributes:
			word.attributes.remove('breathstop')

		if pronunciation.startswith('/\'')  and not ',' in pronunciation:
			# When stress is explicitly specified in a word, espeak uses that stress
			# regardless of any prosody information. This causes the small words to
			# read in a non-natural way.
			#
			# This strips stress information if the word only has a primary stress and
			# that stress is at the beginning to allow espeak prosody to work.
			pronunciation = pronunciation.replace('/\'', '/')

		if w == w.upper(): # all upper case
			w = w.lower() # ... espeak uses lower case for matching abbreviations (NATO, USA, UK, ...)
			if 'allcaps' not in word.attributes:
				word.attributes.append('allcaps')

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
		if c in '[]<>|(){}':
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
EXPR_IN_SEGMENT = 5 # {a|b|c}

def parse_expression(expr, refs):
	words = []
	endings = ['']

	state = EXPR_STRING
	fragment = None

	for token in lex_expression(expr):
		if state == EXPR_STRING:
			if token == '(':
				state = EXPR_IN_CHOICE
				fragment = []
			elif token == '{':
				state = EXPR_IN_SEGMENT
				fragment = []
			elif token == '[':
				state = EXPR_IN_ONEOF
			elif token == '<':
				state = EXPR_IN_REPLACE
			elif token != '':
				words.append(token)
		elif state == EXPR_IN_CHOICE:
			if token == '|':
				pass
			elif token == ')':
				words.append(fragment)
				state = EXPR_STRING
				fragment = None
			else:
				fragment.append(token)
		elif state == EXPR_IN_ONEOF:
			if token == ']':
				words.append(list(fragment))
				state = EXPR_STRING
			else:
				fragment = token
		elif state == EXPR_IN_REPLACE:
			if token == '>':
				words.append(refs[fragment])
				state = EXPR_STRING
			else:
				fragment = token
		elif state == EXPR_IN_SEGMENT:
			if token == '|':
				pass
			elif token == '}':
				endings = fragment
				state = EXPR_STRING
				fragment = None
			else:
				fragment.append(token)

	return words, endings

def expand(expr):
	words = []
	for fragment in expr:
		if type(fragment).__name__ == 'list':
			if len(words) == 0:
				words = fragment
			else:
				newwords = []
				for word in words:
					newwords.extend([word + x for x in fragment])
				words = newwords
		elif type(fragment).__name__ == 'str':
			if len(words) == 0:
				words = [fragment]
			else:
				words = [word + fragment for word in words]
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
	re_ref   = re.compile('([a-zA-Z]+)=(.*)')
	re_alias = re.compile('([^"]+) "([^"]+)"(.*)')
	re_pron  = re.compile('([^/]+) /([^/]+)/(.*)')

	data = {}
	refs = {}
	aliases = {}
	for dictionary in dictionaries:
		with open(dictionary) as f:
			for line in f:
				if line == '\n' or line.startswith('#'):
					continue

				m = re_ref.match(line)
				if m:
					ref = m.group(1)
					expression = m.group(2)

					words, endings = parse_expression(expression, refs)
					refs[ref] = expand(words)
					continue

				m = re_alias.match(line)
				if m:
					words = ' '.join(m.group(1).split())
					alias = m.group(2)
					attributes = m.group(3).split()

					word = Word(words, attributes)
					aliases[word] = alias
					continue

				m = re_pron.match(line)
				if m:
					words, endings = parse_expression(' '.join(m.group(1).split()), refs)
					words = sorted(expand(words))
					pronunciation, pronunciation_endings = parse_expression(m.group(2), {})
					attributes = m.group(3).split()

					for ending, pronunciation_ending in zip(endings, pronunciation_endings):
						for word in words:
							word = Word(word + ending, attributes)
							pron = pronunciation[0] + pronunciation_ending
							if word in data.keys() and data[word]['pronunciation'] != pron:
								raise Exception('Mismatched pronunciation for duplicate word "%s"' % word)
							data[word] = { 'word': word, 'pronunciation': pron }
					continue

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

		words, endings = parse_expression(' '.join(expr.word.split()), refs)
		for word in expand(words):
			word = Word(word)
			data[word] = { 'word': word, 'pronunciation': ' '.join(pronunciation) }
	return data

class Tester:
	def __init__(self):
		self.passed = 0
		self.failed = 0

	def test_dictionary(self, words, generate_exception_dictionary=False, use_actual=False, ipa=True, show_all=False):
		with open('/tmp/words.lst', 'w') as f:
			for item in sorted(words.keys()):
				f.write('%s\n\n' % item)

		os.system('espeak -v en -xq --ipa -f /tmp/words.lst > /tmp/pronunciation.lst')

		with open('/tmp/pronunciation.lst') as f:
			espeak = [ ' '.join(x.split()) for x in f.read().split('\n') ]

		exceptions = {}

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
			if actual.startswith('/ˈə') and not actual.startswith('/ˈəʊ'):
				actual = actual.replace('/ˈə', '/ə') # espeak stresses the schwa which is typically unstressed
			actual = actual.replace('ˈə/', 'ə/') # espeak stresses the schwa which is typically unstressed

			expected = expected.replace('i-', 'ɪ') # espeak does not differ in sound, but preserve the /i/ vs /ɪ/ distinction

			expected = expected.replace('.', '') # espeak does not support syllabic annotations, so ignore
			expected = expected.replace('-', '') # ignore run-on expressions for "say-as" pronunciations

			if 'ˌ' in expected and not 'ˈ' in expected:
				expected = expected.replace('ˌ', 'ˈ') # espeak uses a primary stress if there is only a secondary stress present

			if expected == actual or (word.attributes and 'speakletter' in word.attributes):
				if not generate_exception_dictionary:
					print '%s %s ... pass' % (word, '/%s/' % data['pronunciation'])
				self.passed = self.passed + 1
			else:
				if generate_exception_dictionary:
					if use_actual:
						exceptions[word] = actual
					else:
						exceptions[word] = '/%s/' % data['pronunciation']
				else:
					print '%s %s expected: %s got: %s ... fail' % (word, '/%s/' % data['pronunciation'], expected, actual)
				self.failed = self.failed + 1

		for word, pronunciation in sorted(exceptions.items()):
			print_exception(word, pronunciation, ipa=ipa)

	def summary(self):
		print '%d passed %d failed %d total' % (self.passed, self.failed, self.passed + self.failed)

if __name__ == '__main__':
	args = [ x for x in sys.argv if not x.startswith('--') ]

	dictionarydir = args[1]
	dictionaries = [ os.path.join(dictionarydir, x) for x in os.listdir(dictionarydir) if x.endswith('.dict') ]
	words = parse_dictionaries(dictionaries)

	if '--new-words' in sys.argv:
		have_words = [ str(data['word']).lower() for data in words.values() ]
		new_words  = {}
		with open(args[2]) as f:
			for line in f:
				for m in re.compile('([\\w\']+)').finditer(line):
					word = Word(m.group(1).lower())
					if word.word not in have_words:
						new_words[word] = {'word': word, 'pronunciation': ''}
		if '--list-words' in sys.argv:
			for word in new_words.keys():
				print word
		else:
			test = Tester()
			test.test_dictionary(words=new_words, generate_exception_dictionary=True, use_actual=True, ipa='--ipa' in sys.argv)
	elif '--list-words' in sys.argv:
		for word in words.values():
			print word['word']
	else:
		test = Tester()
		test.test_dictionary(words, generate_exception_dictionary='--exception-dictionary' in sys.argv, ipa='--ipa' in sys.argv)
		if not '--exception-dictionary' in sys.argv:
			test.summary()
