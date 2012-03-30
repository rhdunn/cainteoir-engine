#!/usr/bin/python

import sys
import os
import re

features = {
	'afr': 'affricate',
	'alv': 'alveolar',
	'apr': 'approximant',
	'asp': 'aspirated',
	'bck': 'back',
	'blb': 'bilabial',
	'clk': 'click',
	'cnt': 'center',
	'dnt': 'dental',
	'ejc': 'ejective',
	'flp': 'flap',
	'fnt': 'front',
	'frc': 'fricative',
	'fzd': 'pharyngealized',
	'gld': 'glide',
	'glt': 'glottal',
	'hgh': 'high',
	'imp': 'implosive',
	'lat': 'lateral',
	'lbd': 'labio-dental',
	'lbv': 'labio-velar',
	'lmd': 'lower-mid',
	'lng': 'long',
	'low': 'low',
	'lqd': 'liquid',
	'lzd': 'labialized',
	'mid': 'mid',
	'mrm': 'murmured',
	'nas': 'nasal',
	'nzd': 'nasalized',
	'pal': 'palatal',
	'phr': 'pharyngeal',
	'pla': 'palato-alveolar',
	'pzd': 'palatalized',
	'rfx': 'retroflex',
	'rnd': 'rounded',
	'rzd': 'rhoticized',
	'smh': 'semi-high',
	'smv': 'semi-vowel',
	'syl': 'syllabic',
	'stp': 'stop',
	'trl': 'trill',
	'umd': 'upper-mid',
	'unr': 'unrounded',
	'uvl': 'uvular',
	'vcd': 'voiced',
	'vel': 'velar',
	'vls': 'voiceless',
	'vwl': 'vowel',
	'vzd': 'velarized',
}

annotations = [ # extra features, properties and modifiers
	'asp',
	'fzd',
	'gld',
	'lqd',
	'lzd',
	'mrm',
	'nzd',
	'pzd',
	'smv',
	'unx',
	'vzd',
]

aspects = [ # change the pronunciation, so should not be annotations ...
	'lng',
	'rzd',
	'syl',
]

##### Consonant Table Features ...

place_of_articulation = [
	'blb',
	'lbd',
	'dnt',
	'alv',
	'rfx',
	'pla',
	'pal',
	'vel',
	'uvl',
	'lbv',
	'phr',
	'glt',
]

manner_of_articulation = [
	'nas',
	'stp',
	'frc',
	'afr',
	'apr',
	'lat',
	'trl',
	'flp',
	'ejc',
	'imp',
	'clk',
]

voicing = [
	'vls',
	'vcd',
]

consonants = {
	'name':  'Consonants',
	'x':     place_of_articulation,
	'y':     manner_of_articulation,
	'z':     voicing,
	'extra': [],
}

##### Vowel Table Features ...

roundness = [
	'unr',
	'rnd',
]

vowel_backness = [
	'fnt',
	'cnt',
	'bck',
]

vowel_height = [
	'hgh',
	'smh',
	'umd',
	'mid',
	'lmd',
	'low',
]

vowels = {
	'name':  'Vowels',
	'x':     vowel_backness,
	'y':     vowel_height,
	'z':     roundness,
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
	x = '|'.join(sorted([f for f in codes if not f in annotations]))
	for phoneme, featureset in scheme:
		y = '|'.join(sorted([f for f in featureset if not f in annotations]))
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
