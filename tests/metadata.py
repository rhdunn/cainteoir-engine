#!/usr/bin/python

# Copyright (C) 2010-2014 Reece H. Dunn
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
	test = harness.TestSuite('metadata', sys.argv)
	test.run({'name': 'RDF/XML', 'groups': [
		{'name': 'metadata', 'type': ['ntriple', 'ntriple-all'], 'tests': [
			{'test': 'rdfxml/metadata/empty.rdf', 'result': 'rdfxml/metadata/empty.nt'},
			{'test': 'rdfxml/metadata/description-empty.rdf', 'result': 'rdfxml/metadata/description-empty.nt'},
			{'test': 'rdfxml/metadata/about-datatype.rdf', 'result': 'rdfxml/metadata/about-datatype.nt'},
			{'test': 'rdfxml/metadata/about-predicate_attribute-last_attribute.rdf',  'result': 'rdfxml/metadata/about-predicate_attribute.nt'},
			{'test': 'rdfxml/metadata/about-predicate_attribute-first_attribute.rdf', 'result': 'rdfxml/metadata/about-predicate_attribute.nt'},
		]},
		{'name': 'syntax', 'type': ['ntriple', 'ntriple-all'], 'tests': [
			{'test': 'rdfxml/syntax/example02.rdf', 'result': 'rdfxml/syntax/example02.nt'},
			{'test': 'rdfxml/syntax/example05.rdf', 'result': 'rdfxml/syntax/example05.nt'},
			{'test': 'rdfxml/syntax/example06.rdf', 'result': 'rdfxml/syntax/example06.nt'},
			{'test': 'rdfxml/syntax/example07.rdf', 'result': 'rdfxml/syntax/example07.nt'},
			{'test': 'rdfxml/syntax/example08.rdf', 'result': 'rdfxml/syntax/example08.nt'},
			{'test': 'rdfxml/syntax/example09.rdf', 'result': 'rdfxml/syntax/example09.nt', 'expect': 'fail'},
			{'test': 'rdfxml/syntax/example10.rdf', 'result': 'rdfxml/syntax/example10.nt'},
			{'test': 'rdfxml/syntax/example11.rdf', 'result': 'rdfxml/syntax/example11.nt'},
			{'test': 'rdfxml/syntax/example12.rdf', 'result': 'rdfxml/syntax/example12.nt'},
			{'test': 'rdfxml/syntax/example13.rdf', 'result': 'rdfxml/syntax/example13.nt'},
			{'test': 'rdfxml/syntax/example14.rdf', 'result': 'rdfxml/syntax/example14.nt'},
			{'test': 'rdfxml/syntax/example15.rdf', 'result': 'rdfxml/syntax/example15.nt'},
			{'test': 'rdfxml/syntax/example16.rdf', 'result': 'rdfxml/syntax/example16.nt'},
			{'test': 'rdfxml/syntax/example17.rdf', 'result': 'rdfxml/syntax/example17.nt'},
			{'test': 'rdfxml/syntax/example18.rdf', 'result': 'rdfxml/syntax/example18.nt'},
			{'test': 'rdfxml/syntax/example19.rdf', 'result': 'rdfxml/syntax/example19.nt'},
		]},
		{'name': 'schema', 'type': ['turtle', 'turtle-all'], 'tests': [
			{'test': '../src/schema/dcam.rdf', 'result': 'rdfxml/schema/dcam.n3'},
			{'test': '../src/schema/dcelements.rdf', 'result': 'rdfxml/schema/dcelements.n3'},
			{'test': '../src/schema/dcterms.rdf', 'result': 'rdfxml/schema/dcterms.n3'},
			{'test': '../src/schema/owl.rdf', 'result': 'rdfxml/schema/owl.n3'},
			{'test': '../src/schema/rdf-schema.rdf', 'result': 'rdfxml/schema/rdf-schema.n3'},
			{'test': '../src/schema/rdf-syntax-ns.rdf', 'result': 'rdfxml/schema/rdf-syntax-ns.n3'},
			{'test': '../src/schema/skos.rdf', 'result': 'rdfxml/schema/skos.n3'},
		]},
	]})
	test.run({'name': 'HTML', 'replace': ['mimetype'], 'groups': [
		{'name': 'title metadata', 'type': ['turtle', 'turtle-all'], 'tests': [
			{'test': 'html/metadata/title.html', 'result': 'html/metadata/title.n3', 'mimetype': 'text/html'},
			{'test': 'html/metadata/title.xhtml', 'result': 'html/metadata/title.n3', 'mimetype': 'application/xhtml+xml'},
			{'test': 'html/metadata/title-cdata.xhtml', 'result': 'html/metadata/title.n3', 'mimetype': 'application/xhtml+xml'},
			{'test': 'html/metadata/empty-title.html', 'result': 'html/metadata/empty-title.n3', 'mimetype': 'text/html'},
			{'test': 'html/metadata/empty-title.xhtml', 'result': 'html/metadata/empty-title.n3', 'mimetype': 'application/xhtml+xml'},
		]},
		{'name': 'lang metadata', 'type': ['turtle', 'turtle-all'], 'tests': [
			{'test': 'html/metadata/lang.html', 'result': 'html/metadata/lang.n3', 'mimetype': 'text/html'},
			{'test': 'html/metadata/lang.xhtml', 'result': 'html/metadata/lang.n3', 'mimetype': 'application/xhtml+xml'},
			{'test': 'html/metadata/lang-on-body.html', 'result': 'html/metadata/lang-on-body.n3', 'mimetype': 'text/html'},
			{'test': 'html/metadata/lang-on-body.xhtml', 'result': 'html/metadata/lang-on-body.n3', 'mimetype': 'application/xhtml+xml'},
			{'test': 'html/metadata/xmllang.xhtml', 'result': 'html/metadata/xmllang.n3', 'mimetype': 'application/xhtml+xml'},
			{'test': 'html/metadata/lang-and-xmllang.xhtml', 'result': 'html/metadata/lang.n3', 'mimetype': 'application/xhtml+xml'},
		]},
		{'name': 'meta - basic (@name, @content)', 'type': ['turtle', 'turtle-all'], 'tests': [
			{'test': 'html/metadata/meta/abstract.html', 'result': 'html/metadata/meta/abstract.n3', 'mimetype': 'text/html'},
			{'test': 'html/metadata/meta/abstract.xhtml', 'result': 'html/metadata/meta/abstract.n3', 'mimetype': 'application/xhtml+xml'},
			{'test': 'html/metadata/meta/author.html', 'result': 'html/metadata/meta/creator.n3', 'mimetype': 'text/html'},
			{'test': 'html/metadata/meta/author.xhtml', 'result': 'html/metadata/meta/creator.n3', 'mimetype': 'application/xhtml+xml'},
			{'test': 'html/metadata/meta/description.html', 'result': 'html/metadata/meta/description.n3', 'mimetype': 'text/html'},
			{'test': 'html/metadata/meta/description.xhtml', 'result': 'html/metadata/meta/description.n3', 'mimetype': 'application/xhtml+xml'},
			{'test': 'html/metadata/meta/keywords.html', 'result': 'html/metadata/meta/keywords.n3', 'mimetype': 'text/html'},
			{'test': 'html/metadata/meta/keywords.xhtml', 'result': 'html/metadata/meta/keywords.n3', 'mimetype': 'application/xhtml+xml'},
			{'test': 'html/metadata/meta/shs-author.html', 'result': 'html/metadata/meta/creator.n3', 'mimetype': 'text/html'},
			{'test': 'html/metadata/meta/shs-author.xhtml', 'result': 'html/metadata/meta/creator.n3', 'mimetype': 'application/xhtml+xml'},
			{'test': 'html/metadata/meta/shs-keywords.html', 'result': 'html/metadata/meta/keywords.n3', 'mimetype': 'text/html'},
			{'test': 'html/metadata/meta/shs-keywords.xhtml', 'result': 'html/metadata/meta/keywords.n3', 'mimetype': 'application/xhtml+xml'},
			{'test': 'html/metadata/meta/shs-summary.html', 'result': 'html/metadata/meta/description.n3', 'mimetype': 'text/html'},
			{'test': 'html/metadata/meta/shs-summary.xhtml', 'result': 'html/metadata/meta/description.n3', 'mimetype': 'application/xhtml+xml'},
			{'test': 'html/metadata/meta/shs-title.html', 'result': 'html/metadata/meta/title.n3', 'mimetype': 'text/html'},
			{'test': 'html/metadata/meta/shs-title.xhtml', 'result': 'html/metadata/meta/title.n3', 'mimetype': 'application/xhtml+xml'},
		]},
	]})
	test.run({'name': 'NCX', 'groups': [
		{'name': 'metadata', 'type': 'turtle', 'tests': [
			{'test': 'ncx/metadata/meta.ncx', 'result': 'ncx/metadata/meta.n3'},
			{'test': 'ncx/metadata/author.ncx', 'result': 'ncx/metadata/author.n3'},
			{'test': 'ncx/metadata/title.ncx', 'result': 'ncx/metadata/title.n3'},
		]},
		{'name': 'metadata', 'type': 'turtle-all', 'tests': [
			{'test': 'ncx/metadata/meta.ncx', 'result': 'ncx/metadata/meta.n3'},
			{'test': 'ncx/metadata/author.ncx', 'result': 'ncx/metadata/author.n3'},
			{'test': 'ncx/metadata/title.ncx', 'result': 'ncx/metadata/title-all.n3'},
		]},
		{'name': 'toc', 'type': 'turtle', 'tests': [
			{'test': 'ncx/navMap/linear.ncx', 'result': 'ncx/simple.n3'},
			{'test': 'ncx/navMap/linear-content-before-label.ncx', 'result': 'ncx/simple.n3'},
			{'test': 'ncx/navMap/linear-whitespace.ncx', 'result': 'ncx/simple.n3'},
			{'test': 'ncx/navMap/linear-with-title.ncx', 'result': 'ncx/metadata/title.n3'},
			{'test': 'ncx/navMap/in-subdir.ncx', 'result': 'ncx/simple.n3'},
			{'test': 'ncx/navMap/named-uri.ncx', 'result': 'ncx/simple.n3'},
			{'test': 'ncx/navMap/nested.ncx', 'result': 'ncx/simple.n3'},
		]},
		{'name': 'toc', 'type': 'turtle-all', 'tests': [
			{'test': 'ncx/navMap/linear.ncx', 'result': 'ncx/navMap/linear.n3'},
			{'test': 'ncx/navMap/linear-content-before-label.ncx', 'result': 'ncx/navMap/linear.n3'},
			{'test': 'ncx/navMap/linear-whitespace.ncx', 'result': 'ncx/navMap/linear.n3'},
			{'test': 'ncx/navMap/linear-with-title.ncx', 'result': 'ncx/navMap/linear-with-title.n3'},
			{'test': 'ncx/navMap/in-subdir.ncx', 'result': 'ncx/navMap/in-subdir.n3'},
			{'test': 'ncx/navMap/named-uri.ncx', 'result': 'ncx/navMap/named-uri.n3'},
			{'test': 'ncx/navMap/nested.ncx', 'result': 'ncx/navMap/nested.n3'},
		]},
	]})
	test.run({'name': 'MIME', 'groups': [
		{'name': 'email', 'type': 'turtle', 'tests': [
			{'test': 'mime/email/subject.txt', 'result': 'mime/email/subject.n3'},
			{'test': 'mime/email/from.txt', 'result': 'mime/email/from.n3'},
			{'test': 'mime/email/from-quoted.txt', 'result': 'mime/email/from.n3'},
			{'test': 'mime/email/from-inverted.txt', 'result': 'mime/email/from.n3'},
			{'test': 'mime/email/from-escaped-email.txt', 'result': 'mime/email/from.n3'},
			{'test': 'mime/email/from-no-email.txt', 'result': 'mime/email/from-no-email.n3'},
			{'test': 'mime/email/from-email-only.txt', 'result': 'mime/email/from-email-only.n3'},
			{'test': 'mime/email/from-email-email.txt', 'result': 'mime/email/from-email-only.n3'},
			{'test': 'mime/email/from-header.txt', 'result': 'mime/email/from-header.n3'},
			{'test': 'mime/email/from-archived.txt', 'result': 'mime/email/from.n3'},
			{'test': 'mime/email/newsgroups.txt', 'result': 'mime/email/newsgroups.n3'},
			{'test': 'mime/email/article-subject.txt', 'result': 'mime/email/subject.n3'},
			# some editors will convert the CR LF to LF, so LF only line endings need to be supported ...
			{'test': 'mime/email/subject-lfonly.txt', 'result': 'mime/email/subject.n3'},
			{'test': 'mime/email/from-lfonly.txt', 'result': 'mime/email/from.n3'},
			{'test': 'mime/email/from-quoted-lfonly.txt', 'result': 'mime/email/from.n3'},
			{'test': 'mime/email/from-inverted-lfonly.txt', 'result': 'mime/email/from.n3'},
			{'test': 'mime/email/from-escaped-email-lfonly.txt', 'result': 'mime/email/from.n3'},
			{'test': 'mime/email/from-no-email-lfonly.txt', 'result': 'mime/email/from-no-email.n3'},
			{'test': 'mime/email/from-email-only-lfonly.txt', 'result': 'mime/email/from-email-only.n3'},
			{'test': 'mime/email/from-email-email-lfonly.txt', 'result': 'mime/email/from-email-only.n3'},
			{'test': 'mime/email/from-header-lfonly.txt', 'result': 'mime/email/from-header.n3'},
			{'test': 'mime/email/from-archived-lfonly.txt', 'result': 'mime/email/from.n3'},
			{'test': 'mime/email/newsgroups-lfonly.txt', 'result': 'mime/email/newsgroups.n3'},
			{'test': 'mime/email/article-subject-lfonly.txt', 'result': 'mime/email/subject.n3'},
		]},
		{'name': 'email', 'type': 'turtle-all', 'replace': ['title'], 'tests': [
			{'test': 'mime/email/subject.txt', 'result': 'mime/email/subject-all.n3'},
			{'test': 'mime/email/from.txt', 'result': 'mime/email/from-all.n3', 'title': 'from.txt'},
			{'test': 'mime/email/from-quoted.txt', 'result': 'mime/email/from-all.n3', 'title': 'from-quoted.txt'},
			{'test': 'mime/email/from-inverted.txt', 'result': 'mime/email/from-all.n3', 'title': 'from-inverted.txt'},
			{'test': 'mime/email/from-escaped-email.txt', 'result': 'mime/email/from-all.n3', 'title': 'from-escaped-email.txt'},
			{'test': 'mime/email/from-no-email.txt', 'result': 'mime/email/from-no-email-all.n3', 'title': 'from-no-email.txt'},
			{'test': 'mime/email/from-email-only.txt', 'result': 'mime/email/from-email-only-all.n3', 'title': 'from-email-only.txt'},
			{'test': 'mime/email/from-email-email.txt', 'result': 'mime/email/from-email-only-all.n3', 'title': 'from-email-email.txt'},
			{'test': 'mime/email/from-header.txt', 'result': 'mime/email/from-header-all.n3'},
			{'test': 'mime/email/from-archived.txt', 'result': 'mime/email/from-all.n3', 'title': 'from-archived.txt'},
			{'test': 'mime/email/newsgroups.txt', 'result': 'mime/email/newsgroups-all.n3', 'title': 'newsgroups.txt'},
			{'test': 'mime/email/article-subject.txt', 'result': 'mime/email/subject-all.n3'},
			# some editors will convert the CR LF to LF, so LF only line endings need to be supported ...
			{'test': 'mime/email/subject-lfonly.txt', 'result': 'mime/email/subject-all.n3'},
			{'test': 'mime/email/from-lfonly.txt', 'result': 'mime/email/from-all.n3', 'title': 'from-lfonly.txt'},
			{'test': 'mime/email/from-quoted-lfonly.txt', 'result': 'mime/email/from-all.n3', 'title': 'from-quoted-lfonly.txt'},
			{'test': 'mime/email/from-inverted-lfonly.txt', 'result': 'mime/email/from-all.n3', 'title': 'from-inverted-lfonly.txt'},
			{'test': 'mime/email/from-escaped-email-lfonly.txt', 'result': 'mime/email/from-all.n3', 'title': 'from-escaped-email-lfonly.txt'},
			{'test': 'mime/email/from-no-email-lfonly.txt', 'result': 'mime/email/from-no-email-all.n3', 'title': 'from-no-email-lfonly.txt'},
			{'test': 'mime/email/from-email-only-lfonly.txt', 'result': 'mime/email/from-email-only-all.n3', 'title': 'from-email-only-lfonly.txt'},
			{'test': 'mime/email/from-email-email-lfonly.txt', 'result': 'mime/email/from-email-only-all.n3', 'title': 'from-email-email-lfonly.txt'},
			{'test': 'mime/email/from-header-lfonly.txt', 'result': 'mime/email/from-header-all.n3'},
			{'test': 'mime/email/from-archived-lfonly.txt', 'result': 'mime/email/from-all.n3', 'title': 'from-archived-lfonly.txt'},
			{'test': 'mime/email/newsgroups-lfonly.txt', 'result': 'mime/email/newsgroups-all.n3', 'title': 'newsgroups-lfonly.txt'},
			{'test': 'mime/email/article-subject-lfonly.txt', 'result': 'mime/email/subject-all.n3'},
		]},
		{'name': 'story-metadata', 'type': 'turtle', 'tests': [
			{'test': 'mime/story-metadata/title.txt', 'result': 'mime/story-metadata/title.n3'},
			{'test': 'mime/story-metadata/story.txt', 'result': 'mime/story-metadata/title.n3'},
			{'test': 'mime/story-metadata/author.txt', 'result': 'mime/story-metadata/author.n3'},
			{'test': 'mime/story-metadata/keywords.txt', 'result': 'mime/story-metadata/keywords.n3'},
			{'test': 'mime/story-metadata/title-lfonly.txt', 'result': 'mime/story-metadata/title.n3'},
			{'test': 'mime/story-metadata/story-lfonly.txt', 'result': 'mime/story-metadata/title.n3'},
			{'test': 'mime/story-metadata/author-lfonly.txt', 'result': 'mime/story-metadata/author.n3'},
			{'test': 'mime/story-metadata/keywords-lfonly.txt', 'result': 'mime/story-metadata/keywords.n3'},
		]},
		{'name': 'story-metadata', 'type': 'turtle-all', 'replace': ['title'], 'tests': [
			{'test': 'mime/story-metadata/title.txt', 'result': 'mime/story-metadata/title-all.n3'},
			{'test': 'mime/story-metadata/story.txt', 'result': 'mime/story-metadata/title-all.n3'},
			{'test': 'mime/story-metadata/author.txt', 'result': 'mime/story-metadata/author-all.n3', 'title': 'author.txt'},
			{'test': 'mime/story-metadata/keywords.txt', 'result': 'mime/story-metadata/keywords-all.n3', 'title': 'keywords.txt'},
			{'test': 'mime/story-metadata/title-lfonly.txt', 'result': 'mime/story-metadata/title-all.n3'},
			{'test': 'mime/story-metadata/story-lfonly.txt', 'result': 'mime/story-metadata/title-all.n3'},
			{'test': 'mime/story-metadata/author-lfonly.txt', 'result': 'mime/story-metadata/author-all.n3', 'title': 'author-lfonly.txt'},
			{'test': 'mime/story-metadata/keywords-lfonly.txt', 'result': 'mime/story-metadata/keywords-all.n3', 'title': 'keywords-lfonly.txt'},
		]},
	]})
	test.run({'name': 'SMIL', 'groups': [
		{'name': 'metadata', 'type': ['turtle', 'turtle-all'], 'tests': [
			{'test': 'smil/metadata/embedded-rdf.smil', 'result': 'smil/metadata/embedded-rdf.n3', 'expect': 'fail'},
			{'test': 'smil/metadata/xmllang.smil', 'result': 'smil/metadata/xmllang.n3'},
			{'test': 'smil/metadata/xmllang-noxmldecl.smil', 'result': 'smil/metadata/xmllang.n3'},
		]},
	]})
	test.run({'name': 'SSML', 'groups': [
		{'name': 'metadata', 'type': ['turtle', 'turtle-all'], 'tests': [
			{'test': 'ssml/metadata/xmllang.ssml', 'result': 'ssml/metadata/xmllang.n3'},
			{'test': 'ssml/metadata/meta-see-also.ssml', 'result': 'ssml/metadata/meta-see-also.n3'},
			{'test': 'ssml/metadata/meta-http-equiv.ssml', 'result': 'ssml/metadata/meta-http-equiv.n3'},
			{'test': 'ssml/metadata/meta-other.ssml', 'result': 'ssml/metadata/meta-other.n3'},
			{'test': 'ssml/metadata/metadata-rdfxml.ssml', 'result': 'ssml/metadata/metadata-rdfxml.n3', 'expect': 'fail'},
		]},
	]})
	test.run({'name': 'RTF', 'groups': [
		{'name': 'metadata', 'type': ['turtle'], 'tests': [
			{'test': 'rtf/info/author.rtf', 'result': 'rtf/info/author.n3'},
			{'test': 'rtf/info/comment.rtf', 'result': 'rtf/info/comment.n3'},
			{'test': 'rtf/info/keywords.rtf', 'result': 'rtf/info/keywords.n3'},
			{'test': 'rtf/info/operator.rtf', 'result': 'rtf/info/operator.n3'},
			{'test': 'rtf/info/subject.rtf', 'result': 'rtf/info/subject.n3'},
			{'test': 'rtf/info/title.rtf', 'result': 'rtf/info/title.n3'},
		]},
		{'name': 'metadata', 'type': ['turtle-all'], 'tests': [
			{'test': 'rtf/info/author.rtf', 'result': 'rtf/info/author-toc.n3'},
			{'test': 'rtf/info/comment.rtf', 'result': 'rtf/info/comment-toc.n3'},
			{'test': 'rtf/info/keywords.rtf', 'result': 'rtf/info/keywords-toc.n3'},
			{'test': 'rtf/info/operator.rtf', 'result': 'rtf/info/operator-toc.n3'},
			{'test': 'rtf/info/subject.rtf', 'result': 'rtf/info/subject-toc.n3'},
			{'test': 'rtf/info/title.rtf', 'result': 'rtf/info/title-toc.n3'},
		]},
	]})
	test.run({'name': 'OPF', 'replace': ['mimetype'], 'mimetype': 'application/oebps-package+xml', 'groups': [
		{'name': 'metadata', 'type': ['turtle', 'turtle-all'], 'tests': [
			{'test': 'opf/metadata/dublincore-contributor.opf', 'result': 'opf/metadata/dublincore-contributor.n3'},
			{'test': 'opf/metadata/dublincore-coverage.opf', 'result': 'opf/metadata/dublincore-coverage.n3'},
			{'test': 'opf/metadata/dublincore-creator.opf', 'result': 'opf/metadata/dublincore-creator.n3'},
			{'test': 'opf/metadata/dublincore-date.opf', 'result': 'opf/metadata/dublincore-date.n3'},
			{'test': 'opf/metadata/dublincore-description.opf', 'result': 'opf/metadata/dublincore-description.n3'},
			{'test': 'opf/metadata/dublincore-format.opf', 'result': 'opf/metadata/dublincore-format.n3'},
			{'test': 'opf/metadata/dublincore-identifier.opf', 'result': 'opf/metadata/dublincore-identifier.n3'},
			{'test': 'opf/metadata/dublincore-language.opf', 'result': 'opf/metadata/dublincore-language.n3'},
			{'test': 'opf/metadata/dublincore-publisher.opf', 'result': 'opf/metadata/dublincore-publisher.n3'},
			{'test': 'opf/metadata/dublincore-relation.opf', 'result': 'opf/metadata/dublincore-relation.n3'},
			{'test': 'opf/metadata/dublincore-rights.opf', 'result': 'opf/metadata/dublincore-rights.n3'},
			{'test': 'opf/metadata/dublincore-source.opf', 'result': 'opf/metadata/dublincore-source.n3'},
			{'test': 'opf/metadata/dublincore-subject.opf', 'result': 'opf/metadata/dublincore-subject.n3'},
			{'test': 'opf/metadata/dublincore-title.opf', 'result': 'opf/metadata/dublincore-title.n3'},
			{'test': 'opf/metadata/dublincore-type.opf', 'result': 'opf/metadata/dublincore-type.n3'},
			{'test': 'opf/metadata/whitespace.opf', 'result': 'opf/metadata/whitespace.n3'},
			{'test': 'opf/metadata/xmllang.opf', 'result': 'opf/metadata/xmllang.n3'},
		]},
		{'name': 'dc-metadata', 'type': ['turtle', 'turtle-all'], 'tests': [
			{'test': 'opf/dc-metadata/dublincore-contributor.opf', 'result': 'opf/metadata/dublincore-contributor.n3'},
			{'test': 'opf/dc-metadata/dublincore-coverage.opf', 'result': 'opf/metadata/dublincore-coverage.n3'},
			{'test': 'opf/dc-metadata/dublincore-creator.opf', 'result': 'opf/metadata/dublincore-creator.n3'},
			{'test': 'opf/dc-metadata/dublincore-date.opf', 'result': 'opf/metadata/dublincore-date.n3'},
			{'test': 'opf/dc-metadata/dublincore-description.opf', 'result': 'opf/metadata/dublincore-description.n3'},
			{'test': 'opf/dc-metadata/dublincore-format.opf', 'result': 'opf/metadata/dublincore-format.n3'},
			{'test': 'opf/dc-metadata/dublincore-identifier.opf', 'result': 'opf/metadata/dublincore-identifier.n3'},
			{'test': 'opf/dc-metadata/dublincore-language.opf', 'result': 'opf/metadata/dublincore-language.n3'},
			{'test': 'opf/dc-metadata/dublincore-publisher.opf', 'result': 'opf/metadata/dublincore-publisher.n3'},
			{'test': 'opf/dc-metadata/dublincore-relation.opf', 'result': 'opf/metadata/dublincore-relation.n3'},
			{'test': 'opf/dc-metadata/dublincore-rights.opf', 'result': 'opf/metadata/dublincore-rights.n3'},
			{'test': 'opf/dc-metadata/dublincore-source.opf', 'result': 'opf/metadata/dublincore-source.n3'},
			{'test': 'opf/dc-metadata/dublincore-subject.opf', 'result': 'opf/metadata/dublincore-subject.n3'},
			{'test': 'opf/dc-metadata/dublincore-title.opf', 'result': 'opf/metadata/dublincore-title.n3'},
			{'test': 'opf/dc-metadata/dublincore-type.opf', 'result': 'opf/metadata/dublincore-type.n3'},
			{'test': 'opf/dc-metadata/xmllang.opf', 'result': 'opf/metadata/xmllang.n3'},
		]},
		{'name': 'meta', 'type': ['turtle', 'turtle-all'], 'tests': [
			# metadata patterns
			{'test': 'opf/meta/name-content.opf', 'result': 'opf/meta/name-content.n3'},
			{'test': 'opf/meta/property.opf', 'result': 'opf/meta/property.n3'},
			{'test': 'opf/meta/property-id.opf', 'result': 'opf/meta/property-id.n3'},
			{'test': 'opf/meta/property-about.opf', 'result': 'opf/meta/property-about.n3'},
			{'test': 'opf/meta/property-refines.opf', 'result': 'opf/meta/property-about.n3'},
			{'test': 'opf/meta/property-datatype.opf', 'result': 'opf/meta/property-datatype.n3'},
			{'test': 'opf/meta/property-scheme.opf', 'result': 'opf/meta/property-datatype.n3'},
			# @prefix
			{'test': 'opf/meta/package-prefix.opf', 'result': 'opf/meta/prefix.n3'},
			{'test': 'opf/meta/metadata-prefix.opf', 'result': 'opf/meta/prefix.n3'},
		]},
		{'name': 'link', 'type': ['turtle', 'turtle-all'], 'tests': [
			{'test': 'opf/link/rel.opf', 'result': 'opf/link/rel.n3'},
			{'test': 'opf/link/rel-multicurie.opf', 'result': 'opf/link/rel-multicurie.n3'},
			{'test': 'opf/link/rel-id.opf', 'result': 'opf/link/rel-id.n3'},
			{'test': 'opf/link/rel-about.opf', 'result': 'opf/link/rel-about.n3'},
			{'test': 'opf/link/rel-refines.opf', 'result': 'opf/link/rel-about.n3'},
			{'test': 'opf/link/rel-mediatype.opf', 'result': 'opf/link/rel-mediatype.n3'},
		]},
		{'name': 'RDFa', 'type': ['turtle', 'turtle-all'], 'tests': [
			{'test': 'opf/metadata30/contributor-alternate-script-rdfa.opf', 'result': 'opf/metadata30/contributor-alternate-script.n3'},
			{'test': 'opf/metadata30/contributor-file-as-rdfa.opf', 'result': 'opf/metadata30/contributor-file-as.n3'},
			{'test': 'opf/metadata30/contributor-role-rdfa.opf', 'result': 'opf/metadata30/contributor-role-rdfa.n3'},
			{'test': 'opf/metadata30/creator-alternate-script-rdfa.opf', 'result': 'opf/metadata30/creator-alternate-script.n3'},
			{'test': 'opf/metadata30/creator-file-as-rdfa.opf', 'result': 'opf/metadata30/creator-file-as.n3'},
			{'test': 'opf/metadata30/creator-role-rdfa.opf', 'result': 'opf/metadata30/creator-role.n3'},
			{'test': 'opf/metadata30/identifier-rdfa.opf', 'result': 'opf/metadata30/identifier.n3'},
			{'test': 'opf/metadata30/identifier-type-rdfa.opf', 'result': 'opf/metadata30/identifier-type.n3'},
			{'test': 'opf/metadata30/meta-auth-rdfa.opf', 'result': 'opf/metadata30/meta-auth.n3'},
			{'test': 'opf/metadata30/publication-type-rdfa.opf', 'result': 'opf/metadata30/publication-type.n3'},
			{'test': 'opf/metadata30/source-identifier-rdfa.opf', 'result': 'opf/metadata30/source-identifier.n3'},
			{'test': 'opf/metadata30/title-title-type-rdfa.opf', 'result': 'opf/metadata30/title-title-type.n3'},
			{'test': 'opf/metadata30/title-display-seq-rdfa.opf', 'result': 'opf/metadata30/title-display-seq.n3'},
			{'test': 'opf/metadata30/title-file-as-rdfa.opf', 'result': 'opf/metadata30/title-file-as.n3'},
			{'test': 'opf/metadata30/title-group-position-rdfa.opf', 'result': 'opf/metadata30/title-group-position.n3'},
			{'test': 'opf/metadata30/marc21xml-record-rdfa.opf', 'result': 'opf/metadata30/marc21xml-record.n3'},
			{'test': 'opf/metadata30/mods-record-rdfa.opf', 'result': 'opf/metadata30/mods-record.n3'},
			{'test': 'opf/metadata30/onix-record-rdfa.opf', 'result': 'opf/metadata30/onix-record.n3'},
			{'test': 'opf/metadata30/xml-signature-rdfa.opf', 'result': 'opf/metadata30/xml-signature.n3'},
			{'test': 'opf/metadata30/xmp-record-rdfa.opf', 'result': 'opf/metadata30/xmp-record.n3'},
		]},
		{'name': 'dc:* with @id', 'type': ['turtle', 'turtle-all'], 'tests': [
			{'test': 'opf/metadata30/contributor-id-alternate-script.opf', 'result': 'opf/metadata30/contributor-id-alternate-script.n3'},
			{'test': 'opf/metadata30/contributor-id-file-as.opf', 'result': 'opf/metadata30/contributor-id-file-as.n3'},
			{'test': 'opf/metadata30/contributor-id-role.opf', 'result': 'opf/metadata30/contributor-id-role.n3'},
			{'test': 'opf/metadata30/creator-id-alternate-script.opf', 'result': 'opf/metadata30/creator-id-alternate-script.n3'},
			{'test': 'opf/metadata30/creator-id-file-as.opf', 'result': 'opf/metadata30/creator-id-file-as.n3'},
			{'test': 'opf/metadata30/creator-id-role.opf', 'result': 'opf/metadata30/creator-id-role.n3'},
			{'test': 'opf/metadata30/identifier-id.opf', 'result': 'opf/metadata30/identifier-id.n3'},
			{'test': 'opf/metadata30/identifier-type.opf', 'result': 'opf/metadata30/identifier-type.n3'},
		]},
		{'name': 'preferred values', 'type': ['turtle', 'turtle-all'], 'tests': [
			{'test': 'opf/metadata30/contributor-alternate-script.opf', 'result': 'opf/metadata30/contributor-alternate-script.n3'},
			{'test': 'opf/metadata30/contributor-file-as.opf', 'result': 'opf/metadata30/contributor-file-as.n3'},
			{'test': 'opf/metadata30/contributor-role.opf', 'result': 'opf/metadata30/contributor-role.n3'},
			{'test': 'opf/metadata30/creator-alternate-script.opf', 'result': 'opf/metadata30/creator-alternate-script.n3'},
			{'test': 'opf/metadata30/creator-file-as.opf', 'result': 'opf/metadata30/creator-file-as.n3'},
			{'test': 'opf/metadata30/creator-role.opf', 'result': 'opf/metadata30/creator-role.n3'},
			{'test': 'opf/metadata30/identifier.opf', 'result': 'opf/metadata30/identifier.n3'},
		]},
		{'name': '3.0 metadata properties', 'type': ['turtle', 'turtle-all'], 'tests': [
			{'test': 'opf/metadata30/meta-auth.opf', 'result': 'opf/metadata30/meta-auth.n3'},
			{'test': 'opf/metadata30/publication-type.opf', 'result': 'opf/metadata30/publication-type.n3'},
			{'test': 'opf/metadata30/source-identifier.opf', 'result': 'opf/metadata30/source-identifier.n3'},
			{'test': 'opf/metadata30/title-title-type.opf', 'result': 'opf/metadata30/title-title-type.n3'},
			{'test': 'opf/metadata30/title-display-seq.opf', 'result': 'opf/metadata30/title-display-seq.n3'},
			{'test': 'opf/metadata30/title-file-as.opf', 'result': 'opf/metadata30/title-file-as.n3'},
			{'test': 'opf/metadata30/title-group-position.opf', 'result': 'opf/metadata30/title-group-position.n3'},
			{'test': 'opf/metadata30/marc21xml-record.opf', 'result': 'opf/metadata30/marc21xml-record.n3'},
			{'test': 'opf/metadata30/mods-record.opf', 'result': 'opf/metadata30/mods-record.n3'},
			{'test': 'opf/metadata30/onix-record.opf', 'result': 'opf/metadata30/onix-record.n3'},
			{'test': 'opf/metadata30/xml-signature.opf', 'result': 'opf/metadata30/xml-signature.n3'},
			{'test': 'opf/metadata30/xmp-record.opf', 'result': 'opf/metadata30/xmp-record.n3'},
		]},
		{'name': 'manifest', 'type': 'turtle', 'tests': [
			{'test': 'opf/spine/no-toc-ncx-in-manifest.opf', 'result': 'opf/spine/single-html-file-and-toc.n3'},
			{'test': 'opf/spine/single-html-file-and-toc.opf', 'result': 'opf/spine/single-html-file-and-toc.n3'},
			{'test': 'opf/spine/epub3-html-nav.opf', 'result': 'opf/spine/single-html-file-and-toc.n3'},
			{'test': 'opf/spine/epub3-html-nav-multiple-properties.opf', 'result': 'opf/spine/single-html-file-and-toc.n3'},
			{'test': 'opf/spine/epub3-html-nav-empty-properties.opf', 'result': 'opf/spine/single-html-file-and-toc.n3'},
			{'test': 'opf/spine/epub3-html-nav-and-ncx.opf', 'result': 'opf/spine/single-html-file-and-toc.n3'},
			{'test': 'opf/spine/media-overlay.opf', 'result': 'opf/spine/single-html-file-and-toc.n3'},
		]},
		{'name': 'manifest', 'type': 'turtle-all', 'tests': [
			{'test': 'opf/spine/no-toc-ncx-in-manifest.opf', 'result': 'opf/spine/no-toc-ncx-in-manifest-all.n3'},
			{'test': 'opf/spine/single-html-file-and-toc.opf', 'result': 'opf/spine/single-html-file-and-toc-all.n3'},
			{'test': 'opf/spine/epub3-html-nav.opf', 'result': 'opf/spine/epub3-html-nav-all.n3'},
			{'test': 'opf/spine/epub3-html-nav-multiple-properties.opf', 'result': 'opf/spine/epub3-html-nav-multiple-properties-all.n3'},
			{'test': 'opf/spine/epub3-html-nav-empty-properties.opf', 'result': 'opf/spine/epub3-html-nav-empty-properties-all.n3'},
			{'test': 'opf/spine/epub3-html-nav-and-ncx.opf', 'result': 'opf/spine/epub3-html-nav-and-ncx-all.n3'},
			{'test': 'opf/spine/media-overlay.opf', 'result': 'opf/spine/media-overlay-all.n3'},
		]},
	]})
	test.run({ 'name': 'ePub', 'replace': ['mimetype'], 'mimetype': 'application/epub+zip',
		'archive':
			[
				('mimetype', 'application/epub+zip'),
				('META-INF/container.xml', 'ocf/simple.ocf'),
				('OEBPS/content.opf', '@test'), # replaced with 'test' file in the group tests
				('OEBPS/toc.ncx', 'ncx/metadata/meta.ncx'),
				('OEBPS/test.html', 'html/tree-construction/simple.html')
			],
		'groups': [
			{'name': 'metadata', 'type': ['turtle', 'turtle-all'], 'tests': [
				{'test': 'opf/metadata/dublincore-contributor.opf', 'result': 'opf/metadata/dublincore-contributor.n3'},
				{'test': 'opf/metadata/dublincore-coverage.opf', 'result': 'opf/metadata/dublincore-coverage.n3'},
				{'test': 'opf/metadata/dublincore-creator.opf', 'result': 'opf/metadata/dublincore-creator.n3'},
				{'test': 'opf/metadata/dublincore-date.opf', 'result': 'opf/metadata/dublincore-date.n3'},
				{'test': 'opf/metadata/dublincore-description.opf', 'result': 'opf/metadata/dublincore-description.n3'},
				{'test': 'opf/metadata/dublincore-format.opf', 'result': 'opf/metadata/dublincore-format.n3'},
				{'test': 'opf/metadata/dublincore-identifier.opf', 'result': 'opf/metadata/dublincore-identifier.n3'},
				{'test': 'opf/metadata/dublincore-language.opf', 'result': 'opf/metadata/dublincore-language.n3'},
				{'test': 'opf/metadata/dublincore-publisher.opf', 'result': 'opf/metadata/dublincore-publisher.n3'},
				{'test': 'opf/metadata/dublincore-relation.opf', 'result': 'opf/metadata/dublincore-relation.n3'},
				{'test': 'opf/metadata/dublincore-rights.opf', 'result': 'opf/metadata/dublincore-rights.n3'},
				{'test': 'opf/metadata/dublincore-source.opf', 'result': 'opf/metadata/dublincore-source.n3'},
				{'test': 'opf/metadata/dublincore-subject.opf', 'result': 'opf/metadata/dublincore-subject.n3'},
				{'test': 'opf/metadata/dublincore-title.opf', 'result': 'opf/metadata/dublincore-title.n3'},
				{'test': 'opf/metadata/dublincore-type.opf', 'result': 'opf/metadata/dublincore-type.n3'},
				{'test': 'opf/metadata/whitespace.opf', 'result': 'opf/metadata/whitespace.n3'},
				{'test': 'opf/metadata/xmllang.opf', 'result': 'opf/metadata/xmllang.n3'},
			]},
			{'name': 'dc-metadata', 'type': ['turtle', 'turtle-all'], 'tests': [
				{'test': 'opf/dc-metadata/dublincore-contributor.opf', 'result': 'opf/metadata/dublincore-contributor.n3'},
				{'test': 'opf/dc-metadata/dublincore-coverage.opf', 'result': 'opf/metadata/dublincore-coverage.n3'},
				{'test': 'opf/dc-metadata/dublincore-creator.opf', 'result': 'opf/metadata/dublincore-creator.n3'},
				{'test': 'opf/dc-metadata/dublincore-date.opf', 'result': 'opf/metadata/dublincore-date.n3'},
				{'test': 'opf/dc-metadata/dublincore-description.opf', 'result': 'opf/metadata/dublincore-description.n3'},
				{'test': 'opf/dc-metadata/dublincore-format.opf', 'result': 'opf/metadata/dublincore-format.n3'},
				{'test': 'opf/dc-metadata/dublincore-identifier.opf', 'result': 'opf/metadata/dublincore-identifier.n3'},
				{'test': 'opf/dc-metadata/dublincore-language.opf', 'result': 'opf/metadata/dublincore-language.n3'},
				{'test': 'opf/dc-metadata/dublincore-publisher.opf', 'result': 'opf/metadata/dublincore-publisher.n3'},
				{'test': 'opf/dc-metadata/dublincore-relation.opf', 'result': 'opf/metadata/dublincore-relation.n3'},
				{'test': 'opf/dc-metadata/dublincore-rights.opf', 'result': 'opf/metadata/dublincore-rights.n3'},
				{'test': 'opf/dc-metadata/dublincore-source.opf', 'result': 'opf/metadata/dublincore-source.n3'},
				{'test': 'opf/dc-metadata/dublincore-subject.opf', 'result': 'opf/metadata/dublincore-subject.n3'},
				{'test': 'opf/dc-metadata/dublincore-title.opf', 'result': 'opf/metadata/dublincore-title.n3'},
				{'test': 'opf/dc-metadata/dublincore-type.opf', 'result': 'opf/metadata/dublincore-type.n3'},
				{'test': 'opf/dc-metadata/xmllang.opf', 'result': 'opf/metadata/xmllang.n3'},
			]},
			{'name': 'with-content', 'type': ['turtle', 'turtle-all'], 'tests': [
				{'test': 'opf/spine/single-html-file-and-toc.opf', 'result': 'opf/spine/single-html-file-and-toc.n3'},
			]},
		]
	})
	test.run({ 'name': 'ePub', 'replace': ['mimetype'], 'mimetype': 'application/epub+zip',
		'archive':
			[
				('mimetype', 'application/epub+zip'),
				('META-INF/container.xml', 'ocf/simple.ocf'),
				('OEBPS/content.opf', 'opf/spine/single-html-file-and-toc.opf'),
				('OEBPS/toc.ncx', '@test'), # replaced with 'test' file in the group tests
				('OEBPS/test.html', 'html/tree-construction/simple.html')
			],
		'groups': [
			{'name': 'metadata', 'type': ['turtle'], 'tests': [
				{'test': 'ncx/navMap/linear.ncx', 'result': 'opf/spine/single-html-file-and-toc.n3'},
				{'test': 'ncx/navMap/linear-content-before-label.ncx', 'result': 'opf/spine/single-html-file-and-toc.n3'},
				{'test': 'ncx/navMap/linear-whitespace.ncx', 'result': 'opf/spine/single-html-file-and-toc.n3'},
				{'test': 'ncx/navMap/linear-with-title.ncx', 'result': 'opf/spine/single-html-file-and-toc.n3'},
				{'test': 'ncx/navMap/in-subdir.ncx', 'result': 'opf/spine/single-html-file-and-toc.n3'},
				{'test': 'ncx/navMap/named-uri.ncx', 'result': 'opf/spine/single-html-file-and-toc.n3'},
				{'test': 'ncx/navMap/nested.ncx', 'result': 'opf/spine/single-html-file-and-toc.n3'},
			]},
			{'name': 'metadata', 'type': ['turtle-all'], 'tests': [
				{'test': 'ncx/navMap/linear.ncx', 'result': 'ncx/navMap/linear-epub.n3'},
				{'test': 'ncx/navMap/linear-content-before-label.ncx', 'result': 'ncx/navMap/linear-epub.n3'},
				{'test': 'ncx/navMap/linear-whitespace.ncx', 'result': 'ncx/navMap/linear-epub.n3'},
				{'test': 'ncx/navMap/linear-with-title.ncx', 'result': 'ncx/navMap/linear-with-title-epub.n3'},
				{'test': 'ncx/navMap/in-subdir.ncx', 'result': 'ncx/navMap/in-subdir-epub.n3'},
				{'test': 'ncx/navMap/named-uri.ncx', 'result': 'ncx/navMap/named-uri-epub.n3'},
				{'test': 'ncx/navMap/nested.ncx', 'result': 'ncx/navMap/nested-epub.n3'},
			]},
		]
	})
	test.summary()
