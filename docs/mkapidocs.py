#!/usr/bin/python

import os
import sys

from xml.dom import minidom

class Node:
	def __init__(self, node):
		self.node = node
		self.name = node.nodeName

	def children(self):
		for child in self.node.childNodes:
			if child.nodeType == child.ELEMENT_NODE:
				yield Node(child)

	def text(self):
		return self.node.childNodes[0].nodeValue

	def __getitem__(self, name):
		return self.node.getAttribute(name)

class Document(Node):
	def __init__(self, filename):
		Node.__init__(self, minidom.parse(filename).documentElement)

def parseDoxygenId(ref):
	kind = None
	for kindname in [u'struct', u'class', u'dir', u'namespace']:
		if ref.startswith(kindname):
			ref = ref[len(kindname):]
			kind = kindname
	if not kind:
		if u'_8' in ref:
			kind = u'file'
		elif ref in [u'deprecated', u'todo']:
			kind = u'page'
	if not kind:
		raise Exception('Unable to determine the kind for id "%s"' % ref)
	ret = []
	for item in ref.split(u'_1'):
		if item == '':
			continue
		for match, replace in [(u'__', u'_')]:
			item = item.replace(match, replace)
		ret.append(item)
	return kind, ret[:-1], ret[-1]

class DocItem:
	def __init__(self, ref, kind, name, compound):
		self.ref = ref
		self.kind = kind
		self.name = name
		self.compound = compound
		_, self.scope, self.shortname = parseDoxygenId(ref)

	def __str__(self):
		return '%s %s' % (self.kind, self.name)

class DocCompound(DocItem):
	def __init__(self, ref, kind, name):
		DocItem.__init__(self, ref, kind, name, True)
		self.members = []

class DocClass(DocCompound):
	def __init__(self, ref, kind, name):
		DocCompound.__init__(self, ref, kind, name)
		self.protection = None
		self.base = []

	def __str__(self):
		return '%s %s %s' % (self.protection, self.kind, self.name)

class DocMember(DocItem):
	def __init__(self, ref, kind, name):
		DocItem.__init__(self, ref, kind, name, False)

class Documentation:
	def __init__(self):
		self.items = {}

	def create(self, ref, kind, name):
		if ref in self.items.keys():
			return self.items[ref]
		if kind == 'class':     self.items[ref] = DocClass(ref, kind, name)
		if kind == 'define':    self.items[ref] = DocMember(ref, kind, name)
		if kind == 'dir':       self.items[ref] = DocCompound(ref, kind, name)
		if kind == 'enum':      self.items[ref] = DocMember(ref, kind, name)
		if kind == 'enumvalue': self.items[ref] = DocMember(ref, kind, name)
		if kind == 'file':      self.items[ref] = DocCompound(ref, kind, name)
		if kind == 'function':  self.items[ref] = DocMember(ref, kind, name)
		if kind == 'namespace': self.items[ref] = DocCompound(ref, kind, name)
		if kind == 'page':      self.items[ref] = DocCompound(ref, kind, name)
		if kind == 'struct':    self.items[ref] = DocClass(ref, kind, name)
		if kind == 'typedef':   self.items[ref] = DocMember(ref, kind, name)
		if kind == 'variable':  self.items[ref] = DocMember(ref, kind, name)
		if ref not in self.items.keys():
			raise Exception('Unsupported kind "%s"' % kind)
		return self.items[ref]

	def __iter__(self):
		for item in self.items.values():
			yield item

	def __getitem__(self, ref):
		return self.items[ref]

def parseDoxygenClass(xmlroot, c, doc):
	xml = Document(os.path.join(xmlroot, '%s.xml' % c.ref))
	compound = list(xml.children())[0]

	ref = compound['id']
	kind = compound['kind']
	protection = compound['prot']

	if ref != c.ref or kind != c.kind:
		raise Exception('Class documentation file mismatch for %s' % c.ref)

	c.protection = protection
	for node in compound.children():
		if node.name == 'basecompoundref':
			baseref = node['refid']
			if baseref != '':
				c.base.append(doc[baseref])

def parseDoxygenDocumentation(xmlroot):
	xml = Document(os.path.join(xmlroot, 'index.xml'))
	doc = Documentation()
	for compound in xml.children():
		ref = compound['refid']
		kind = compound['kind']

		for node in compound.children():
			if node.name == 'name':
				name = node.text()
				c = doc.create(ref, kind, name)
			elif node.name == 'member':
				memberref = node['refid']
				memberkind = node['kind']

				for membernode in node.children():
					if membernode.name == 'name':
						membername = membernode.text()
						m = doc.create(memberref, memberkind, membername)
						c.members.append(m)

	for c in doc:
		if c.kind in ['struct', 'class']:
			parseDoxygenClass(xmlroot, c, doc)
		scope = []
		namespace = []
		for ns in c.scope:
			scope.append(ns)
			for kind in ['namespace', 'class', 'struct']:
				scopename = '%s%s' % (kind, '_1_1'.join([x.replace('_', '__') for x in scope]))
				try:
					value = doc[scopename]
					namespace.append(value)
				except KeyError:
					pass
		c.scope = namespace

	return doc

docroot = sys.argv[2]
doc = parseDoxygenDocumentation(sys.argv[1])
for item in doc:
	if item.kind in ['struct', 'class', 'namespace']:
		print 'writing %s ...' % item.ref
		with open(os.path.join(docroot, '%s.html' % item.ref), 'w') as f:
			title = '%s %s Documentation' % (item.name, item.kind.capitalize())
			f.write('---\n')
			f.write('layout: rdfa\n')
			f.write('title: Cainteoir Text-to-Speech API &mdash; %s\n' % title)
			f.write('nav:\n')
			f.write('  - { title: Home , url: ../index.html }\n')
			f.write('  - { title: API , url: index.html }\n')
			for s in item.scope:
				f.write('  - { title: "%s" , url: %s.html }\n' % (s.shortname, s.ref))
			f.write('  - { title: "%s" }\n' % item.shortname)
			f.write('---\n')
			f.write('<h1>%s</h1>\n' % title)
