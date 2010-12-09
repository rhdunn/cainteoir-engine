#!/usr/bin/python

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

class EqualityException(Exception):
	def __init__(self, expected, actual, message):
		Exception.__init__(self, '%s : values are not equal\n\texpected: %s\n\tactual:   %s' % (message, expected, actual))
		self.expected = expected
		self.actual = actual

def ok(cond, message):
	if (not cond):
		raise Exception(message)

def equal(a, b, message):
	if (not a == b):
		raise EqualityException(expected=b, actual=a, message=message)
