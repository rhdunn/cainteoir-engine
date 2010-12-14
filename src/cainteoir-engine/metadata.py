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

class RDFResource:
	def __init__(self, ref, base=None):
		if ref.startswith('http://') or ref.startswith('/'):
			if '#' in ref:
				self.base, self.ref = ref.split('#')
				self.base = '%s#' % self.base
			else:
				self.base = '%s/' % os.path.dirname(ref)
				self.ref  = os.path.basename(ref)
			self.uri = ref
		else:
			self.base = base
			self.ref  = ref
			self.uri  = '%s%s' % (self.base, self.ref)

	def format(self, prefixes={}):
		if self.base in prefixes.keys():
			return '%s:%s' % (prefixes[self.base], self.ref)
		return '<%s>' % self.uri

	def __str__(self):
		return self.uri

class RDFLiteral:
	def __init__(self, text, language=None, type=None):
		self.text = ' '.join(text.split())
		self.language = language
		if type:
			self.type = RDFResource(type)
		else:
			self.type = None

	def format(self, prefixes={}):
		text = self.text.replace('"', '\\"')
		if self.language and self.type:
			return '"%s"@%s^^%s' % (text, self.language, self.type.format(prefixes))
		if self.language:
			return '"%s"@%s' % (text, self.language)
		if self.type:
			return '"%s"^^%s' % (text, self.type.format(prefixes))
		return '"%s"' % text

	def __str__(self):
		return self.text

class RDFTriple:
	def __init__(self, subject, predicate, object):
		self.subject   = subject
		self.predicate = predicate
		self.object    = object

	def format(self, prefixes={}):
		return '%s %s %s .' % (self.subject.format(prefixes), self.predicate.format(prefixes), self.object.format(prefixes))

