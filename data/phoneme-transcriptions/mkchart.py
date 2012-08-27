#!/usr/bin/python

import sys
import os
import re

##### Features ...

features = {}

with open('phoneme-features.csv') as f:
	for line in f:
		try:
			feature, name = line.replace('\n', '').split(',')
			features[feature] = name
		except:
			print '|%s|' % line.replace('\n', '')

consonants = {
	'name':  'Consonants',
	'x':     ['blb','lbd','dnt','alv','rfx','pla','pal','vel','uvl','lbv','phr','glt'], # place
	'y':     ['nas','stp','frc','afr','apr','lat','trl','flp','ejc','imp','clk'], # manner
	'z':     ['vls','vcd'], # voicing
	'extra': [],
}

vowels = {
	'name':  'Vowels',
	'x':     ['fnt','cnt','bck'], # backness
	'y':     ['hgh','smh','umd','mid','lmd','low'], # height
	'z':     ['unr','rnd'], # roundness
	'extra': ['vwl']
}

##### Phoneme Scheme File Reader ...

phoneme_decl = re.compile("""
	^
	\s*
	/([^/]*)/
	\s*
	\{([^\}]*)\}
	.*
	$""", re.VERBOSE)

def load_scheme(filename):
	scheme = []
	with open(filename) as f:
		for line in f:
			line = line.replace('\n', '')
			if line == '' or line.startswith('#'):
				continue

			m = phoneme_decl.match(line)
			if m:
				phon = m.group(1)
				feat = m.group(2).split(',')
				for f in feat:
					if f not in features.keys():
						raise Exception('Unrecognised phoneme feature "%s".' % f)
				scheme.append((phon, feat))
	return scheme

scheme = load_scheme(sys.argv[1])

##### HTML-Based IPA Table Generation ...

def lookup_transcription(scheme, codes):
	x = '|'.join(sorted(codes))
	for phoneme, featureset in scheme:
		y = '|'.join(sorted(featureset))
		if x == y:
			return phoneme
	return None

def to_xml(s):
	if not s:
		return '&#xA0;'
	return s.replace('&', '&amp;').replace('<', '&lt;').replace('>', '&gt;')

def print_table(info, scheme):
	print '<table width="100%%"><caption>%s</caption>' % info['name']
	print '<tr>'
	print '<td></td>'
	for x in info['x']:
		print '<td colspan="2">%s</td>' % features[x]
	print '</tr>'
	for y in info['y']:
		print '<tr>'
		print '<td>%s</td>' % features[y]
		for x in info['x']:
			for z in info['z']:
				f = [x, y, z]
				f.extend(info['extra'])
				s = to_xml(lookup_transcription(scheme, f))
				print '<td class="%s">%s</td>' % (z, s)
		print '</tr>'
	print '</table>'

print '<html>'
print '<head>'
print '<title>Phoneme Transcription Scheme</title>'
print '<style type="text/css">'
print '    .vls, .unr { text-align: left;  font-family: monospace; }'
print '    .vcd, .rnd { text-align: right; font-family: monospace; }'
print '</style>'
print '</head>'
print '<body>'
print_table(consonants, scheme)
print_table(vowels, scheme)
print '</body>'
print '</html>'

#	REFERENCES
#
#		ASCII-IPA
#			[http://www.hpl.hp.com/personal/Evan_Kirshenbaum/IPA/faq.html]
#			Evan Kirshenbaum, 1993
