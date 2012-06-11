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
import json

from xml.dom import minidom
from operator import itemgetter, attrgetter

unicode_data_path = sys.argv[1]

def utf8(c):
	return ''.join(['\\x%02X' % ord(byte) for byte in unichr(c).encode('utf-8')])

def within(c, r):
	r_first, r_last = r
	if isinstance(c, int):
		return r_first <= c and c <= r_last
	c_first, c_last = c
	return r_first <= c_first and c_last <= r_last

def enumerate_codepoints(codepoints):
	if isinstance(codepoints, int):
		yield codepoints
	else:
		first, last = codepoints
		for codepoint in range(first, last+1):
			yield codepoint

def unicode_data_file(path, name):
	for filename in os.listdir(path):
		if filename.startswith(name) and filename.endswith('.txt'):
			return os.path.join(path, filename)
	raise Exception('Unable to find Unicode Character Data file %s' % name)

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
				data[0] = (int(first[0], 16), int(data[0], 16))
				data[1] = data[1].replace(', Last>', '>')
				first = None
			elif '..' in data[0]:
				codepoints = data[0].split('..')
				data[0] = (int(codepoints[0], 16), int(codepoints[1], 16))
			else:
				data[0] = int(data[0], 16)
			yield data

def select(tag, ns, name):
	value = tag.getElementsByTagNameNS(ns, name)[0].firstChild.nodeValue
	if type(value).__name__ == 'unicode':
		return value.encode('utf-8')
	return value

def parse_script_codes(language_data):
	doc = minidom.parse(language_data).documentElement
	unicode_names = { # These are where the names differ between IANA and Unicode:
		'Cans': ['canadian_aboriginal'],
		'Egyp': ['egyptian_hieroglyphs'],
		'Hrkt': ['katakana_or_hiragana'],
		'Mtei': ['meetei_mayek'],
		'Nkoo': ['nko'],
		'Phag': ['phags_pa'],
		'Zyyy': ['common', 'inherited'],
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
			scripts[name.lower()] = ref
	return scripts

script_codes = parse_script_codes('languages.rdf')

unicode_char = {}
for data in read_data(unicode_data_file(unicode_data_path, 'UnicodeData')):
	if data[1] == '<control>' and data[10] != '':
		name = data[10]
	else:
		name = data[1]
	if name.startswith('<') and not name == '<control>':
		name = name[1:-1]
	for codepoint in enumerate_codepoints(data[0]):
		unicode_char[codepoint] = {
			'name': name,
			'category': data[2],
			'script': 'Zzzz', # Unknown
			'whitespace': '-',
			'dash': '-',
			'hyphen': '-',
			'quote': '-',
			'terminal': '-',
			'end-of-sentence': '-',
		}

for data in read_data(unicode_data_file(unicode_data_path, 'DerivedAge')):
	for codepoint in enumerate_codepoints(data[0]):
		try:
			unicode_char[codepoint]['age'] = data[1]
		except KeyError:
			pass

for data in read_data(unicode_data_file(unicode_data_path, 'Scripts')):
	script = script_codes[data[1].lower()]
	for codepoint in enumerate_codepoints(data[0]):
		try:
			unicode_char[codepoint]['script'] = script
		except KeyError:
			pass

for data in read_data(unicode_data_file(unicode_data_path, 'PropList')):
	props = {
		'White_Space': ('whitespace', 'W'),
		'Dash': ('dash', 'D'),
		'Hyphen': ('hyphen', 'H'),
		'Quotation_Mark': ('quote', 'Q'),
		'Terminal_Punctuation': ('terminal', 'T'), # sentence or clause
		'STerm': ('end-of-sentence', 'S'),
	}
	if data[1] in props.keys():
		prop, code = props[data[1]]
		for codepoint in enumerate_codepoints(data[0]):
			try:
				unicode_char[codepoint][prop] = code
			except KeyError:
				pass

blocks = {}
for data in read_data(unicode_data_file(unicode_data_path, 'Blocks')):
	blocks[data[1]] = data[0]

for codepoint in enumerate_codepoints((0, int('10FFFF', 16))):
	try:
		data = unicode_char[codepoint]
	except KeyError:
		data = {
			'name': '',
			'category': 'Cn',
			'age': '0.0',
			'script': 'Zzzz',
			'whitespace': '-',
			'dash': '-',
			'hyphen': '-',
			'quote': '-',
			'terminal': '-',
			'end-of-sentence': '-',
		}
	data['codepoint'] = codepoint
	data['utf8'] = utf8(codepoint)
	print '%(script)s,%(category)s,%(whitespace)c%(dash)c%(hyphen)c%(quote)c%(terminal)c%(end-of-sentence)c,%(age)s,%(codepoint)04X,%(utf8)s,%(name)s' % data
