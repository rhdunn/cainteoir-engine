# HTML Tree Construction Tests

These tests are designed to cover the tree construction phase of the HTML 5
parsing algorithm. Specifically, they cover how missing or misplaced tags are
converted into an XML-like balanced tag stream.

The tests are grouped into directories that take the form:

	SECTION-INSERTION_MODE
	SECTION-INSERTION_MODE~SUBGROUP

Here, `SECTION` relates to section `12.2.5.4.SECTION` of the WHATWG HTML spec [1] or
section `8.2.5.SECTION` of the W3C HTML5 spec [2].

## References

1.  [HTML Living Standard](http://www.whatwg.org/specs/web-apps/current-work/multipage/), 2014/01/08. WHATWG.
2.  [HTML5 CR](http://www.w3.org/TR/2013/CR-html5-20130806/), 2013/08/06. W3C.
