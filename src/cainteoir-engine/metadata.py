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
	def __init__(self, ref, base):
		if ref.startswith('http://'):
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

	def __str__(self):
		return self.uri

class RDFLiteral:
	def __init__(self, text, language=None, type=None):
		self.text = ' '.join(text.split())
		self.language = language
		if type:
			self.type = RDFResource(type, None)
		else:
			self.type = None

class RDFTriple:
	def __init__(self, subject, predicate, object):
		self.subject   = subject
		self.predicate = predicate
		self.object    = object

