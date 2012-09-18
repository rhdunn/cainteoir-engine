#!/usr/bin/python

import os
import sys

from xml.dom import minidom

def xmlstr(x):
	return x.replace('&', '&amp;').replace('<', '&lt;')

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
		return ''.join(self._text(self.node))

	def _text(self, node):
		ret = []
		for child in node.childNodes:
			if child.nodeType == child.TEXT_NODE:
				ret.append(xmlstr(child.nodeValue))
			elif child.nodeType == child.ELEMENT_NODE:
				ret.extend(self._text(child))
		return ret

	def __getitem__(self, name):
		return self.node.getAttribute(name)

class Document(Node):
	def __init__(self, filename):
		Node.__init__(self, minidom.parse(filename).documentElement)

class KindInfo:
	def __init__(self, single, plural, category=None):
		self.single = single
		self.plural = plural
		self.category = category

kinds = {
	# kind               # single             # plural
	'attrib':    KindInfo( 'Attribute',         'Attributes'         ),
	'class':     KindInfo( 'Class',             'Classes'            ),
	'define':    KindInfo( 'Definition',        'Definitions'        ),
	'dir':       KindInfo( 'Directory',         'Directories'        ),
	'enum':      KindInfo( 'Enumeration',       'Enumerations'       ),
	'enumvalue': KindInfo( 'Enumeration Value', 'Enumeration Values' ),
	'file':      KindInfo( 'File',              'Files'              ),
	'func':      KindInfo( 'Function',          'Functions'          ),
	'function':  KindInfo( 'Function',          'Functions'          ),
	'method':    KindInfo( 'Member Function',   'Member Functions'   ),
	'namespace': KindInfo( 'Namespace',         'Namespaces'         ),
	'page':      KindInfo( 'Page',              'Pages'              ),
	'struct':    KindInfo( 'Structure',         'Structures'         ),
	'type':      KindInfo( 'Type',              'Types'              ),
	'typedef':   KindInfo( 'Type',              'Types'              ),
	'var':       KindInfo( 'Variable',          'Variables'          ),
	'variable':  KindInfo( 'Variable',          'Variables'          ),
}

protection_kinds = {
	# kind               # single             # plural
	'public':    KindInfo( 'Public',            'Public'),
	'protected': KindInfo( 'Protected',         'Protected'),
	'private':   KindInfo( 'Private',           'Private'),
}

section_kinds = { 'user-defined': KindInfo(None, 'User Defined') }
for kindname, kind in kinds.items():
	section_kinds[kindname] = kind
	for protname, prot in protection_kinds.items():
		section_kinds['%s-%s' % (protname, kindname)] = KindInfo(
			'%s %s' % (prot.single, kind.single),
			'%s %s' % (prot.plural, kind.plural))
		section_kinds['%s-static-%s' % (protname, kindname)] = KindInfo(
			'%s Static %s' % (prot.single, kind.single),
			'%s Static %s' % (prot.plural, kind.plural))

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

doxygen_formatting_tags = {
	'emphasis':       ('inline', 'em'),
	'itemizedlist':   ('block',  'ul'),
	'listitem':       ('block',  'li'),
	'para':           ('block',  'p'),
	'programlisting': ('block',  'pre'),
}

class DoxyString:
	def __init__(self, node, doc):
		self.items = []
		self._parseNode(node, doc)

	def _parseNode(self, node, doc):
		for child in node.node.childNodes:
			if child.nodeType == child.TEXT_NODE:
				self.items.append(xmlstr(child.nodeValue))
			elif child.nodeType == child.ELEMENT_NODE:
				if child.nodeName == 'ref':
					self.items.append(doc[child.getAttribute('refid')])
				elif child.nodeName in doxygen_formatting_tags.keys():
					kind, tag = doxygen_formatting_tags[child.nodeName]
					if kind == 'inline':
						self.items.append('<%s>%s</%s>' % (tag, Node(child).text(), tag))
					elif kind == 'block':
						self.items.append('<%s>' % tag)
						self._parseNode(Node(child), doc)
						self.items.append('</%s>' % tag)
				elif child.nodeName in ['codeline', 'highlight', 'ulink']:
					self._parseNode(Node(child), doc)
				elif child.nodeName in ['sp', 'simplesectsep']:
					self.items.append(' ')
				elif child.nodeName in ['parameterlist', 'simplesect', 'xrefsect']:
					pass
				else:
					raise Exception('Unsupported element %s' % child.nodeName)

	def __str__(self):
		return ''.join([str(x) for x in self.items])

class DocInheritance:
	def __init__(self, item, protection, virtual, name):
		self.item = item
		self.protection = protection
		self.virtual = virtual
		self.name = name

	def __str__(self):
		return '<a href="%s.html">%s</a>' % (self.item.ref, self.name)

class DocSection:
	def __init__(self, kind):
		self.kind = kind
		self.name = section_kinds[kind].plural
		self.members = []

class DocItem:
	def __init__(self, ref, kind, name, compound):
		self.kind = kind
		self.shortdoc = None
		self.longdoc = []
		self.docsections = []
		self.ref = ref
		self.name = name
		self.compound = compound
		_, self.scope, self.shortname = parseDoxygenId(ref)
		self.protection = 'public'

class DocCompound(DocItem):
	def __init__(self, ref, kind, name):
		DocItem.__init__(self, ref, kind, name, True)
		self.members = []
		self.base = []
		self.derived = []

	def __str__(self):
		return '<a href="%s.html">%s</a>' % (self.ref, self.name)

class DocMember(DocItem):
	def __init__(self, ref, kind, name):
		DocItem.__init__(self, ref, kind, name, False)
		self.typeof = ''
		self.argstr = ''

	def __str__(self):
		return '<a href="#%s">%s</a>' % (self.ref, self.name)

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

def parseDoxygenMember(member, m, doc):
	m.protection = member['prot']
	for node in member.children():
		if node.name == 'briefdescription':
			para = node.children()
			if len(para) != 0:
				m.shortdoc = DoxyString(para[0], doc)
				m.longdoc.append(m.shortdoc)
		elif node.name == 'detaileddescription':
			paras = node.children()
			if len(paras) != 0:
				for para in paras:
					for child in para.children():
						if child.name == 'parameterlist':
							for param in child.children():
								for item in param.children():
									if item.name == 'parameternamelist':
										title = item.children()[0].text()
									if item.name == 'parameterdescription':
										desc = item
								m.docsections.append((title, DoxyString(desc, doc)))
						elif child.name == 'xrefsect':
							for xref in child.children():
								if xref.name == 'xreftitle':
									title = xref.text()
								if xref.name == 'xrefdescription':
									desc = xref
							m.docsections.append((title, DoxyString(desc, doc)))
						elif child.name == 'simplesect':
							if child['kind'] == 'return':
								m.docsections.append(('Returns', DoxyString(child, doc)))
							elif child['kind'] == 'see':
								m.docsections.append(('See', DoxyString(child, doc)))
							else:
								raise Exception('Unsupported simplesect (kind=%s)' % child['kind'])
					m.longdoc.append(DoxyString(para, doc))
		elif node.name == 'type':
			m.typeof = DoxyString(node, doc)
		elif node.name == 'argsstring':
			m.argstr = DoxyString(node, doc)

def parseDoxygenCompound(xmlroot, c, doc):
	print 'parsing %s.xml' % c.ref
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
					t = para.text()
					if t:
						c.longdoc.append(t)
		elif node.name == 'sectiondef':
			s = DocSection(node['kind'])
			c.members.append(s)
			for sec in node.children():
				if sec.name == 'memberdef':
					m = doc[sec['id']]
					s.members.append(m)
					parseDoxygenMember(sec, m, doc)
				elif sec.name == 'header':
					s.name = sec.text()
				else:
					raise Exception('Unsupported element %s' % sec.name)
		elif node.name in ['innerclass', 'innernamespace']:
			kind = node.name.replace('inner', '')
			s = None
			for sec in c.members:
				if sec.kind == kind:
					s = sec
			if not s:
				s = DocSection(kind)
				c.members.append(s)
			m = doc[node['refid']]
			if node['prot'] != '':
				m.protection = node['prot']
			s.members.append(m)

def parseDoxygenDocumentation(xmlroot):
	print 'parsing index.xml'
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
						if m.kind == 'function' and c.kind in ['class', 'struct']:
							m.kind = 'method'

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

protection_scope = ['public', 'protected']

docroot = sys.argv[2]
doc = parseDoxygenDocumentation(sys.argv[1])
for item in doc:
	if item.kind in ['struct', 'class', 'namespace']:
		print 'writing %s ...' % item.ref
		with open(os.path.join(docroot, '%s.html' % item.ref), 'w') as f:
			title = '%s %s Documentation' % (item.name, kinds[item.kind].single)
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
				f.write('<ul>\n')
				for base in item.base:
					if base.item.protection in protection_scope:
						f.write('<li>%s</li>\n' % base)
				f.write('</ul>\n')
			if len(item.derived) != 0:
				f.write('<h2 id="derived">Inherited By</h2>\n')
				f.write('<ul>\n')
				for derived in item.derived:
					if derived.item.protection in protection_scope:
						f.write('<li>%s</li>\n' % derived)
				f.write('</ul>\n')
			for group in item.members:
				members = [m for m in group.members if m.protection in protection_scope]
				if len(members) != 0:
					f.write('<h2 id="%s">%s</h2>\n' % (group.kind, group.name))
					for member in members:
						if member.compound:
							f.write('<p>%s</p>\n' % member)
						else:
							f.write('<p>%s %s %s</p>\n' % (member.typeof, member, member.argstr))
						if member.shortdoc:
							f.write('<blockquote>%s</blockquote>\n' % member.shortdoc)
			if len(item.longdoc) > 1:
				f.write('<h2 id="detailed_description">Detailed Description</h2>\n')
				f.write('<blockquote>\n')
				for para in item.longdoc:
					f.write('<p>%s</p>\n' % para)
				f.write('</blockquote>\n')
			if len(item.members) != 0:
				f.write('<h2 id="members">Documentation</h2>\n')
				for group in item.members:
					for member in group.members:
						if member.protection in protection_scope and not member.compound:
							f.write('<h3 id="%s">%s %s %s::%s %s</h3>\n' % (member.ref, member.protection, member.typeof, item.name, member.name, member.argstr))
							f.write('<blockquote>\n')
							for para in member.longdoc:
								f.write('<p>%s</p>\n' % para)
							f.write('</blockquote>\n')
							if len(member.docsections) != 0:
								f.write('<dl class="memberdoc">\n')
								for name, sec in member.docsections:
									f.write('<dt>%s</dt>' % name)
									f.write('<dd>%s</dd>\n' % sec)
								f.write('</dl>\n')
