"""Metadata

This module provides the framework for storing and working with metadata in
the Cainteoir Text-to-Speech engine using RDF.
"""

# Copyright (C) 2010 Reece H. Dunn

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

class Resource:
	def __init__(self, ref, base):
		self.ref = ref
		self.base = base

	def __str__(self):
		return '%s%s' % (self.base, self.ref)

