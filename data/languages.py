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
import json

##### IANA Subtag Registry parser

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
		if line == '%%':
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
				# Some descriptions have date ranges in parenthesis, so
				# remove the date information from those entries.
				if ' (' in value:
					tag[key] = value.split(' (')[0]
				else:
					tag[key] = value
		else:
			tag[key] = value
	yield tag

def read_iana_subtags(path):
	datasrc='http://www.iana.org/assignments/language-subtag-registry'
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

		if '..' not in ref: # exclude private use definitions
			tags[ref] = tag
	return tags

typemap = {
	'extlang':       'ExtLang',
	'grandfathered': 'Grandfathered',
	'language':      'Language',
	'redundant':     'Redundant',
	'region':        'Region',
	'script':        'Script',
	'variant':       'Variant',
}

scopemap = {
	'collection':    'Collection',
	'macrolanguage': 'MacroLanguage',
	'special':       'Special',
}

tagnames = {
	'Added':           ('date',      'iana:added'),
	'Comments':        ('string:en', 'dct:description'),
	'Deprecated':      ('date',      'iana:deprecated'),
	'Description':     ('string:en', 'dct:title'),
	'Macrolanguage':   ('resource',  'iana:macrolanguage'),   # => iana:MacroLanguage
	'Prefix':          ('string',    'iana:prefix'),          # => Language Tag
	'Preferred-Value': ('resource',  'iana:preferred-value'), # => subtag
	'Suppress-Script': ('resource',  'iana:suppress-script'), # => iana:Script
}

subtag_uri='http://rhdunn.github.com/cainteoir/data/iana/subtags#'

print """<?xml version="1.0" encoding="utf-8"?>
<rdf:RDF
	xmlns:rdf ="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
	xmlns:rdfs="http://www.w3.org/2000/01/rdf-schema#"
	xmlns:iana="http://rhdunn.github.com/cainteoir/schema/iana#"
	xmlns:dc  ="http://purl.org/dc/elements/1.1/"
	xmlns:dct ="http://purl.org/dc/terms/"
	xmlns:cc  ="http://web.resource.org/cc/"
>"""

tags=read_iana_subtags('languages.dat')
for name, tag in sorted(tags.items()):
	if 'Scope' in tag.keys():
		if tag['Type'] != 'language':
			raise Exception('"Scope" property unexpected for Type="%s"' % tag['Type'])
		typename = scopemap[ tag['Scope'] ]
	else:
		typename = typemap[ tag['Type'] ]

	print '<iana:%s rdf:about="%s%s">' % (typename, subtag_uri, name)
	print '	<rdf:value>%s</rdf:value>' % name
	for key, value in sorted(tag.items()):
		if key not in ['Type', 'Scope']:
			reftype, ref = tagnames[key]
			if reftype in ['date', 'string']:
				print '	<%s>%s</%s>' % (ref, value, ref)
			elif reftype in ['string:en']:
				print '	<%s xml:lang="en">%s</%s>' % (ref, value, ref)
			elif reftype in ['resource']:
				print '	<%s rdf:resource="%s%s"/>' % (ref, subtag_uri, value)
	print '</iana:%s>' % typename
print '</rdf:RDF>'
