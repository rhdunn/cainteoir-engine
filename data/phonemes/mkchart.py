#!/usr/bin/python

import sys
import os
import re

##### Features ...

features = {}

with open(os.path.join(sys.path[0], 'phoneme-features.csv')) as f:
	for line in f:
		try:
			feature, name = line.replace('\n', '').split(',')
			features[feature] = name
		except:
			print '|%s|' % line.replace('\n', '')

consonants = {
	'name':  'Consonants',
	'x':     ['blb','lbd','dnt','alv','pla','rfx','pal','vel','uvl','lbv','phr','glt'], # place
	'y':     ['stp','nas','trl','flp','frc','afr',['lat','frc'],'apr',['lat','apr'],'ejc','imp','clk'], # manner
	'z':     ['vls','vcd'], # voicing
	'extra': [],
}

vowels = {
	'name':  'Vowels',
	'x':     ['fnt','cnt',['cnt','rzd'],'bck'], # backness
	'y':     ['hgh','smh','umd','mid','lmd','low'], # height
	'z':     ['unr','rnd'], # roundness
	'extra': ['vwl']
}

long_vowels = {
	'name':  'Long Vowels',
	'x':     ['fnt','cnt',['cnt','rzd'],'bck'], # backness
	'y':     ['hgh','smh','umd','mid','lmd','low'], # height
	'z':     ['unr','rnd'], # roundness
	'extra': ['lng','vwl']
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

def feature_name(feature):
	if type(feature).__name__ == 'list':
		return ' '.join([features[f] for f in feature])
	return features[feature]

def feature_set(features):
	# These features provide additional annotational properties to phonemes
	# that are included in the consonant table, but do not help describe the
	# phoneme's location in the table. As a result, they are ignored when
	# comparing phonemes.
	ignored_features = ['gld','lqd','mrm']

	ret = []
	for feature in features:
		if type(feature).__name__ == 'list':
			for f in feature:
				if f not in ignored_features:
					ret.append(f)
		else:
			if feature not in ignored_features:
				ret.append(feature)
	return sorted(ret)

def lookup_transcription(scheme, codes):
	x = '|'.join(feature_set(codes))
	for phoneme, features in scheme:
		y = '|'.join(feature_set(features))
		if x == y:
			return phoneme
	return None

def to_xml(s):
	if not s:
		return '&#xA0;'
	return s.replace('&', '&amp;').replace('<', '&lt;').replace('>', '&gt;')

def print_table(info, scheme):
	print '<table width="100%%" cellspacing="0" cellpadding="0" class="phoneme"><caption>%s</caption>' % info['name']
	print '<tr>'
	print '<td>&#xA0;</td>'
	for x in info['x']:
		print '<td colspan="2">%s</td>' % feature_name(x)
	print '</tr>'
	for y in info['y']:
		print '<tr>'
		print '<td>%s</td>' % feature_name(y)
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
print '    table      { border: 1px solid black; }'
print '    td         { border: 1px solid black; padding: 0.2em; }'
print '    caption    { text-align: left; margin-top: 0.5em; margin-bottom: 0.5em; }'
print '    .vls, .unr { text-align: left;  font-family: monospace; border-right: 0; }'
print '    .vcd, .rnd { text-align: right; font-family: monospace; border-left:  0; }'
print '    .layout    { border: 0; }'
print '</style>'
print '</head>'
print '<body>'
print_table(consonants, scheme)
print '<table width="100%%" cellspacing="0" cellpadding="0" class="layout">'
print '<tr>'
print '<td width="49%" class="layout">'
print_table(vowels, scheme)
print '</td>'
print '<td width="2%" class="layout">'
print '</td>'
print '<td width="49%" class="layout">'
print_table(long_vowels, scheme)
print '</td>'
print '</tr>'
print '</table>'
print '</body>'
print '</html>'

#	REFERENCES
#
#		ASCII-IPA
#			[http://www.hpl.hp.com/personal/Evan_Kirshenbaum/IPA/faq.html]
#			Evan Kirshenbaum, 1993
