#!/usr/bin/python

# Copyright (C) 2012-2014 Reece H. Dunn
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
import json

from xml.dom import minidom

def read_data(path, split_char=':'):
	with open(path) as f:
		for line in f:
			line = line.replace('\n', '')
			if not line.startswith('#'):
				yield line.split(split_char)

def fold_lines(path):
	next_line = None
	with open(path) as f:
		for line in f:
			line = line.replace('\n', '')
			if line.startswith(' '):
				next_line = '%s%s' % (next_line, line[1:])
				continue
			if next_line:
				yield next_line
			next_line = line

def iana_subtag_entries(path):
	tag = {}
	for line in fold_lines(path):
		if line.startswith('%%'):
			if len(line.split()) > 1: # comment
				continue

			if 'Type' in tag:
				yield tag
			tag = {}
			continue

		packed = line.split(': ')
		key    = packed[0]
		value  = ': '.join(packed[1:])

		if key == 'Description':
			# Only select the first Description. This handles subtag codes
			# that have multiple descriptions (e.g. 'es' maps to "Spanish"
			# and "Castilian").
			if not key in tag.keys():
				tag[key] = value
		else:
			tag[key] = value
	yield tag

typemap = {
	'extlang':       'ExtLang',
	'grandfathered': 'Grandfathered',
	'language':      'Language',
	'private':       'Private',
	'redundant':     'Redundant',
	'region':        'Region',
	'script':        'Script',
	'variant':       'Variant',
}

scopemap = {
	'collection':    'Collection',
	'macrolanguage': 'MacroLanguage',
	'special':       'Special',
	'private-use':   'PrivateUse',
}

def read_iana_subtags(path, datasrc=None):
	if not os.path.exists(path):
		print 'downloading IANA subtag registry file "%s" to "%s" ...' % (datasrc, path)
		os.system('wget -O %s %s' % (path, datasrc))

	tags = {}
	for tag in iana_subtag_entries(path):
		if 'Subtag' in tag.keys():
			ref = tag['Subtag']
			del tag['Subtag']
		else:
			ref = tag['Tag']
			del tag['Tag']

		if 'Scope' in tag.keys():
			if tag['Type'] != 'language':
				raise Exception('"Scope" property unexpected for Type="%s"' % tag['Type'])

			tag['Type'] = scopemap[ tag['Scope'] ]
			del tag['Scope']
		else:
			tag['Type'] = typemap[ tag['Type'] ]

		if '..' not in ref: # exclude private use definitions
			tags[ref] = tag
	return tags

def attr(tag, attr):
	value = tag.getAttribute(attr)
	if type(value).__name__ == 'unicode':
		return value.encode('utf-8')
	return value

def read_iso_639(path):
	doc = minidom.parse(path).documentElement
	tags = {}
	for tag in doc.getElementsByTagName('iso_639_entry'):
		iso_639_1  = attr(tag, 'iso_639_1_code')
		iso_639_2b = attr(tag, 'iso_639_2B_code')
		iso_639_2t = attr(tag, 'iso_639_2T_code')
		name       = attr(tag, 'name')

		if iso_639_1  != '': tags[iso_639_1]  = name
		if iso_639_2b != '': tags[iso_639_2b] = name
		tags[iso_639_2t] = name
	return tags

def read_iso_3166(path):
	doc = minidom.parse(path).documentElement
	tags = {}
	for tag in doc.getElementsByTagName('iso_3166_entry'):
		alpha2  = attr(tag, 'alpha_2_code')
		numeric = attr(tag, 'numeric_code')
		name    = attr(tag, 'name')

		tags[alpha2]  = name
		tags[numeric] = name
	return tags

def read_iso_15924(path):
	doc = minidom.parse(path).documentElement
	tags = {}
	for tag in doc.getElementsByTagName('iso_15924_entry'):
		alpha4  = attr(tag, 'alpha_4_code')
		name    = attr(tag, 'name')

		tags[alpha4] = name
	return tags

tags            = read_iana_subtags('languages.dat', 'http://www.iana.org/assignments/language-subtag-registry')
tags.update(read_iana_subtags('private-use.dat'))

iso_639_codes   = read_iso_639('/usr/share/xml/iso-codes/iso_639.xml')
iso_3166_codes  = read_iso_3166('/usr/share/xml/iso-codes/iso_3166.xml')
iso_15924_codes = read_iso_15924('/usr/share/xml/iso-codes/iso_15924.xml')

for name, tag in sorted(tags.items()):
	try:
		iso_name = None
		if tag['Type'] in ['Script']:
			iso_name = iso_15924_codes[ name ]
		if tag['Type'] in ['Region']:
			iso_name = iso_3166_codes[ name ]
		if tag['Type'] in ['Special', 'MacroLanguage', 'Language', 'Collection']:
			iso_name = iso_639_codes[ name ]

		if iso_name and iso_name != tag['Description']:
			tag['Description'] = iso_name

	except KeyError:
		pass

for filename in os.listdir('languages'):
	if filename.endswith('.dat'):
		for rel, a, b in read_data(os.path.join('languages', filename)):
			if rel == 'h': # ISO 639-5 Hierarchy
				tags[a]['Classification'] = b
			elif rel == 'c':
				tags[a]['LanguageCollection'] = b
			elif rel == 'a':
				tags[a]['Ancestor'] = b

# Language <=> Script mapping

datasrc='http://unicode.org/repos/cldr/trunk/common/supplemental/supplementalData.xml'
path='supplementalData.xml'
if not os.path.exists(path):
	print 'downloading file "%s" to "%s" ...' % (datasrc, path)
	os.system('wget -O %s %s' % (path, datasrc))

doc = minidom.parse(path).documentElement
for tag in doc.getElementsByTagName('language'):
	lang    = attr(tag, 'type')
	scripts = attr(tag, 'scripts')
	if lang and scripts:
		tags[lang]['HasScript'] = scripts.split()
		for script in scripts.split():
			if not 'IsScriptOf' in tags[script].keys():
				tags[script]['IsScriptOf'] = []
			tags[script]['IsScriptOf'].append(lang)

# generate RDF metadata for the language data

tagnames = {
	'Added':           ('date',      'iana:added'),
	'Comments':        ('string:en', 'iana:comments'),
	'Deprecated':      ('date',      'iana:deprecated'),
	'Description':     ('string:en', 'iana:label'),
	'HasScript':       ('resources', 'iana:hasScript'),      # => iana:Script
	'IsScriptOf':      ('resources', 'iana:isScriptOf'),     # => iana:Language
	'Macrolanguage':   ('resource',  'iana:macrolanguage'),  # => iana:MacroLanguage
	'Prefix':          ('string',    'iana:prefix'),         # => Language Tag
	'Preferred-Value': ('resource',  'iana:preferredValue'), # => subtag
	'Suppress-Script': ('resource',  'iana:suppressScript'), # => iana:Script
}

subtag_uri='http://rhdunn.github.com/cainteoir/data/iana/subtags#'

with open('languages.rdf', 'w') as f:
	f.write('<?xml version="1.0" encoding="utf-8"?>\n')
	f.write('<rdf:RDF\n')
	f.write('	xmlns:rdf ="http://www.w3.org/1999/02/22-rdf-syntax-ns#"\n')
	f.write('	xmlns:rdfs="http://www.w3.org/2000/01/rdf-schema#"\n')
	f.write('	xmlns:iana="http://reecedunn.co.uk/schema/2013/iana#"\n')
	f.write('	xmlns:dc  ="http://purl.org/dc/elements/1.1/"\n')
	f.write('	xmlns:dct ="http://purl.org/dc/terms/"\n')
	f.write('	xmlns:cc  ="http://web.resource.org/cc/"\n')
	f.write('>\n')

	for name, tag in sorted(tags.items()):
		f.write('<iana:%s rdf:about="%s%s">\n' % (tag['Type'], subtag_uri, name))
		f.write('	<iana:code>%s</iana:code>\n' % name)
		for key, value in sorted(tag.items()):
			if key not in ['Type', 'Ancestor', 'Classification', 'LanguageCollection']:
				reftype, ref = tagnames[key]
				if reftype in ['date', 'string']:
					f.write('	<%s>%s</%s>\n' % (ref, value, ref))
				elif reftype in ['string:en']:
					f.write('	<%s xml:lang="en">%s</%s>\n' % (ref, value, ref))
				elif reftype in ['resource']:
					f.write('	<%s rdf:resource="%s%s"/>\n' % (ref, subtag_uri, value))
				elif reftype in ['resources']:
					for v in value:
						f.write('	<%s rdf:resource="%s%s"/>\n' % (ref, subtag_uri, v))
		f.write('</iana:%s>\n' % tag['Type'])
	f.write('</rdf:RDF>\n')

# generate a language code tree ...

attributes = {
	'Ancestor': 'blue',
	'Classification': 'green',
	'LanguageCollection': 'orange',
	#'Macrolanguage': 'red',
}

with open('languages.dot', 'w') as f:
	f.write('digraph "Language Codes" {\n')
	f.write('	node [shape=box]')
	nodes = set()
	for name, tag in sorted(tags.items()):
		if tag['Type'] in ['Language', 'ExtLang', 'Collection', 'MacroLanguage']:
			for key, color in attributes.items():
				if key in tag.keys():
					f.write('	"%s" -> "%s" [color=%s]\n' % (name, tag[key], color))
					nodes.add(name)
					nodes.add(tag[key])
	for name in nodes:
		f.write('	"%s" [tooltip="%s"]\n' % (name, tags[name]['Description']))
	f.write('}\n')

