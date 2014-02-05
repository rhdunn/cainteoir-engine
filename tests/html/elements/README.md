# HTML Element Tests

These tests are designed to cover all supported HTML elements in the different
HTML specifications and flavours. These are not intended to test the semantics
of the elements so the HTML files will not necessarily be structured correctly.

This ensures that:

  1.  all HTML elements are identified by the HTML parser;
  2.  the elements are identified irrespective of their case (upper/lower);
  3.  the XHTML variants are recognised with the different supported namespaces.

Thus, the other tests can focus on the semantics of the HTML and not need to
cover the different combinations of ways the HTML tags can be specified.

The tests are organised into:

	SPECIFICATION-VARIANT
	SPECIFICATION-DOCTYPE

where `SPECIFICATION` can be:

| Specification | Description |
| ------------- |-------------|
| frameset      | HTML 4 framesets |
| html4         | HTML 4 |
| html5         | HTML 5 and WHATWG HTML |
| xhtml10       | XHTML 1.0 |

and `VARIANT` can be:

| Variant            | Description |
|--------------------|-------------|
| lowercase.html     | The elements are in lower-case HTML form. |
| uppercase.html     | The elements are in upper-case HTML form. |
| rec-html40.html    | Elements are in the "http://www.w3.org/TR/REC-html40" namespace (e.g. MS Word). |
| .xhtml             | XML form with elements in the "http://www.w3.org/1999/xhtml" namespace. |
| no-namespace.xhtml | XML form with un-namespaced elements. |
