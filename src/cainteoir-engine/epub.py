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

import metadata

class OpfMetadata:
	def __init__(self, filename, docpath):
		self.subject = metadata.RDFResource('%s#' % docpath, None)
		self.dom = minidom.parse(filename).documentElement
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
					if predicate.ref == 'creator':
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
					else:
						yield metadata.RDFTriple(self.subject, predicate, object)
				else:
					yield metadata.RDFTriple(self.subject, predicate, object)

	def format(self, prefixes={}):
		for triple in self.triples():
			yield triple.format(prefixes)

