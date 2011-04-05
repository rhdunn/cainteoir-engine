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
	test.run({'name': 'HTML', 'groups': [
		{'name': 'semantics', 'type': 'events', 'tests': [
			{'test': 'html/semantics/simple.html', 'result': 'html/semantics/simple.events'},
			{'test': 'html/semantics/simple.xhtml', 'result': 'html/semantics/simple.events'},
			{'test': 'html/semantics/with-style.html', 'result': 'html/semantics/with-style.events'},
			{'test': 'html/semantics/with-style.xhtml', 'result': 'html/semantics/with-style.events'},
			{'test': 'html/semantics/with-style-type.html', 'result': 'html/semantics/with-style-type.events'},
			{'test': 'html/semantics/with-style-type.xhtml', 'result': 'html/semantics/with-style-type.events'},
		]},
		{'name': 'scripting', 'type': 'events', 'tests': [
			{'test': 'html/scripting/with-script.html', 'result': 'html/scripting/with-script.events'},
			{'test': 'html/scripting/with-script.xhtml', 'result': 'html/scripting/with-script.events'},
			{'test': 'html/scripting/with-script-type.html', 'result': 'html/scripting/with-script-type.events'},
			{'test': 'html/scripting/with-script-type.xhtml', 'result': 'html/scripting/with-script-type.events'},
		]},
		{'name': 'sections', 'type': 'events', 'tests': [
			{'test': 'html/sections/headings.html', 'result': 'html/sections/headings.events'},
			{'test': 'html/sections/headings.xhtml', 'result': 'html/sections/headings.events'},
			{'test': 'html/sections/paragraphs.html', 'result': 'html/sections/paragraphs.events'},
			{'test': 'html/sections/paragraphs.xhtml', 'result': 'html/sections/paragraphs.events'},
		]},
	]})
	test.run({ 'name': 'ePub',
		'archive':
			[
				('mimetype', 'application/epub+zip'),
				('META-INF/container.xml', 'ocf/simple.ocf'),
				('OEBPS/content.opf', '@test'), # replaced with 'test' file in the group tests
				('OEBPS/toc.ncx', 'ncx/empty-toc-with-title.ncx'),
				('OEBPS/test.html', 'html/semantics/simple.xhtml')
			],
		'groups': [
			{'name': 'with-content', 'type': 'events', 'tests': [
				{'test': 'epub/metadata/htmlfile-in-spine.opf', 'result': 'html/semantics/simple.events'},
				{'test': 'epub/missing-html-file.opf', 'result': 'html/semantics/simple.events'},
			]},
		]
	})
	test.summary()
