#!/usr/bin/python

# Copyright (C) 2012 Reece H. Dunn
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
	test = harness.TestSuite('styles', sys.argv)
	test.run({'name': 'CSS', 'groups': [
		{'name': 'Counter Styles', 'type': 'styles', 'tests': [
			{'test': 'css/counter-styles/empty.css', 'result': 'css/counter-styles/empty.styles'},
			{'test': 'css/counter-styles/empty-cased.css', 'result': 'css/counter-styles/empty.styles'},
			{'test': 'css/counter-styles/empty-dual.css', 'result': 'css/counter-styles/empty-dual.styles'},
			{'test': 'css/counter-styles/comments.css', 'result': 'css/counter-styles/empty.styles'},
			{'test': 'css/counter-styles/system-cyclic.css', 'result': 'css/counter-styles/system-cyclic.styles'},
			{'test': 'css/counter-styles/system-cyclic-cased.css', 'result': 'css/counter-styles/system-cyclic.styles'},
			{'test': 'css/counter-styles/system-fixed.css', 'result': 'css/counter-styles/system-fixed.styles'},
			{'test': 'css/counter-styles/system-fixed-cased.css', 'result': 'css/counter-styles/system-fixed.styles'},
			{'test': 'css/counter-styles/system-fixed-offset.css', 'result': 'css/counter-styles/system-fixed-offset.styles'},
			{'test': 'css/counter-styles/system-fixed-negative-offset.css', 'result': 'css/counter-styles/system-fixed-negative-offset.styles'},
			{'test': 'css/counter-styles/system-symbolic.css', 'result': 'css/counter-styles/empty.styles'},
			{'test': 'css/counter-styles/system-symbolic-cased.css', 'result': 'css/counter-styles/empty.styles'},
			{'test': 'css/counter-styles/system-alphabetic.css', 'result': 'css/counter-styles/system-alphabetic.styles'},
			{'test': 'css/counter-styles/system-alphabetic-cased.css', 'result': 'css/counter-styles/system-alphabetic.styles'},
			{'test': 'css/counter-styles/system-numeric.css', 'result': 'css/counter-styles/system-numeric.styles'},
			{'test': 'css/counter-styles/system-numeric-cased.css', 'result': 'css/counter-styles/system-numeric.styles'},
			{'test': 'css/counter-styles/system-additive.css', 'result': 'css/counter-styles/system-additive.styles'},
			{'test': 'css/counter-styles/system-additive-cased.css', 'result': 'css/counter-styles/system-additive.styles'},
			{'test': 'css/counter-styles/negative.css', 'result': 'css/counter-styles/negative.styles'},
			{'test': 'css/counter-styles/negative-cased.css', 'result': 'css/counter-styles/negative.styles'},
			{'test': 'css/counter-styles/negative-empty.css', 'result': 'css/counter-styles/negative-empty.styles'},
			{'test': 'css/counter-styles/prefix.css', 'result': 'css/counter-styles/prefix.styles'},
			{'test': 'css/counter-styles/prefix-cased.css', 'result': 'css/counter-styles/prefix.styles'},
			{'test': 'css/counter-styles/prefix-empty.css', 'result': 'css/counter-styles/prefix-empty.styles'},
			{'test': 'css/counter-styles/suffix.css', 'result': 'css/counter-styles/suffix.styles'},
			{'test': 'css/counter-styles/suffix-cased.css', 'result': 'css/counter-styles/suffix.styles'},
			{'test': 'css/counter-styles/suffix-empty.css', 'result': 'css/counter-styles/suffix-empty.styles'},
			{'test': 'css/counter-styles/symbols.css', 'result': 'css/counter-styles/symbols.styles'},
			{'test': 'css/counter-styles/symbols-cased.css', 'result': 'css/counter-styles/symbols.styles'},
			{'test': 'css/counter-styles/additive-symbols.css', 'result': 'css/counter-styles/additive-symbols.styles'},
			{'test': 'css/counter-styles/additive-symbols-cased.css', 'result': 'css/counter-styles/additive-symbols.styles'},
			{'test': 'css/counter-styles/range.css', 'result': 'css/counter-styles/range.styles'},
			{'test': 'css/counter-styles/range-cased.css', 'result': 'css/counter-styles/range.styles'},
			{'test': 'css/counter-styles/disc.css', 'result': 'css/counter-styles/disc.styles'},
		]},
	]})
	test.summary()
