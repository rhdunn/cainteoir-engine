#!/usr/bin/python

import os
import sys

from xml.dom import minidom

yes_no = {'yes': True , 'no': False, '': False}

class XmlNode:
	def __init__(self, node):
		self.node = node
		self.name = node.nodeName

	def __iter__(self):
		for child in self.node.childNodes:
			if child.nodeType == child.ELEMENT_NODE:
				yield XmlNode(child)

	def __getitem__(self, name):
		return self.node.getAttribute(name)

	def text(self):
		return ''.join(self._text(self.node))

	def _text(self, node):
		ret = []
		for child in node.childNodes:
			if child.nodeType == child.TEXT_NODE:
				ret.append(child.nodeValue)
			elif child.nodeType == child.ELEMENT_NODE:
				ret.extend(self._text(child))
		return ret


class XmlDocument(XmlNode):
	def __init__(self, filename):
		XmlNode.__init__(self, minidom.parse(filename).documentElement)


class Member:
	def __init__(self, ref):
		self.ref = ref
		self.kind = None

	def parse(self, xml, references):
		self.kind = xml['kind']

	def output(self):
		print('   (%s)' % (self.kind))

	def __str__(self):
		return '(%s)' % self.kind


class Variable(Member):
	def __init__(self, ref):
		Member.__init__(self, ref)
		self.protection = None
		self.static = None
		self.mutable = None
		self.vartype = None
		self.name = None

	def parse(self, xml, references):
		self.kind = xml['kind']
		self.protection = xml['prot']
		self.static = yes_no[xml['static']]
		self.mutable = yes_no[xml['mutable']]
		for child in xml:
			if child.name == 'type':
				self._parseType(child, references)
			elif child.name == 'name':
				self.name = child.text()

	def _parseType(self, xml, references):
		for child in xml:
			if child.name == 'ref':
				self.vartype = references.create(child)
		if not self.vartype:
			self.vartype = xml.text()

	def output(self):
		print('   %s %s %s' % (self.protection, self.vartype, self.name))


class Function(Member):
	def __init__(self, ref):
		Member.__init__(self, ref)


class BaseCompound:
	def __init__(self, base):
		self.base = base
		self.protection = None
		self.virtual = None

	def parse(self, xml, references):
		self.protection = xml['prot']
		self.virtual = xml['virt']


class Compound:
	def __init__(self, ref):
		self.ref = ref
		self.name = None
		self.kind = None
		self.protection = None
		self.parents = []
		self.members = []

	def __str__(self):
		if self.name:
			return self.name
		return '[%s]' % self.ref

	def parse(self, xml, references):
		self.kind = xml['kind']
		self.protection = xml['prot']
		for child in xml:
			if child.name == 'compoundname':
				self.name = child.text()
			elif child.name == 'basecompoundref':
				b = references.create(child)
				if b:
					self.parents.append(b)
			elif child.name == 'sectiondef':
				self._parseSectionDef(child, references)

	def _parseSectionDef(self, xml, references):
		for child in xml:
			if child.name == 'memberdef':
				m = references.create(child)
				if m:
					self.members.append(m)

	def output(self):
		print('%s %s %s' % (self.protection, self.kind, self.name))
		for p in self.parents:
			print('   : %s %s' % (p.protection, p.base))
		print('{')
		for m in self.members:
			m.output()
		print('}')
		print('')


class References:
	def __init__(self):
		self.object_types = { #     ref-type      val-type
			'basecompoundref': (BaseCompound, Compound),
			'compounddef':     (Compound,     Compound),
			'compound':        (Compound,     Compound),           # ref|@kindref
			'function':        (Function,     Function),           # memberdef|@kind
			'memberdef':       (Member,       Member),
			'member':          (Member,       Member),
			'variable':        (Variable,     Variable),           # memberdef|@kind
		}
		self.items = {}

	def create(self, xml):
		if xml.name == 'ref':
			kind = xml['kindref']
			ref  = xml['refid']
			if not ref in self.items.keys():
				ref_type, val_type = self.object_types[kind]
				obj = val_type(ref)
				self.items[ref] = obj
			return self.items[ref]

		kind = xml['kind']
		if kind in self.object_types.keys():
			ref_type, val_type = self.object_types[kind]
		else:
			if not xml.name in self.object_types.keys():
				return None
			ref_type, val_type = self.object_types[xml.name]

		ref = xml['id']
		if ref:
			obj = val_type(ref)
			if not ref in self.items.keys():
				self.items[ref] = val_type(ref)
			obj = self.items[ref]
			obj.parse(xml, self)
		else:
			ref = xml['refid']
			if not ref:
				return None
			if not ref in self.items.keys():
				self.items[ref] = val_type(ref)
			obj = ref_type(self.items[ref])
			obj.parse(xml, self)
		return obj

	def __iter__(self):
		for item in self.items.values():
			yield item


def parseDoxygenXml(element, references):
	compounds = []
	for child in element:
		compounds.append(references.create(child))
	return compounds


references = References()
compounds  = []
for filename in sys.argv[1:]:
	doc = XmlDocument(filename)
	compounds.extend(parseDoxygenXml(doc, references))

for c in compounds:
	c.output()
