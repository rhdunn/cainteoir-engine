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
			{'test': 'html/tree-construction/2-before-html/anything-else.html', 'result': 'html/tree-construction/simple.tags'},
			{'test': 'html/tree-construction/2-before-html/end-of-file.html', 'result': 'html/tree-construction/3-before-head/end-of-file.tags'},
		]},
		{'name': '12.2.5.4.3: before head', 'type': 'htmlreader', 'tests': [
			{'test': 'html/tree-construction/3-before-head/anything-else.html', 'result': 'html/tree-construction/simple.tags'},
			{'test': 'html/tree-construction/3-before-head/anything-else-2.html', 'result': 'html/tree-construction/simple.tags'},
			{'test': 'html/tree-construction/3-before-head/end-of-file.html', 'result': 'html/tree-construction/3-before-head/end-of-file.tags'},
		]},
		{'name': '12.2.5.4.4: in head - base element', 'type': 'htmlreader', 'tests': [
			{'test': 'html/tree-construction/4-in-head~base/self-closing.html', 'result': 'html/tree-construction/4-in-head~base/self-closing.tags'},
			{'test': 'html/tree-construction/4-in-head~base/self-closed.html', 'result': 'html/tree-construction/4-in-head~base/self-closing.tags'},
			{'test': 'html/tree-construction/4-in-head~base/self-closing-title.html', 'result': 'html/tree-construction/4-in-head~base/self-closing-title.tags'},
			{'test': 'html/tree-construction/4-in-head~base/self-closed-title.html', 'result': 'html/tree-construction/4-in-head~base/self-closing-title.tags'},
		]},
		{'name': '12.2.5.4.4: in head - basefont element', 'type': 'htmlreader', 'tests': [
			{'test': 'html/tree-construction/4-in-head~basefont/self-closing.html', 'result': 'html/tree-construction/4-in-head~basefont/self-closing.tags'},
			{'test': 'html/tree-construction/4-in-head~basefont/self-closed.html', 'result': 'html/tree-construction/4-in-head~basefont/self-closing.tags'},
			{'test': 'html/tree-construction/4-in-head~basefont/self-closing-title.html', 'result': 'html/tree-construction/4-in-head~basefont/self-closing-title.tags'},
			{'test': 'html/tree-construction/4-in-head~basefont/self-closed-title.html', 'result': 'html/tree-construction/4-in-head~basefont/self-closing-title.tags'},
		]},
		{'name': '12.2.5.4.4: in head - bgsound element', 'type': 'htmlreader', 'tests': [
			{'test': 'html/tree-construction/4-in-head~bgsound/self-closing.html', 'result': 'html/tree-construction/4-in-head~bgsound/self-closing.tags'},
			{'test': 'html/tree-construction/4-in-head~bgsound/self-closed.html', 'result': 'html/tree-construction/4-in-head~bgsound/self-closing.tags'},
			{'test': 'html/tree-construction/4-in-head~bgsound/self-closing-title.html', 'result': 'html/tree-construction/4-in-head~bgsound/self-closing-title.tags'},
			{'test': 'html/tree-construction/4-in-head~bgsound/self-closed-title.html', 'result': 'html/tree-construction/4-in-head~bgsound/self-closing-title.tags'},
		]},
		{'name': '12.2.5.4.4: in head - link element', 'type': 'htmlreader', 'tests': [
			{'test': 'html/tree-construction/4-in-head~link/self-closing.html', 'result': 'html/tree-construction/4-in-head~link/self-closing.tags'},
			{'test': 'html/tree-construction/4-in-head~link/self-closed.html', 'result': 'html/tree-construction/4-in-head~link/self-closing.tags'},
			{'test': 'html/tree-construction/4-in-head~link/self-closing-title.html', 'result': 'html/tree-construction/4-in-head~link/self-closing-title.tags'},
			{'test': 'html/tree-construction/4-in-head~link/self-closed-title.html', 'result': 'html/tree-construction/4-in-head~link/self-closing-title.tags'},
		]},
		{'name': '12.2.5.4.4: in head - meta element', 'type': 'htmlreader', 'tests': [
			{'test': 'html/tree-construction/4-in-head~meta/self-closing.html', 'result': 'html/tree-construction/4-in-head~meta/self-closing.tags'},
			{'test': 'html/tree-construction/4-in-head~meta/self-closed.html', 'result': 'html/tree-construction/4-in-head~meta/self-closing.tags'},
			{'test': 'html/tree-construction/4-in-head~meta/self-closing-title.html', 'result': 'html/tree-construction/4-in-head~meta/self-closing-title.tags'},
			{'test': 'html/tree-construction/4-in-head~meta/self-closed-title.html', 'result': 'html/tree-construction/4-in-head~meta/self-closing-title.tags'},
		]},
		{'name': '12.2.5.4.4: in head', 'type': 'htmlreader', 'tests': [
			{'test': 'html/tree-construction/4-in-head/anything-else.html', 'result': 'html/tree-construction/simple.tags'},
			{'test': 'html/tree-construction/4-in-head/anything-else-2.html', 'result': 'html/tree-construction/simple.tags'},
			{'test': 'html/tree-construction/4-in-head/end-of-file.html', 'result': 'html/tree-construction/4-in-head/end-of-file.tags'},
		]},
		{'name': '12.2.5.4.6: after head', 'type': 'htmlreader', 'tests': [
			{'test': 'html/tree-construction/6-after-head/anything-else.html', 'result': 'html/tree-construction/simple.tags'},
			{'test': 'html/tree-construction/6-after-head/anything-else-2.html', 'result': 'html/tree-construction/simple.tags'},
			{'test': 'html/tree-construction/6-after-head/end-of-file.html', 'result': 'html/tree-construction/6-after-head/end-of-file.tags'},
		]},
		{'name': '12.2.5.4.7: in body', 'type': 'htmlreader', 'tests': [
			{'test': 'html/tree-construction/7-in-body/end-of-file.html', 'result': 'html/tree-construction/7-in-body/end-of-file.tags'},
			{'test': 'html/tree-construction/7-in-body/pre-unclosed.html', 'result': 'html/tree-construction/7-in-body/pre-unclosed.tags'},
			{'test': 'html/tree-construction/7-in-body/pre-unclosed-with-body.html', 'result': 'html/tree-construction/7-in-body/pre-unclosed.tags'},
			{'test': 'html/tree-construction/7-in-body/nested-unclosed-inner.html', 'result': 'html/tree-construction/7-in-body/nested-unclosed-inner.tags'},
			{'test': 'html/tree-construction/7-in-body/no-body.html', 'result': 'html/tree-construction/simple.tags'},
		]},
		{'name': '12.2.5.4.19: after body', 'type': 'htmlreader', 'tests': [
			{'test': 'html/tree-construction/19-after-body/end-of-file.html', 'result': 'html/tree-construction/simple.tags'},
		]},
	]})
	test.summary()
