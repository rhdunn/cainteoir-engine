#!/usr/bin/python

import os
import sys

def status(value):
	if value == 'no':
		return ('failure', 'No')
	if value == 'na':
		return ('na', 'N/A')
	if value.startswith('*'):
		return ('inprogress', value.replace('*', ''))
	return ('success', value)

def url(value):
	if value.endswith('.csv'):
		return value.replace('.csv', '.html')
	return value

def parse_csv(filename):
	ref  = filename.replace('.csv', '')
	data = { "support": [] }
	with open(filename) as f:
		for line in f:
			line = line.replace('\n', '')
			if line == '' or line.startswith('#'): # comment or blank line
				pass
			elif line.startswith('@'):
				key, value = line.replace('@', '').split(',')
				if key == 'references':
					data[key] = value.split(' ')
				else:
					data[key] = value
			else:
				s = line.split(',')
				if data['type'] == 'spec':
					data['support'].append({
						"section": s[0],
						"title": s[1],
						"url": url(s[2]),
						"implemented": status(s[3]),
						"tests": status(s[4]),
						"comments": s[5]
					})
				elif data['type'] == 'standard':
					data['support'].append({
						"version": s[0],
						"url": url(s[1]),
						"implemented": status(s[2]),
						"comments": s[3]
					})
	return ref, data

def print_url(data, ref):
	if data['url'] == '':
		f.write('\t\t<td>%s</td>\n' % data[ref])
	else:
		f.write('\t\t<td><a href="%s">%s</a></td>\n' % (data['url'], data[ref]))

def print_status(data, ref):
	status, label = data[ref]
	f.write('\t\t<td class="%s">%s</td>\n' % (status, label))

specs = {}
for csv in os.listdir('.'):
	if csv.endswith('.csv'):
		ref, data = parse_csv(csv)
		specs[ref] = data

for ref, spec in specs.items():
	with open('%s.html' % ref, 'w') as f:
		f.write('---\n')
		f.write('layout: rdfa\n')
		if spec['type'] == 'spec':
			f.write('title: %s %s Specification Support &mdash; Cainteoir Text-to-Speech\n' % (spec['name'], spec['version']))
			f.write('description: The state of %s %s specification implementation in the Cainteoir Text-to-Speech program.\n' % (spec['name'], spec['version']))
		else:
			f.write('title: %s Support &mdash; Cainteoir Text-to-Speech\n' % spec['name'])
			f.write('description: The state of %s implementation in the Cainteoir Text-to-Speech program.\n' % spec['name'])
		f.write('keywords: text to speech, tts, cainteoir, %s\n' % spec['name'].lower())
		f.write('---\n')
		f.write('<div class="nav">\n')
		f.write('\t<a href="index.html">Home</a>\n')
		f.write('\t&raquo;\n')
		f.write('\t<a href="listen.html">Document Format Support</a>\n')
		f.write('\t&raquo;\n')
		if spec['type'] == 'spec':
			f.write('\t<a href="%s.html">%s</a>\n' % (spec['name'].lower(), spec['name']))
			f.write('\t&raquo;\n')
			f.write('\t<span>%s</span>\n' % spec['version'])
		else:
			f.write('\t<span>%s</span>\n' % spec['name'])
		f.write('</div>\n')
		f.write('<ol class="toc">\n')
		f.write('\t<li><a href="#status">Implementation Status</a></li>\n')
		f.write('\t<li><a href="#references">References</a></li>\n')
		f.write('</ol>\n')
		f.write('<h2 id="status">Implementation Status</h2>\n')
		f.write('<table style="width: 100%;">\n')
		f.write('\t<tr>\n')
		if spec['type'] == 'spec':
			f.write('\t\t<th width= "5%">Section</th>\n')
			f.write('\t\t<th width="25%">Title</th>\n')
			f.write('\t\t<th width="10%">Implemented</th>\n')
			f.write('\t\t<th width="10%">Tests</th>\n')
			f.write('\t\t<th width="50%">Comments</th>\n')
		else:
			f.write('\t\t<th width= "5%">Version</th>\n')
			f.write('\t\t<th width="10%">Implemented</th>\n')
			f.write('\t\t<th width="85%">Comments</th>\n')
		f.write('\t</tr>\n')
		for data in spec['support']:
			f.write('\t<tr>\n')
			if spec['type'] == 'spec':
				f.write('\t\t<td>%s</td>\n' % data['section'])
				print_url(data, 'title')
				print_status(data, 'implemented')
				print_status(data, 'tests')
				f.write('\t\t<td>%s</td>\n' % data['comments'])
			else:
				print_url(data, 'version')
				print_status(data, 'implemented')
				f.write('\t\t<td>%s</td>\n' % data['comments'])
			f.write('\t</tr>\n')
		f.write('</table>\n')
		f.write('<h2 id="references">References</h2>\n')
		f.write('<ol class="references">\n')
		for ref in spec['references']:
			with open('%s.xml' % ref) as r:
				f.write(r.read())
		f.write('</ol>\n')
