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
import os
import harness

test = harness.TestSuite('events')

def run_tests(name, tests=[]):
	print 'testing events for %s:' % name
	for doc, events, expect in tests:
		test.check_events(os.path.join(sys.path[0], doc), os.path.join(sys.path[0], events), test_expect=expect)

if __name__ == '__main__':
	run_tests(name='HTML -- semantics', tests=[
		('html/semantics/simple.html',  'html/semantics/simple.events', 'expect-pass'),
		('html/semantics/simple.xhtml', 'html/semantics/simple.events', 'expect-pass'),
		('html/semantics/with-style.html',  'html/semantics/with-style.events', 'expect-pass'),
		('html/semantics/with-style.xhtml', 'html/semantics/with-style.events', 'expect-pass'),
		('html/semantics/with-style-type.html',  'html/semantics/with-style-type.events', 'expect-pass'),
		('html/semantics/with-style-type.xhtml', 'html/semantics/with-style-type.events', 'expect-pass'),
	])
	run_tests(name='HTML -- scripting', tests=[
		('html/scripting/with-script.html',  'html/scripting/with-script.events', 'expect-pass'),
		('html/scripting/with-script.xhtml', 'html/scripting/with-script.events', 'expect-pass'),
		('html/scripting/with-script-type.html',  'html/scripting/with-script-type.events', 'expect-pass'),
		('html/scripting/with-script-type.xhtml', 'html/scripting/with-script-type.events', 'expect-pass'),
	])
	run_tests(name='HTML -- sections', tests=[
		('html/sections/headings.html',  'html/sections/headings.events', 'expect-pass'),
		('html/sections/headings.xhtml', 'html/sections/headings.events', 'expect-pass'),
		('html/sections/paragraphs.html',  'html/sections/paragraphs.events', 'expect-pass'),
		('html/sections/paragraphs.xhtml', 'html/sections/paragraphs.events', 'expect-pass'),
	])
	test.summary()
