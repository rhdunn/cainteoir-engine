#!/usr/bin/python

import os
import sys

from xml.dom import minidom


##### Configuration


extensionless_links = False
rootdir = 'docs/api/html'
stylesheet = 'http://127.0.0.1/css/main.css'
breadcrumbs = [
	('/', 'Home'),
	('/cainteoir', 'Cainteoir Text-to-Speech'),
	('/cainteoir/api/engine', 'Engine API'),
]
end_year = '2014'


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

	def children(self):
		for child in self.node.childNodes:
			yield XmlNode(child)

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
		self.brief  = None

	def __str__(self):
		return self.name

	def __iter__(self):
		return
		yield

	def ancestors(self):
		ret = []
		parent = self
		while parent:
			if parent.name:
				ret.append(parent)
			parent = parent.parent
		return reversed(ret)

	def scopedname(self):
		return '::'.join([ x.name for x in self.ancestors() ])

	def signature(self):
		return '%s %s' % (self.kind, self.scopedname())


class ScopedItem(Item):
	def __init__(self, kind, name, parent):
		Item.__init__(self, kind, name, parent)
		self.items  = {}

	def __iter__(self):
		for key, item in sorted(self.items.items()):
			yield item

	def get(self, kind, name):
		if name in self.items.keys():
			ret = self.items[name]
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


##### Doxygen Object Model


class Reference:
	def __init__(self, ref, item):
		self.ref  = ref
		self.item = item

	def __str__(self):
		return self.ref

	def accept(self, visitor, kwargs):
		visitor.onReference(self, **kwargs)


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
			item.item.ref = item
		elif compound:
			item.item = compound.item.get(kind, name)
			item.item.ref = item
		else:
			raise Exception('Item %s is not a namespace, struct, class or member object' % name)
	return item


class NamedReference:
	def __init__(self, ref, name):
		self.ref = create_itemref(ref, None)
		self.name = name

	def accept(self, visitor, kwargs):
		visitor.onNamedReference(self, **kwargs)


class DocText:
	def __init__(self, text, style=None):
		self.text = text
		self.style = style

	def accept(self, visitor, kwargs):
		if not self.style:
			visitor.onText(self, **kwargs)
		elif self.style == 'bold':
			visitor.onBoldText(self, **kwargs)
		elif self.style == 'computeroutput':
			visitor.onComputerOutput(self, **kwargs)


class DocLink:
	def __init__(self, ref, name):
		self.ref = ref
		self.name = name

	def accept(self, visitor, kwargs):
		visitor.onLink(self, **kwargs)


class DocParagraph:
	def __init__(self):
		self.contents = []

	def add(self, item):
		self.contents.append(item)

	def accept(self, visitor, kwargs):
		visitor.onParagraph(self, **kwargs)


class DocBriefDescription:
	def __init__(self, para):
		self.contents = para.contents

	def accept(self, visitor, kwargs):
		visitor.onBriefDescription(self, **kwargs)


##### Doxygen XML Parser


def parseDoxygenXml_para(xml):
	para = DocParagraph()
	for child in xml.children():
		if child.name == '#text':
			para.add(DocText(child.node.nodeValue))
		elif child.name in ['bold', 'computeroutput']:
			para.add(DocText(child.node.nodeValue, child.name))
		elif child.name == 'ref':
			para.add(NamedReference(child['refid'], child.text()))
		elif child.name == 'ulink':
			para.add(DocLink(child['url'], child.text()))
		else:
			raise Exception('Unknown element %s on para' % child.name)
	return para


def parseDoxygenXml_briefdescription(xml, item):
	for child in xml:
		if child.name == 'para':
			if item.item.brief:
				print('warning: %s already has a brief description' % item.item.scopedname())
			item.item.brief = DocBriefDescription(parseDoxygenXml_para(child))
		else:
			raise Exception('Unknown element %s on briefdescription' % child.name)


def parseDoxygenXml_enumvalue(xml, compound):
	for child in xml:
		if child.name == 'name':
			member = create_item(xml['id'], 'enumvalue', child.text(), compound)
		elif child.name == 'briefdescription':
			parseDoxygenXml_briefdescription(child, member)
		elif child.name in ['detaileddescription', 'initializer']:
			pass
		else:
			raise Exception('Unknown enumvalue node : %s' % child.name)


def parseDoxygenXml_memberdef_enum(xml, compound):
	for child in xml:
		if child.name == 'name':
			member = create_item(xml['id'], xml['kind'], child.text(), compound)
		elif child.name == 'enumvalue':
			parseDoxygenXml_enumvalue(child, member)
		elif child.name == 'briefdescription':
			parseDoxygenXml_briefdescription(child, member)
		elif child.name in ['detaileddescription', 'inbodydescription', 'location']:
			pass
		else:
			raise Exception('Unknown memberdef node : %s' % child.name)


def parseDoxygenXml_memberdef_typedef(xml, compound):
	for child in xml:
		if child.name == 'name':
			member = create_item(xml['id'], xml['kind'], child.text(), compound)
		elif child.name == 'briefdescription':
			parseDoxygenXml_briefdescription(child, member)
		elif child.name in ['type', 'definition', 'argsstring', 'detaileddescription', 'inbodydescription', 'location']:
			pass
		else:
			raise Exception('Unknown memberdef node : %s' % child.name)


def parseDoxygenXml_memberdef_variable(xml, compound):
	for child in xml:
		if child.name == 'name':
			member = create_item(xml['id'], xml['kind'], child.text(), compound)
		elif child.name == 'briefdescription':
			parseDoxygenXml_briefdescription(child, member)
		elif child.name in ['type', 'definition', 'argsstring', 'detaileddescription', 'inbodydescription', 'location']:
			pass
		else:
			raise Exception('Unknown memberdef node : %s' % child.name)


def parseDoxygenXml_memberdef_function(xml, compound):
	for child in xml:
		if child.name == 'name':
			member = create_item(xml['id'], xml['kind'], child.text(), compound)
		elif child.name == 'briefdescription':
			parseDoxygenXml_briefdescription(child, member)
		elif child.name in ['type', 'definition', 'argsstring', 'param', 'templateparamlist', 'detaileddescription', 'inbodydescription', 'location', 'reimplements', 'reimplementedby']:
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
	compound = None
	for child in xml:
		if child.name == 'compoundname':
			compound = create_item(xml['id'], xml['kind'], child.text())
		elif child.name == 'sectiondef':
			parseDoxygenXml_sectiondef(child, compound)
		elif child.name == 'briefdescription':
			parseDoxygenXml_briefdescription(child, compound)
		elif child.name in ['innerclass', 'innernamespace', 'detaileddescription', 'location']:
			pass
		else:
			raise Exception('Unknown compounddef node : %s' % child.name)
	return compound


def parseDoxygenXml_compounddef_class(xml):
	compound = None
	for child in xml:
		if child.name == 'compoundname':
			compound = create_item(xml['id'], xml['kind'], child.text())
		elif child.name == 'sectiondef':
			parseDoxygenXml_sectiondef(child, compound)
		elif child.name == 'briefdescription':
			parseDoxygenXml_briefdescription(child, compound)
		elif child.name in ['includes', 'detaileddescription', 'inheritancegraph', 'collaborationgraph', 'location', 'listofallmembers', 'innerclass', 'templateparamlist', 'derivedcompoundref', 'basecompoundref']:
			pass
		else:
			raise Exception('Unknown compounddef node : %s' % child.name)
	return compound


def parseDoxygenXml(xml):
	compound = None
	for child in xml:
		if child.name == 'compounddef':
			if compound:
				raise Exception('A compound was already provided for this file.')
			if child['kind'] in ['class', 'struct']:
				compound = parseDoxygenXml_compounddef_class(child)
			elif child['kind'] == 'namespace':
				compound = parseDoxygenXml_compounddef_namespace(child)
			elif child['kind'] in ['file', 'dir', 'group', 'page']:
				pass
			else:
				raise Exception('Unknown compounddef : %s' % child['kind'])
		elif child.name == 'compound':
			pass
		else:
			raise Exception('Unknown doxygen node : %s' % child.name)
	return compound


##### Documentation Generators


def link(href):
	if extensionless_links:
		return href
	return '%s.html' % href


class HtmlPrinter:
	def __init__(self, f):
		self.f = f

	def visit(self, node, **kwargs):
		if not node:
			self.onBlankNode(**kwargs)
			return
		node.accept(self, kwargs)

	def onBlankNode(self, classname=None):
		self.f.write('&#xA0;')

	def onText(self, node):
		self.f.write(node.text)

	def onComputerOutput(self, node):
		self.f.write('<code>%s</code>' % (node.text))

	def onLink(self, node):
		self.f.write('<a href="%s">%s</a>' % (node.ref, node.name))

	def onReference(self, node):
		self.f.write('<a href="%s">%s</a>' % (link(node.ref), node.item.name))

	def onNamedReference(self, node):
		self.f.write('<a href="%s">%s</a>' % (link(node.ref), node.name))

	def onBriefDescription(self, node, classname):
		self.f.write('<blockquote class="%s">' % classname)
		map(self.visit, node.contents)
		self.f.write('</blockquote>\n')


def writeHtmlHeader(f, title, description, keywords, breadcrumbs):
	f.write('<!DOCTYPE html>\n')
	f.write('<html xmlns="http://www.w3.org/1999/xhtml" lang="en" prefix="dc: http://purl.org/dc/elements/1.1/ dct: http://purl.org/dc/terms/ doap: http://usefulinc.com/ns/doap# foaf: http://xmlns.com/foaf/0.1/ rdf: http://www.w3.org/1999/02/22-rdf-syntax-ns# rdfs: http://www.w3.org/2000/01/rdf-schema# s: http://schema.org/ v: http://rdf.data-vocabulary.org/# xsd: http://www.w3.org/2001/XMLSchema#" typeof="s:WebPage">\n')
	f.write('<head>\n')
	f.write('<meta charset="utf-8"/>\n')
	f.write('<meta name="viewport" content="width=device-width; initial-scale=1"/>\n')
	f.write('<meta name="description" content="%s"/>\n' % description)
	f.write('<meta name="keywords" content="%s"/>\n' % ', '.join(keywords))
	f.write('<meta name="robots" content="all"/>\n')
	f.write('<title property="s:name dc:title">%s</title>\n' % title)
	f.write('<link rel="stylesheet" type="text/css" href="%s"/>\n' % stylesheet)
	f.write('</head>\n')
	f.write('<body>\n')
	f.write('<header role="banner">\n')
	f.write('<div style="font-size: 1.20em;">Cainteoir</div>\n')
	f.write('<div style="font-size: 0.86em;">Technologies</div>\n')
	f.write('</header>\n')
	f.write('<nav role="navigation" class="breadcrumbs">\n')
	f.write('  <a rel="s:breadcrumbs" href="#breadcrumb0"></a>\n')
	f.write('  <ol>\n')
	for i, data in enumerate(breadcrumbs):
		href, item_title = data
		f.write('    <li id="#breadcrumb%d" about="#breadcrumb%d" typeof="v:Breadcrumb">\n' % (i, i))
		if i == (len(breadcrumbs) - 1):
			f.write('      <span property="v:title">%s</span>\n' % item_title)
		else:
			f.write('      <a rel="v:url" property="v:title" href="%s">%s</a>\n' % (link(href), item_title))
			f.write('      <a rel="v:child" href="#breadcrumb%d"></a>\n' % (i + 1))
		f.write('    </li>\n')
	f.write('  </ol>\n')
	f.write('</nav>\n')
	f.write('<div role="main">\n')
	f.write('<h1>%s</h1>\n' % title)

def writeHtmlFooter(f):
	f.write('</div>\n')
	f.write('<footer>\n')
	f.write('<p style="text-align: center;">\n')
	f.write('<a rel="license" href="http://creativecommons.org/licenses/by-sa/3.0/" style="border: 0;"><img alt="CC-BY-SA 3.0" title="Creative Commons Attribution-ShareAlike 3.0 License" src="http://i.creativecommons.org/l/by-sa/3.0/88x31.png"/></a>\n')
	f.write('<a href="http://www.w3.org/html/logo/" style="border: 0;">\n')
	f.write('<img src="http://www.w3.org/html/logo/badge/html5-badge-h-css3-semantics.png" width="80" height="31" alt="HTML5 Powered with CSS3 / Styling, and Semantics" title="HTML5 Powered with CSS3 / Styling, and Semantics"/>\n')
	f.write('</a>\n')
	f.write('<img src="http://www.w3.org/Icons/SW/Buttons/sw-rdfa-green.png" alt="RDFa 1.1" title="RDFa 1.1" height="15" width="80" style="padding-top: 8px; padding-bottom: 8px;" />\n')
	f.write('<span>\n')
	f.write('<a href="http://jigsaw.w3.org/css-validator/check/referer" style="border: 0;"><img src="http://jigsaw.w3.org/css-validator/images/vcss" alt="Valid CSS" title="Valid CSS" height="31" width="88" /></a>\n')
	f.write('</span>\n')
	f.write('</p>\n')
	f.write('<p class="copyright" property="dc:rights">Copyright &#169; 2010-%s Reece H. Dunn</p>\n' % end_year)
	f.write('<p><em>Cainteoir</em> is a registered trademark of Reece Dunn.</p>\n')
	f.write('<p><em>W3C</em> is a trademark (registered in numerous countries) of the World Wide Web Consortium; marks of W3C are registered and held by its host institutions MIT, ERCIM, and Keio.</p>\n')
	f.write('<p><em>Android</em> and <em>Google Play</em> are registered trademarks of Google Inc.</p>\n')
	f.write('<p>All trademarks are property of their respective owners.</p>\n')
	f.write('<p>This website is generated using the <a href="https://github.com/mojombo/jekyll">Jekyll</a> static site generator with the <a href="https://github.com/rhdunn/website-template">Website Template</a> layouts and plugins.</p>\n')
	f.write('</footer>\n')
	f.write('</body>\n')
	f.write('</html>\n')


def writeHtmlDocumentation(item):
	with open(os.path.join(rootdir, '%s.html' % item.ref), 'w') as f:
		title = '%s %s Reference' % (item.scopedname(), item.kind.capitalize())
		description = ''
		keywords = []
		nav = [x for x in breadcrumbs]
		for x in item.ancestors():
			try:
				nav.append((x.ref, x.name))
			except:
				nav.append(('/', x.name))
				print('%s | %s' % (item.signature(), x.signature()))

		printer = HtmlPrinter(f)

		writeHtmlHeader(f, title, description, keywords, nav)
		printer.visit(item.brief, classname='about')
		item_docs = [
			('Namespaces',   ['namespace']),
			('Classes',      ['class', 'struct']),
			('Typedefs',     ['typedef']),
			('Enumerations', ['enum']),
			('Functions',    ['function']),
			('Attributes',   ['variable']),
		]
		for title, kinds in item_docs:
			items = [ x for x in item if x.kind in kinds ]
			if len(items) > 0:
				f.write('<h2 class="memberdoc">%s</h2>\n' % title)
				for child in items:
					f.write('<pre class="memberdoc">%s ' % child.kind)
					printer.visit(child.ref)
					f.write('</pre>')
					printer.visit(child.brief, classname='memberdoc')
		writeHtmlFooter(f)


##### Main Entry Point


compounds = []
for filename in sys.argv[1:]:
	doc = XmlDocument(filename)
	compound = parseDoxygenXml(doc)
	if compound:
		compounds.append(compound.item)

for ref, item in _items.items():
	if not item.item:
		print('warning: reference %s does not have a referenced item' % ref)

if not os.path.exists(rootdir):
	os.mkdir(rootdir)
for c in compounds:
	writeHtmlDocumentation(c)
