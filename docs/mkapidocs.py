#!/usr/bin/python

import os
import sys

from xml.dom import minidom

yes_no = {'yes': True , 'no': False, '': False}


##### XML API


class XmlNode:
	def __init__(self, node):
		self.node = node
		self.name = node.nodeName

	def __iter__(self):
		for child in self.node.childNodes:
			if child.nodeType == child.ELEMENT_NODE:
				yield XmlNode(child)

	def __getitem__(self, name):
		ret = self.node.getAttribute(name)
		if ret == '':
			return None
		return ret

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


##### C++ Object Model

item_types = {}

class Item:
	def __init__(self, kind, name, parent):
		self.kind   = kind
		self.name   = name
		self.parent = parent

	def __str__(self):
		return self.name

	def __iter__(self):
		return
		yield

	def scopedname(self):
		if self.parent:
			return '%s::%s' % (self.parent.scopedname(), self.name)
		return self.name

	def signature(self):
		return '%s %s' % (self.kind, self.scopedname())


class ScopedItem(Item):
	def __init__(self, kind, name, parent):
		Item.__init__(self, kind, name, parent)
		self.items  = {}

	def __iter__(self):
		for key, item in self.items.items():
			yield item

	def get(self, kind, name):
		if name in self.items.keys():
			ret = self.items[name]
			if ret.kind != kind:
				ret = item_types[kind](kind, name, self)
				self.items[name] = ret
		else:
			ret = item_types[kind](kind, name, self)
			self.items[name] = ret
		return ret


class Class(ScopedItem):
	def __init__(self, kind, name, parent):
		ScopedItem.__init__(self, kind, name, parent)


class Enum(ScopedItem):
	def __init__(self, kind, name, parent):
		ScopedItem.__init__(self, kind, name, parent)


class EnumValue(Item):
	def __init__(self, kind, name, parent):
		Item.__init__(self, kind, name, parent)


class Function(ScopedItem):
	def __init__(self, kind, name, parent):
		ScopedItem.__init__(self, kind, name, parent)


class Namespace(ScopedItem):
	def __init__(self, kind, name, parent):
		ScopedItem.__init__(self, kind, name, parent)


class Struct(ScopedItem):
	def __init__(self, kind, name, parent):
		ScopedItem.__init__(self, kind, name, parent)


class Typedef(Item):
	def __init__(self, kind, name, parent):
		Item.__init__(self, kind, name, parent)


class Variable(Item):
	def __init__(self, kind, name, parent):
		Item.__init__(self, kind, name, parent)


global_namespace = ScopedItem('namespace', '', None)
item_types['class']     = Class
item_types['enum']      = Enum
item_types['enumvalue'] = EnumValue
item_types['function']  = Function
item_types['namespace'] = Namespace
item_types['struct']    = Struct
item_types['typedef']   = Typedef
item_types['variable']  = Variable


def create_scoped_item(kind, name):
	items = name.split('::')
	ns = global_namespace
	for item in items[:-1]:
		ns = ns.get('namespace', item)
	return ns.get(kind, items[-1])


def create_member_item(kind, name, compound):
	return compound.get(kind, name, Item)


##### Doxygen XML Parser


class Reference:
	def __init__(self, ref, item):
		self.ref  = ref
		self.item = item

	def __str__(self):
		return '[%s] => %s' % (self.ref, self.item)


_items = {}


def create_itemref(ref, name):
	if not ref:
		raise Exception('create_itemref: no reference for %s' % name)
	if ref in _items.keys():
		item = _items[ref]
	else:
		item = Reference(ref, None)
		_items[ref] = item
	return item


def create_item(ref, kind, name, compound=None):
	if not ref or not kind or not name:
		raise Exception('Item not fully defined')
	item = create_itemref(ref, name)
	if not item.item:
		if kind in ['namespace', 'struct', 'class']:
			item.item = create_scoped_item(kind, name)
		elif compound:
			item.item = compound.item.get(kind, name)
		else:
			raise Exception('Item %s is not a namespace, struct, class or member object' % name)
	return item


def parseDoxygenXml_enumvalue(xml, compound):
	for child in xml:
		if child.name == 'name':
			member = create_item(xml['id'], 'enumvalue', child.text(), compound)
		elif child.name in ['briefdescription', 'detaileddescription', 'initializer']:
			pass
		else:
			raise Exception('Unknown enumvalue node : %s' % child.name)


def parseDoxygenXml_memberdef_enum(xml, compound):
	for child in xml:
		if child.name == 'name':
			member = create_item(xml['id'], xml['kind'], child.text(), compound)
		elif child.name == 'enumvalue':
			parseDoxygenXml_enumvalue(child, member)
		elif child.name in ['briefdescription', 'detaileddescription', 'inbodydescription', 'location']:
			pass
		else:
			raise Exception('Unknown memberdef node : %s' % child.name)


def parseDoxygenXml_memberdef_typedef(xml, compound):
	for child in xml:
		if child.name == 'name':
			member = create_item(xml['id'], xml['kind'], child.text(), compound)
		elif child.name in ['type', 'definition', 'argsstring', 'briefdescription', 'detaileddescription', 'inbodydescription', 'location']:
			pass
		else:
			raise Exception('Unknown memberdef node : %s' % child.name)


def parseDoxygenXml_memberdef_variable(xml, compound):
	for child in xml:
		if child.name == 'name':
			member = create_item(xml['id'], xml['kind'], child.text(), compound)
		elif child.name in ['type', 'definition', 'argsstring', 'briefdescription', 'detaileddescription', 'inbodydescription', 'location']:
			pass
		else:
			raise Exception('Unknown memberdef node : %s' % child.name)


def parseDoxygenXml_memberdef_function(xml, compound):
	for child in xml:
		if child.name == 'name':
			member = create_item(xml['id'], xml['kind'], child.text(), compound)
		elif child.name in ['type', 'definition', 'argsstring', 'param', 'templateparamlist', 'briefdescription', 'detaileddescription', 'inbodydescription', 'location', 'reimplements', 'reimplementedby']:
			pass
		else:
			raise Exception('Unknown memberdef node : %s' % child.name)


def parseDoxygenXml_sectiondef(xml, compound):
	for child in xml:
		if child.name == 'memberdef':
			if child['kind'] == 'variable':
				parseDoxygenXml_memberdef_variable(child, compound)
			elif child['kind'] == 'function':
				parseDoxygenXml_memberdef_function(child, compound)
			elif child['kind'] == 'typedef':
				parseDoxygenXml_memberdef_typedef(child, compound)
			elif child['kind'] == 'enum':
				parseDoxygenXml_memberdef_enum(child, compound)
			else:
				raise Exception('Unknown memberdef : %s' % child['kind'])
		elif child.name in ['header']:
			pass
		else:
			raise Exception('Unknown sectiondef node : %s' % child.name)


def parseDoxygenXml_compounddef_namespace(xml):
	for child in xml:
		if child.name == 'compoundname':
			compound = create_item(xml['id'], xml['kind'], child.text())
		elif child.name == 'sectiondef':
			parseDoxygenXml_sectiondef(child, compound)
		elif child.name in ['innerclass', 'innernamespace', 'briefdescription', 'detaileddescription', 'location']:
			pass
		else:
			raise Exception('Unknown compounddef node : %s' % child.name)


def parseDoxygenXml_compounddef_class(xml):
	for child in xml:
		if child.name == 'compoundname':
			compound = create_item(xml['id'], xml['kind'], child.text())
		elif child.name == 'sectiondef':
			parseDoxygenXml_sectiondef(child, compound)
		elif child.name in ['includes', 'briefdescription', 'detaileddescription', 'inheritancegraph', 'collaborationgraph', 'location', 'listofallmembers', 'innerclass', 'templateparamlist', 'derivedcompoundref', 'basecompoundref']:
			pass
		else:
			raise Exception('Unknown compounddef node : %s' % child.name)


def parseDoxygenXml(xml):
	for child in xml:
		if child.name == 'compounddef':
			if child['kind'] in ['class', 'struct']:
				parseDoxygenXml_compounddef_class(child)
			elif child['kind'] == 'namespace':
				parseDoxygenXml_compounddef_namespace(child)
			elif child['kind'] in ['file', 'dir', 'group', 'page']:
				pass
			else:
				raise Exception('Unknown compounddef : %s' % child['kind'])
		elif child.name == 'compound':
			pass
		else:
			raise Exception('Unknown doxygen node : %s' % child.name)


##### Main Entry Point

def printItem(item):
	print(item.signature())
	for child in item:
		printItem(child)


compounds  = []
for filename in sys.argv[1:]:
	doc = XmlDocument(filename)
	parseDoxygenXml(doc)

for ref, item in _items.items():
	if not item.item:
		raise Exception('Reference %s does not have a referenced item' % ref)

printItem(global_namespace)
