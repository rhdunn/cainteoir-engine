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

import harness

if __name__ == '__main__':
	test = harness.TestSuite('events')
	test.run_tests(name='HTML -- semantics', testtype='events', tests=[
		('html/semantics/simple.html',  'html/semantics/simple.events', 'expect-pass'),
		('html/semantics/simple.xhtml', 'html/semantics/simple.events', 'expect-pass'),
		('html/semantics/with-style.html',  'html/semantics/with-style.events', 'expect-pass'),
		('html/semantics/with-style.xhtml', 'html/semantics/with-style.events', 'expect-pass'),
		('html/semantics/with-style-type.html',  'html/semantics/with-style-type.events', 'expect-pass'),
		('html/semantics/with-style-type.xhtml', 'html/semantics/with-style-type.events', 'expect-pass'),
	])
	test.run_tests(name='HTML -- scripting', testtype='events', tests=[
		('html/scripting/with-script.html',  'html/scripting/with-script.events', 'expect-pass'),
		('html/scripting/with-script.xhtml', 'html/scripting/with-script.events', 'expect-pass'),
		('html/scripting/with-script-type.html',  'html/scripting/with-script-type.events', 'expect-pass'),
		('html/scripting/with-script-type.xhtml', 'html/scripting/with-script-type.events', 'expect-pass'),
	])
	test.run_tests(name='HTML -- sections', testtype='events', tests=[
		('html/sections/headings.html',  'html/sections/headings.events', 'expect-pass'),
		('html/sections/headings.xhtml', 'html/sections/headings.events', 'expect-pass'),
		('html/sections/paragraphs.html',  'html/sections/paragraphs.events', 'expect-pass'),
		('html/sections/paragraphs.xhtml', 'html/sections/paragraphs.events', 'expect-pass'),
	])
	test.run_epub_tests(name='ePub -- html files', testtype='events', tests=[
		('epub/metadata/htmlfile-in-spine.opf', 'html/semantics/simple.events', 'expect-pass'),
		('epub/missing-html-file.opf', 'html/semantics/simple.events', 'expect-pass'),
	])
	test.summary()
