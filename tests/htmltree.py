#!/usr/bin/python

# Copyright (C) 2013 Reece H. Dunn
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

import harness
import sys

if __name__ == '__main__':
	test = harness.TestSuite('HTMLTree', sys.argv)
	test.run({'name': 'HTML', 'groups': [
		{'name': '12.2.5.4.2: before html', 'type': 'htmlreader', 'tests': [
			{'test': 'html/tree-construction/simple.html', 'result': 'html/tree-construction/simple.tags'},
			{'test': 'html/tree-construction/2-before_html-anything-else.html', 'result': 'html/tree-construction/simple.tags'},
		]},
		{'name': '12.2.5.4.3: before head', 'type': 'htmlreader', 'tests': [
			{'test': 'html/tree-construction/3-before_head-anything-else.html', 'result': 'html/tree-construction/simple.tags'},
			{'test': 'html/tree-construction/3-before_head-anything-else-2.html', 'result': 'html/tree-construction/simple.tags'},
		]},
		{'name': '12.2.5.4.4: in head', 'type': 'htmlreader', 'tests': [
			# self-closing elements
			{'test': 'html/tree-construction/4-in_head-base.html', 'result': 'html/tree-construction/4-in_head-base.tags'},
			{'test': 'html/tree-construction/4-in_head-base-self-closed.html', 'result': 'html/tree-construction/4-in_head-base.tags'},
			{'test': 'html/tree-construction/4-in_head-base-title.html', 'result': 'html/tree-construction/4-in_head-base-title.tags'},
			{'test': 'html/tree-construction/4-in_head-base-self-closed-title.html', 'result': 'html/tree-construction/4-in_head-base-title.tags'},
		]},
	]})
	test.summary()
