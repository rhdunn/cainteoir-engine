"""Metadata

This module provides the framework for storing and working with metadata in
the Cainteoir Text-to-Speech engine using RDF.
"""

# Copyright (C) 2010 Reece H. Dunn
#
# This file is part of cainteoir-engine.
#
# cainteoir-engine is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# cainteoir-engine is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with cainteoir-engine.  If not, see <http://www.gnu.org/licenses/>.

import os
from xml.dom import minidom, Node
import zipfile

import metadata

class OpfMetadata:
	def __init__(self, dom, docpath):
		self.subject = metadata.RDFResource('%s#' % docpath, None)
		self.dom = dom
		self.nextAvailableNode = 1

	def generateNode(self):
		nodeid = 'id%s' % self.nextAvailableNode
		self.nextAvailableNode = self.nextAvailableNode + 1
		return metadata.RDFResource(nodeid, self.subject.base)

	def triples(self):
		for node in self.dom.getElementsByTagName('metadata')[0].childNodes:
			if node.nodeType == Node.ELEMENT_NODE:
				predicate = metadata.RDFResource(node.localName, node.namespaceURI)
				lang = node.getAttribute('xml:lang')
				object = metadata.RDFLiteral(node.childNodes[0].nodeValue, language=lang)
				if predicate.base == 'http://purl.org/dc/elements/1.1/':
					if predicate.ref == 'creator' or predicate.ref == 'contributor':
						role = node.getAttributeNS('http://www.idpf.org/2007/opf', 'role')
						fileas = node.getAttributeNS('http://www.idpf.org/2007/opf', 'file-as')
						if role or fileas:
							temp = self.generateNode()
							yield metadata.RDFTriple(self.subject, predicate, temp)
							yield metadata.RDFTriple(temp, metadata.RDFResource('http://www.w3.org/1999/02/22-rdf-syntax-ns#value'), object)
							if role:
								yield metadata.RDFTriple(temp, metadata.RDFResource('http://www.idpf.org/2007/opf#role'), metadata.RDFLiteral(role))
							if fileas:
								yield metadata.RDFTriple(temp, metadata.RDFResource('http://www.idpf.org/2007/opf#file-as'), metadata.RDFLiteral(fileas))
						else:
							yield metadata.RDFTriple(self.subject, predicate, object)
					elif predicate.ref == 'date':
						event = node.getAttributeNS('http://www.idpf.org/2007/opf', 'event')
						if event:
							temp = self.generateNode()
							yield metadata.RDFTriple(self.subject, predicate, temp)
							yield metadata.RDFTriple(temp, metadata.RDFResource('http://www.w3.org/1999/02/22-rdf-syntax-ns#value'), object)
							yield metadata.RDFTriple(temp, metadata.RDFResource('http://www.idpf.org/2007/opf#event'), metadata.RDFLiteral(event))
						else:
							yield metadata.RDFTriple(self.subject, predicate, object)
					elif predicate.ref == 'identifier':
						scheme = node.getAttributeNS('http://www.idpf.org/2007/opf', 'scheme')
						if scheme:
							temp = self.generateNode()
							yield metadata.RDFTriple(self.subject, predicate, temp)
							yield metadata.RDFTriple(temp, metadata.RDFResource('http://www.w3.org/1999/02/22-rdf-syntax-ns#value'), object)
							yield metadata.RDFTriple(temp, metadata.RDFResource('http://www.idpf.org/2007/opf#scheme'), metadata.RDFLiteral(scheme))
						else:
							yield metadata.RDFTriple(self.subject, predicate, object)
					else:
						yield metadata.RDFTriple(self.subject, predicate, object)
				else:
					yield metadata.RDFTriple(self.subject, predicate, object)

	def format(self, prefixes={}):
		for triple in self.triples():
			yield triple.format(prefixes)

class EpubDocument:
	def __init__(self, filename):
		self.opf = None
		self.metadata = None
		self.filename = filename

		zf = zipfile.ZipFile(filename)
		if not zf.read('mimetype').startswith('application/epub+zip'):
			raise Exception('Invalid mimetype "%s".' % zf.read('mimetype'))

		container = minidom.parseString(zf.read('META-INF/container.xml')).documentElement
		for node in container.getElementsByTagName('rootfile'):
			if node.getAttribute('media-type') == 'application/oebps-package+xml':
				self.opf = minidom.parseString(zf.read(node.getAttribute('full-path'))).documentElement
				self.metadata = OpfMetadata(self.opf, self.filename)
				return

		raise Exception('Unable to find the OPF data file.')

	def triples(self):
		return self.metadata.triples()

	def format(self, prefixes={}):
		return self.metadata.format(prefixes)
