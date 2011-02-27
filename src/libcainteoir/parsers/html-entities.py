#!/usr/bin/python

# Processing for the HTML entities data file listed at http://svn.whatwg.org/webapps/entities-unicode.inc:
#    wget -O html-entities.dat http://svn.whatwg.org/webapps/entities-unicode.inc

data='html-entities.dat'
header='html-entities.h'

entities = {}

with open('html-entities.dat') as f:
	for line in f:
		a, b, c, d, e, f, g, h, i, j, k, l, m = line.replace('\n', '').split('>')

		ent, _ = d.split(';')
		uchar = g.replace('</td', '').split()

		entities[ent] = uchar

def u(x):
	c = unichr(int(x.replace('U+', ''), 16)).encode('utf-8')
	if c == '"' or c == '\'':
		return '\\%s' % c
	return repr(c).replace('\'', '')

with open('html-entities.h', 'w') as f:
	f.write('// Generated from http://svn.whatwg.org/webapps/entities-unicode.inc\n')
	for ent in sorted(entities.keys()):
		uchar = entities[ent]

		if len(uchar) == 1:
			f.write('{ "%s", "%s" },\n' % (ent, u(uchar[0])))
		else:
			f.write('{ "%s", "%s%s" },\n' % (ent, u(uchar[0]), u(uchar[1])))
