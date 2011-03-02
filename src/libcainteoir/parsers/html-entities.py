#!/usr/bin/python

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

entities = {}

if not os.path.exists(data):
	print 'downloading entity data file "%s" to "%s" ...' % (datasrc, data)
	os.system('wget -O %s %s' % (data, datasrc))

print 'processing entity data file "%s" ...' % data

with open(data) as f:
	for line in f:
		a, b, c, d, e, f, g, h, i, j, k, l, m = line.replace('\n', '').split('>')

		ent, _ = d.split(';')
		uchar = g.replace('</td', '').split()

		entities[ent] = uchar

print 'generating html-entities.h ...'

with open('html-entities.h', 'w') as f:
	f.write('// Generated from http://svn.whatwg.org/webapps/entities-unicode.inc\n')
	for ent in sorted(entities.keys()):
		uchar = entities[ent]

		if len(uchar) == 1:
			f.write('{ "%s", "%s" },\n' % (ent, u(uchar[0])))
		else:
			f.write('{ "%s", "%s%s" },\n' % (ent, u(uchar[0]), u(uchar[1])))

print 'generating ../../../tests/xmlparser/entity/html.xml ...'

with open('../../../tests/xmlparser/entity/html.xml', 'w') as f:
	f.write('<div>\n')
	for ent in sorted(entities.keys()):
		f.write('&%s;\n' % ent)
	f.write('</div>\n')

print 'generating ../../../tests/xmlparser/entity/html.tags ...'

with open('../../../tests/xmlparser/entity/html.tags', 'w') as f:
	f.write('|begin-tag| div\n')
	f.write('|text| """\n')
	for ent in sorted(entities.keys()):
		uchar = entities[ent]

		if len(uchar) == 1:
			f.write('%s\n' % utf8(uchar[0]))
		else:
			f.write('%s%s\n' % (utf8(uchar[0]), utf8(uchar[1])))
	f.write('"""\n')
	f.write('|end-tag| div\n')
	f.write('|text| """\n')
	f.write('"""\n')
