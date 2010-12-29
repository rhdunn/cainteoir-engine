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

import sys

def ok(cond, message):
	if (not cond):
		raise Exception(message)

def check_equal(a, b, message):
	if (not a == b):
		sys.stderr.write('%s\n' % message)
		sys.stderr.write('\texpected -- %s\n' % b)
		sys.stderr.write('\tactual   -- %s\n' % a)
		return False
	return True

def equal(a, b, message):
	are_equal = True
	if not check_equal(type(a).__name__, type(b).__name__, '%s -- type' % message):
		are_equal = False
	if isinstance(a, list) and isinstance(b, list):
		if not check_equal(len(a), len(b), '%s -- length' % message):
			are_equal = False
		for i, avalue in enumerate(a):
			if not check_equal(avalue, b[i], '%s -- item #%d' % (message, i + 1)):
				are_equal = False
	else:
		if not check_equal(a, b, '%s -- value' % message):
			are_equal = False
	ok(are_equal, '%s : values are not equal' % message)
