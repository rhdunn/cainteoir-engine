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
	return kind, ret

class DocItem:
	def __init__(self, ref, kind, name, compound):
		self.ref = ref
		self.kind = kind
		self.name = name
		self.compound = compound

	def __str__(self):
		return '%s %s' % (self.kind, self.name)

class DocCompound(DocItem):
	def __init__(self, ref, kind, name):
		DocItem.__init__(self, ref, kind, name, True)
		self.members = []

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
	return doc

doc = parseDoxygenDocumentation(sys.argv[1])
for item in doc:
	if item.compound:
		print '%s' % (item)
		for member in item.members:
			print '... %s' % (member)
