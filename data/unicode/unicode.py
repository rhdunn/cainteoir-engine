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

unicode_data_path = sys.argv[1]

class UnicodeCodePoint:
	def __init__(self, codepoint):
		self.codepoint = codepoint

	def __str__(self):
		return self.codepoint

	def __repr__(self):
		return self.codepoint

class UnicodeRange:
	def __init__(self, first, last):
		self.first = first
		self.last = last

	def __str__(self):
		return '%s..%s' % (self.first, self.last)

	def __repr__(self):
		return '%s..%s' % (self.first, self.last)

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

unicode_data = {}
for data in read_data(os.path.join(unicode_data_path, 'UnicodeData.txt')):
	if data[1] == '<control>' and data[10] != '':
		unicode_data[data[0]] = {'name': data[9], 'category': data[2]}
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
	scripts[data[0]] = data[1]
