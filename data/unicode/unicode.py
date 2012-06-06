#!/usr/bin/python

# Copyright (C) 2012 Reece H. Dunn
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

import os
import sys

from xml.dom import minidom

unicode_data_path = sys.argv[1]

class UnicodeCodePoint:
	def __init__(self, codepoint):
		self.codepoint = int(codepoint, 16)

	def __str__(self):
		return '%06X' % self.codepoint

	def __repr__(self):
		return str(self)

	def __hash__(self):
		return hash(str(self))

	def __eq__(self, other):
		if isinstance(other, UnicodeRange):
			return False
		return self.codepoint == other.codepoint

	def __lt__(self, other):
		if isinstance(other, UnicodeRange):
			return self.codepoint < other.first.codepoint
		return self.codepoint < other.codepoint

class UnicodeRange:
	def __init__(self, first, last):
		self.first = UnicodeCodePoint(first)
		self.last = UnicodeCodePoint(last)

	def __str__(self):
		return '%s..%s' % (self.first, self.last)

	def __repr__(self):
		return str(self)

	def __hash__(self):
		return hash(str(self))

	def __eq__(self, other):
		if isinstance(other, UnicodeCodePoint):
			return False
		return self.first == other.first and self.last == other.last

	def __lt__(self, other):
		if isinstance(other, UnicodeCodePoint):
			return self.first.codepoint < other.codepoint
		return self.first < other.first

def read_data(path, split_char=';'):
	first = None
	with open(path) as f:
		for line in f:
			line = line.replace('\n', '')
			if line == '':
				continue

			if '#' in line:
				line = line.split('#')[0]

			data = [' '.join(x.split()) for x in line.split(split_char)]
			if len(data) == 0 or (len(data) == 1 and data[0] == ''):
				continue

			if data[1].endswith(', First>'):
				first = data
				continue

			if data[1].endswith(', Last>'):
				data[0] = UnicodeRange(first[0], data[0])
				data[1] = data[1].replace(', Last>', '>')
				first = None
			elif '..' in data[0]:
				codepoints = data[0].split('..')
				data[0] = UnicodeRange(codepoints[0], codepoints[1])
			else:
				data[0] = UnicodeCodePoint(data[0])
			yield data

def select(tag, ns, name):
	value = tag.getElementsByTagNameNS(ns, name)[0].firstChild.nodeValue
	if type(value).__name__ == 'unicode':
		return value.encode('utf-8')
	return value

def parse_script_codes(language_data):
	doc = minidom.parse(language_data).documentElement
	unicode_names = { # These are where the names differ between IANA and Unicode:
		'Cans': ['Canadian_Aboriginal'],
		'Egyp': ['Egyptian_Hieroglyphs'],
		'Mtei': ['Meetei_Mayek'],
		'Nkoo': ['Nko'],
		'Phag': ['Phags_Pa'],
		'Zyyy': ['Common', 'Inherited'],
	}
	scripts = {}
	for script in doc.getElementsByTagNameNS('http://rhdunn.github.com/cainteoir/schema/iana#', 'Script'):
		ref = select(script, 'http://www.w3.org/1999/02/22-rdf-syntax-ns#', 'value')
		if ref in unicode_names.keys():
			for name in unicode_names[ref]:
				scripts[name] = ref
		else:
			name = select(script, 'http://purl.org/dc/terms/', 'title')
			if '(' in name:
				name = name.split('(')[0]
			if ',' in name:
				name = name.split(',')[0]
			name = '_'.join(name.split())
			scripts[name] = ref
	return scripts

script_codes = parse_script_codes('../languages.rdf')

unicode_data = {}
for data in read_data(os.path.join(unicode_data_path, 'UnicodeData.txt')):
	if data[1] == '<control>' and data[10] != '':
		unicode_data[data[0]] = {'name': data[10], 'category': data[2]}
	else:
		unicode_data[data[0]] = {'name': data[1], 'category': data[2]}

age = {}
for data in read_data(os.path.join(unicode_data_path, 'DerivedAge.txt')):
	age[data[0]] = data[1]

blocks = {}
for data in read_data(os.path.join(unicode_data_path, 'Blocks.txt')):
	blocks[data[0]] = data[1]

scripts = {}
for data in read_data(os.path.join(unicode_data_path, 'Scripts.txt')):
	scripts[data[0]] = script_codes[data[1]]
