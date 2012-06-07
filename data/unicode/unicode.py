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

unicode_data_path = sys.argv[1]

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

names = {}
categories = {}
for data in read_data(os.path.join(unicode_data_path, 'UnicodeData.txt')):
	if data[1] == '<control>' and data[10] != '':
		name = data[10]
	else:
		name = data[1]
	if not name.startswith('<'):
		names[name] = data[0]

	if not data[2] in categories.keys():
		categories[data[2]] = []
	categories[data[2]].append(data[0])

age = {}
for data in read_data(os.path.join(unicode_data_path, 'DerivedAge.txt')):
	if not data[1] in age.keys():
		age[data[1]] = []
	age[data[1]].append(data[0])

blocks = {}
for data in read_data(os.path.join(unicode_data_path, 'Blocks.txt')):
	blocks[data[1]] = data[0]

scripts = {}
for data in read_data(os.path.join(unicode_data_path, 'Scripts.txt')):
	script = script_codes[data[1]]
	if not script in scripts.keys():
		scripts[script] = []
	scripts[script].append(data[0])

#print json.dumps(names, indent=2, sort_keys=True)
#print json.dumps(categories, indent=2, sort_keys=True)
#print json.dumps(age, indent=2, sort_keys=True)
#print json.dumps(blocks, indent=2, sort_keys=True)
#print json.dumps(scripts, indent=2, sort_keys=True)
