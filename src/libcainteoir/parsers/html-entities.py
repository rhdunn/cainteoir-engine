#!/usr/bin/python

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


# Processing for the HTML entities data file listed at http://svn.whatwg.org/webapps/entities-unicode.inc:
#    wget -O html-entities.dat http://svn.whatwg.org/webapps/entities-unicode.inc

import os

data='html-entities.dat'
datasrc='http://svn.whatwg.org/webapps/entities-unicode.inc'

def utf8(x):
	return unichr(int(x.replace('U+', ''), 16)).encode('utf-8')

def u(x):
	c = unichr(int(x.replace('U+', ''), 16)).encode('utf-8')
	if c == '"' or c == '\'':
		return '\\%s' % c
	return repr(c).replace('\'', '')

entities = {
	'A': {}, 'B': {}, 'C': {}, 'D': {}, 'E': {}, 'F': {}, 'G': {}, 'H': {}, 'I': {}, 'J': {},
	'K': {}, 'L': {}, 'M': {}, 'N': {}, 'O': {}, 'P': {}, 'Q': {}, 'R': {}, 'S': {}, 'T': {},
	'U': {}, 'V': {}, 'W': {}, 'X': {}, 'Y': {}, 'Z': {},
	'a': {}, 'b': {}, 'c': {}, 'd': {}, 'e': {}, 'f': {}, 'g': {}, 'h': {}, 'i': {}, 'j': {},
	'k': {}, 'l': {}, 'm': {}, 'n': {}, 'o': {}, 'p': {}, 'q': {}, 'r': {}, 's': {}, 't': {},
	'u': {}, 'v': {}, 'w': {}, 'x': {}, 'y': {}, 'z': {},
}

if not os.path.exists(data):
	print 'downloading entity data file "%s" to "%s" ...' % (datasrc, data)
	os.system('wget -O %s %s' % (data, datasrc))

print 'processing entity data file "%s" ...' % data

with open(data) as f:
	for line in f:
		a, b, c, d, e, f, g, h, i, j, k, l, m = line.replace('\n', '').split('>')

		ent, _ = d.split(';')
		uchar = g.replace('</td', '').split()

		entities[ ent[0] ][ent] = uchar

print "entity count -- %s" % ', '.join([ '%s: %d' % (idx, len(entities[idx])) for idx in sorted(entities.keys()) ])

print 'generating html-entities.h ...'

with open('html-entities.h', 'w') as f:
	f.write('// Generated from http://svn.whatwg.org/webapps/entities-unicode.inc\n')
	for idx in sorted(entities.keys()):
		f.write('static const entity html_entities_%s[] = {\n' % idx)
		for ent in sorted(entities[idx].keys()):
			uchar = entities[idx][ent]

			if len(uchar) == 1:
				f.write('{ "%s", "%s" },\n' % (ent, u(uchar[0])))
			else:
				f.write('{ "%s", "%s%s" },\n' % (ent, u(uchar[0]), u(uchar[1])))
		f.write('};\n')
		f.write('static const entity_set html_entity_set_%s = {\n' % idx)
		f.write('html_entities_%s,\n' % idx)
		f.write('html_entities_%s + (sizeof(html_entities_%s) / sizeof(html_entities_%s[0]))\n' % (idx, idx, idx))
		f.write('};\n')

	f.write('static const entity_set * html_entities[] = {\n')
	for idx in sorted(entities.keys()):
		f.write('&html_entity_set_%s,\n' % idx)
	f.write('};\n')

print 'generating ../../../tests/xmlparser/entity/html.xml ...'

with open('../../../tests/xmlparser/entity/html.xml', 'w') as f:
	f.write('<div>\n')
	for idx in sorted(entities.keys()):
		for ent in sorted(entities[idx].keys()):
			f.write('&%s;\n' % ent)
	f.write('</div>\n')

print 'generating ../../../tests/xmlparser/entity/html.tags ...'

with open('../../../tests/xmlparser/entity/html.tags', 'w') as f:
	f.write('|begin-tag| div\n')
	f.write('|text| """\n')
	for idx in sorted(entities.keys()):
		for ent in sorted(entities[idx].keys()):
			uchar = entities[idx][ent]

			if len(uchar) == 1:
				f.write('%s\n' % utf8(uchar[0]))
			else:
				f.write('%s%s\n' % (utf8(uchar[0]), utf8(uchar[1])))
	f.write('"""\n')
	f.write('|end-tag| div\n')
	f.write('|text| """\n')
	f.write('"""\n')
