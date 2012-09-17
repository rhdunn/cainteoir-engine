#!/usr/bin/python

import os
import sys

from xml.dom import minidom

class Node:
	def __init__(self, node):
		self.node = node
		self.name = node.nodeName

	def children(self):
		ret = []
		for child in self.node.childNodes:
			if child.nodeType == child.ELEMENT_NODE:
				ret.append(Node(child))
		return ret

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

class DocInheritance:
	def __init__(self, item, protection, virtual, name):
		self.item = item
		self.protection = protection
		self.virtual = virtual
		self.name = name

class DocItem:
	def __init__(self, ref, kind, name, compound):
		self.ref = ref
		self.kind = kind
		self.name = name
		self.compound = compound
		_, self.scope, self.shortname = parseDoxygenId(ref)
		self.protection = None
		self.shortdoc = None
		self.longdoc = []

	def __str__(self):
		return '%s %s' % (self.kind, self.name)

class DocCompound(DocItem):
	def __init__(self, ref, kind, name):
		DocItem.__init__(self, ref, kind, name, True)
		self.members = []
		self.base = []
		self.derived = []

class DocMember(DocItem):
	def __init__(self, ref, kind, name):
		DocItem.__init__(self, ref, kind, name, False)

class Documentation:
	def __init__(self):
		self.items = {}

	def create(self, ref, kind, name):
		if ref in self.items.keys():
			return self.items[ref]
		if kind == 'class':     self.items[ref] = DocCompound(ref, kind, name)
		if kind == 'define':    self.items[ref] = DocMember(ref, kind, name)
		if kind == 'dir':       self.items[ref] = DocCompound(ref, kind, name)
		if kind == 'enum':      self.items[ref] = DocMember(ref, kind, name)
		if kind == 'enumvalue': self.items[ref] = DocMember(ref, kind, name)
		if kind == 'file':      self.items[ref] = DocCompound(ref, kind, name)
		if kind == 'function':  self.items[ref] = DocMember(ref, kind, name)
		if kind == 'namespace': self.items[ref] = DocCompound(ref, kind, name)
		if kind == 'page':      self.items[ref] = DocCompound(ref, kind, name)
		if kind == 'struct':    self.items[ref] = DocCompound(ref, kind, name)
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

def parseDoxygenCompound(xmlroot, c, doc):
	xml = Document(os.path.join(xmlroot, '%s.xml' % c.ref))
	compound = xml.children()[0]

	if compound['id'] != c.ref or compound['kind'] != c.kind:
		raise Exception('Compound documentation file mismatch for %s' % c.ref)

	c.protection = compound['prot']
	for node in compound.children():
		if node.name == 'basecompoundref':
			ref = node['refid']
			if ref != '':
				c.base.append(DocInheritance(doc[ref], node['prot'], node['virt'], node.text()))
		elif node.name == 'derivedcompoundref':
			ref = node['refid']
			if ref != '':
				c.derived.append(DocInheritance(doc[ref], node['prot'], node['virt'], node.text()))
		elif node.name == 'briefdescription':
			para = node.children()
			if len(para) != 0:
				c.shortdoc = para[0].text()
				c.longdoc.append(c.shortdoc)
		elif node.name == 'detaileddescription':
			paras = node.children()
			if len(paras) != 0:
				for para in paras:
					c.longdoc.append(para.text())

def parseDoxygenDocumentation(xmlroot):
	xml = Document(os.path.join(xmlroot, 'index.xml'))
	doc = Documentation()
	for compound in xml.children():
		for node in compound.children():
			if node.name == 'name':
				c = doc.create(compound['refid'], compound['kind'], node.text())
			elif node.name == 'member':
				for membernode in node.children():
					if membernode.name == 'name':
						membername = membernode.text()
						m = doc.create(node['refid'], node['kind'], membernode.text())
						c.members.append(m)

	for c in doc:
		if c.compound:
			parseDoxygenCompound(xmlroot, c, doc)
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
			if item.shortdoc:
				if len(item.longdoc) != 1:
					f.write('<blockquote>%s <a href="#detailed_description">More...</a></blockquote>' % item.shortdoc)
				else:
					f.write('<blockquote>%s</blockquote>' % item.shortdoc)
			if len(item.base) != 0:
				f.write('<h2 id="base">Inherited From</h2>\n')
				f.write('<ol>\n')
				for base in item.base:
					f.write('<li><a href="%s.html">%s</a></li>' % (base.item.ref, base.name))
				f.write('</ol>\n')
			if len(item.derived) != 0:
				f.write('<h2 id="derived">Inherited By</h2>\n')
				f.write('<ol>\n')
				for derived in item.derived:
					f.write('<li><a href="%s.html">%s</a></li>' % (derived.item.ref, derived.name))
				f.write('</ol>\n')
			if len(item.longdoc) != 1:
				f.write('<h2 id="detailed_description">Detailed Description</h2>\n')
				f.write('<blockquote>\n')
				for para in item.longdoc:
					f.write('<p>%s</p>' % para)
				f.write('</blockquote>\n')
