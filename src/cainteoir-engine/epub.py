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

	def triples(self):
		for node in self.dom.getElementsByTagName('metadata')[0].childNodes:
			if node.nodeType == Node.ELEMENT_NODE:
				predicate = metadata.RDFResource(node.localName, node.namespaceURI)
				object = metadata.RDFLiteral(node.childNodes[0].nodeValue)
				yield metadata.RDFTriple(self.subject, predicate, object)

	def format(self, prefixes={}):
		for triple in self.triples():
			yield triple.format(prefixes)

