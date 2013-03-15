#!/usr/bin/python

import os
import sys

status_values = {
	'no':  ('failure', 'No',  None),
	'na':  ('na',      'N/A', None),
	'ed':  ('w3c-ed',  'ED',  'Editor\'s Draft'),
	'wd':  ('w3c-wd',  'WD',  'Working Draft'),
	'lc':  ('w3c-lc',  'LC',  'Last Call'),
	'cr':  ('w3c-cr',  'CR',  'Candidate Recommendation'),
	'pr':  ('w3c-pr',  'PR',  'Proposed Recommendation'),
	'rec': ('w3c-rec', 'REC', 'Recommendation'),
}

class StringFormat:
	def __init__(self, title, statusref=None, urlref=None):
		self.title = title
		self.implementation = False
		self.statusref = statusref
		self.urlref = urlref

	def parse(self, values):
		return values[0].replace('\\@', '@').replace('\\#', '#').replace('\\[', '[')

	def html(self, data, ref):
		if self.urlref and self.urlref in data.keys() and data[self.urlref] != '':
			value = '<a href="%s">%s</a>' % (data[self.urlref], data[ref])
		else:
			value = data[ref]
		if self.statusref in data.keys():
			status, _, _ = data[self.statusref]
			return '\t\t<td class="%s">%s</td>\n' % (status, value)
		return '\t\t<td>%s</td>\n' % value

class CommentFormat:
	def __init__(self, title):
		self.title = title
		self.implementation = False

	def parse(self, values):
		value = ','.join(values)
		return value.replace('<', '&lt;').replace('>', '&gt;')

	def html(self, data, ref):
		return '\t\t<td>%s</td>\n' % data[ref]

class UrlFormat:
	def __init__(self, title):
		self.title = title
		self.implementation = False

	def parse(self, values):
		return values[0].replace('.csv', '.html')

	def html(self, data, ref):
		return '\t\t<td>%s</td>\n' % data[ref]

class StatusFormat:
	def __init__(self, title, implementation=False):
		self.title = title
		self.implementation = implementation

	def parse(self, values):
		if values[0] in status_values.keys():
			return status_values[values[0]]
		if values[0].startswith('*'):
			return ('inprogress', values[0].replace('*', ''), None)
		return ('success', values[0], None)

	def html(self, data, ref):
		status, label, title = data[ref]
		if title:
			return '\t\t<td class="%s" title="%s">%s</td>\n' % (status, title, label)
		return '\t\t<td class="%s">%s</td>\n' % (status, label)

class StatusSetFormat:
	def __init__(self, title):
		self.title = title
		self.implementation = False

	def parse(self, values):
		statusfmt = StatusFormat(None)
		ret = []
		for value in values:
			x, impl = value.split('/')
			if impl in ['no', 'na']:
				label = x
				status, _, _ = statusfmt.parse([impl])
			else:
				status, label, title = statusfmt.parse([x])
			ret.append((status, label, impl))
		return ret

	def html(self, data, ref):
		ret = []
		for status, label, impl in data[ref]:
			ret.append('\t\t<td class="%s" style="width: 10%%;" title="Supported since %s">%s</td>\n' % (status, impl, label))
		return ''.join(ret)

properties = {
	'category': StringFormat(None),
	'comments': CommentFormat('Comments'),
	'implemented': StatusFormat('Implemented', implementation=True),
	'item': StringFormat('Name'),
	'model': StatusFormat('Model', implementation=True),
	'parsing': StatusFormat('Parsing', implementation=True),
	'rdf': StatusFormat('Metadata', implementation=True),
	'section': StringFormat('Section'),
	'spec': StringFormat('Specification', urlref='url'),
	'status': StatusFormat(None),
	'tests': StatusFormat('Tests'),
	'title': StringFormat('Title', statusref='status', urlref='url'),
	'toc': StatusFormat('Table of Content', implementation=True),
	'tts': StatusFormat('Text', implementation=True),
	'url': UrlFormat(None),
	'version': StringFormat('Version'),
	'versions': StatusSetFormat('Versions'),
}

types = {
	'css-specs': ['spec'],
	'css-index': ['title', 'model', 'parsing', 'tests', 'spec', 'url'],
	'css-spec': ['section', 'category', 'title', 'url', 'model', 'parsing', 'tests', 'comments'],
	'format': ['title', 'version', 'url', 'status', 'tts', 'rdf', 'toc', 'comments'],
	'formats': [ 'title', 'url', 'versions' ],
	'spec': ['section', 'title', 'url', 'implemented', 'tests', 'comments'],
	'standard': ['version', 'url', 'implemented', 'comments'],
}

categories = {
	'document': 'Document Format',
	'metadata': 'Metadata Export',
}

css_categories = [
	('syntax', 'Syntax'),
	('unit', 'Units'),
	('at-rule', 'At Rules'),
	('property', 'Properties'),
	('selector', 'Selectors'),
	('function', 'Functions'),
	('counter', 'Counters'),
	('color', 'Colours'),
]

def parse_csv(filename):
	ref = filename.replace('.csv', '')
	section = []
	section_name = 'Implementation Status'
	data = { "support": [], 'category': 'document', 'items': 0, 'success': 0, 'inprogress': 0 }
	for item, _, _ in status_values.values():
		data[item] = 0
	with open(filename) as f:
		for line in f:
			line = line.replace('\n', '')
			if line == '' or line.startswith('#'): # comment or blank line
				pass
			elif line.startswith('@'):
				try:
					key, value = line.replace('@', '').split(',')
					if key == 'references':
						if value == '':
							data[key] = []
						else:
							data[key] = value.split(' ')
					else:
						data[key] = value
				except ValueError:
					raise Exception('line "%s" contains too many \',\'s' % line)
			elif line.startswith('['):
				if len(section) != 0:
					data['support'].append((section_name, section))
				section_name = line[1:-1]
				section = []
			else:
				try:
					s = line.split(',')
					props = {}
					for prop in types[data['type']]:
						p = properties[prop]
						props[prop] = p.parse(s)
						if p.implementation:
							istatus, _, _ = props[prop]
							data[istatus] = data[istatus] + 1
							data['items'] = data['items'] + 1
						s = s[1:]
					section.append(props)
				except ValueError:
					raise Exception('line "%s" contains too many \',\'s' % line)
	if len(section) != 0:
		data['support'].append((section_name, section))
	return ref, data

def refname(name):
	return name.lower().replace(' ', '_')

specs = {}
for csv in os.listdir('.'):
	if csv.endswith('.csv'):
		ref, data = parse_csv(csv)
		specs[ref] = data

for ref, spec in specs.items():
	if spec['type'] == 'css-specs':
		spec['type'] = 'css-index'
		spec['items'] = 0
		spec['success'] = 0
		spec['inprogress'] = 0
		for item, _, _ in status_values.values():
			spec[item] = 0
		support = spec['support'][0][1]
		spec['support'] = []
		data = {}
		for key, title in css_categories:
			data[key] = {}
		for cssspec in support:
			css = specs[cssspec['spec'].replace('.csv', '')]
			for title, items in css['support']:
				for item in items:
					if item['category'] in data.keys():
						props = data[item['category']][item['title']] = {
							'title': item['title'],
							'model': item['model'],
							'parsing': item['parsing'],
							'tests': item['tests'],
							'spec': '%s Level %s' % (css['name'], css['version']),
							'url': cssspec['spec'].replace('.csv', '.html')
						}
						for prop in props.keys():
							p = properties[prop]
							if p.implementation:
								istatus, _, _ = props[prop]
								spec[istatus] = spec[istatus] + 1
								spec['items'] = spec['items'] + 1
					elif item['category'] != '':
						raise Exception('Unsupported CSS category: %s' % item['category'])
		for key, title in css_categories:
			items = [v for k, v in sorted(data[key].items())]
			spec['support'].append((title, items))

for ref, spec in specs.items():
	print 'generating %s.html ...' % ref
	with open('%s.html' % ref, 'w') as f:
		f.write('---\n')
		f.write('layout: rdfa\n')
		if spec['type'] == 'spec':
			title = '%s %s %s Support' % (spec['name'], spec['version'], categories[spec['category']])
			f.write('title: %s\n' % title)
			f.write('description: The state of %s %s specification implementation in the Cainteoir Text-to-Speech program.\n' % (spec['name'], spec['version']))
		elif spec['type'] == 'css-spec':
			title = '%s %s Support' % (spec['name'], spec['version'])
			f.write('title: %s\n' % title)
			f.write('description: The state of %s %s specification implementation in the Cainteoir Text-to-Speech program.\n' % (spec['name'], spec['version']))
		else:
			if spec['name'] == categories[spec['category']]:
				title = '%s Support' % spec['name']
			else:
				title = '%s %s Support' % (spec['name'], categories[spec['category']])
			f.write('title: %s\n' % title)
			f.write('description: The state of %s implementation in the Cainteoir Text-to-Speech program.\n' % spec['name'])
		f.write('keywords: text to speech, tts, cainteoir, %s\n' % spec['name'].lower())
		f.write('nav:\n')
		f.write('  - { title: Home , url: / }\n')
		f.write('  - { title: Cainteoir , url: ../index.html }\n')
		if ref in categories.keys():
			f.write('  - { title: %s Support }\n' % categories[ref])
		else:
			f.write('  - { title: %s Support , url: %s.html }\n' % (categories[spec['category']], spec['category']))
			if spec['version'] == '':
				f.write('  - { title: %s }\n' % spec['name'])
			elif 'parent' in spec.keys():
				f.write('  - { title: %s , url: %s.html }\n' % (specs[spec['parent']]['name'], spec['parent']))
				f.write('  - { title: %s }\n' % spec['version'])
			else:
				f.write('  - { title: %s , url: %s.html }\n' % (spec['name'], spec['name'].lower().replace('/', '')))
				f.write('  - { title: %s }\n' % spec['version'])
		f.write('sidebar:\n')
		for section, data in spec['support']:
			f.write('  - { title: %s , url: "#%s" }\n' % (section, refname(section)))
		if 'references' in spec.keys() and len(spec['references']) != 0:
			f.write('  - { title: References , url: "#references" }\n')
		if 'redirects' in spec.keys() and len(spec['redirects']) != 0:
			f.write('redirects: [%s]\n' % ' , '.join(spec['redirects'].split()))
		f.write('---\n')
		f.write('<h1>%s</h1>\n' % title)
		if spec['type'] != 'formats':
			base = spec['items'] - spec['na']
			completed = 0
			if base != 0:
				completed = int((float(spec['success']) / base) * 100)
			if completed == 0:
				f.write('<div style="border: 1px solid #ddd;" class="na">%d%%</div>' % completed)
			else:
				f.write('<div style="border: 1px solid #ddd; padding: 0;" class="na">')
				f.write('<div style="border: 1px solid #080; width: %d%%;" class="success">%d%%</div>' % (completed, completed))
				f.write('</div>')
			f.write('<table style="width: 100%;">\n')
			columns = [c for c in types[spec['type']] if properties[c].title]
			for section, data in spec['support']:
				f.write('\t<tr><td colspan="%d"><h2 id="%s">%s</h2></td></tr>\n' % (len(columns), refname(section), section))
				f.write('\t<tr>\n')
				for column in columns:
					title = properties[column].title
					f.write('\t\t<th>%s</th>\n' % title)
				f.write('\t</tr>\n')
				for row in data:
					f.write('\t<tr>\n')
					for column in columns:
						prop = properties[column]
						f.write(prop.html(row, column))
					f.write('\t</tr>\n')
			f.write('</table>\n')
		else:
			f.write('<table style="width: 100%;">\n')
			for i in range(0, 9):
				f.write('\t\t<col width="10%"/>\n')
			for section, data in spec['support']:
				for row in data:
					f.write('\t<tr>\n')
					for column in types[spec['type']]:
						prop = properties[column]
						if prop.title:
							f.write(prop.html(row, column))
					f.write('\t</tr>\n')
			f.write('</table>\n')
		if 'references' in spec.keys() and len(spec['references']) != 0:
			f.write('<h2 id="references">References</h2>\n')
			f.write('<ol class="references">\n')
			for ref in spec['references']:
				with open('%s.xml' % ref) as r:
					f.write(r.read())
			f.write('</ol>\n')
