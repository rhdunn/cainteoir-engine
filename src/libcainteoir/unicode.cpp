/* Unicode Codepoint Data.
 *
 * Copyright (C) 2010 Reece H. Dunn
 *
 * This file is part of cainteoir-engine.
 *
 * cainteoir-engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cainteoir-engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cainteoir-engine.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stddef.h>
#include <stdint.h>

namespace unicode
{
	typedef uint32_t codepoint;

	namespace script
	{
		enum type
		{
			counting_rod,
			klingon, /* StarTrek */
			latin,
			none,
			tengwar, /* J. R. R. Tolkien - Cursive Script */
		};
	}

	namespace classification
	{
		enum type
		{
			/* core types */
			core        = 0x00000FFF,
			unused      = 0x00000000, /* no character associated with this codepoint */
			control     = 0x00000001, /* control character */
			newline     = 0x00000002,
			space       = 0x00000004,
			punctuation = 0x00000008,
			number      = 0x00000010,
			letter      = 0x00000020,
			symbol      = 0x00000040,
			invalid     = 0x00000080, /* not a unicode character */
			/* modifiers */
			capital     = 0x00001000, /* capital letter (upper case) */
			skip        = 0x00002000, /* skip this character when matching word spans */
		};
	}

	struct codepoint_data
	{
		/**
		  * For numbers, this is the numberical value of the codepoint.
		  * For everything else, this is the normalised character value -- the canonical form.
		  */
		codepoint normalised;
		script::type script;
		uint32_t classification;
	};
}

namespace script = unicode::script;
namespace type   = unicode::classification;

struct unicode_range_data
{
	unicode::codepoint first;
	unicode::codepoint last;
	unicode::codepoint_data *codepoints;
};

/** @see http://www.unicode.org/charts/PDF/U0000.pdf - C0 Controls And Basic Latin */
static unicode::codepoint_data unicode_c0_controls_and_basic_latin[] =
{
	/* 0x0000: C0 Controls */
	{ 0x0000, script::none,  type::control },
	{ 0x0001, script::none,  type::control },
	{ 0x0002, script::none,  type::control },
	{ 0x0003, script::none,  type::control },
	{ 0x0004, script::none,  type::control },
	{ 0x0005, script::none,  type::control },
	{ 0x0006, script::none,  type::control },
	{ 0x0007, script::none,  type::control },
	{ 0x0008, script::none,  type::control },
	{ ' ',    script::none,  type::space   }, /* CHARACTER TABULATION */
	{ 0x000A, script::none,  type::newline }, /* LINE FEED (LF) */
	{ 0x000B, script::none,  type::control },
	{ 0x000C, script::none,  type::newline }, /* FORM FEED (FF) */
	{ 0x000D, script::none,  type::newline }, /* CARRIAGE RETURN (CR) */
	{ 0x000E, script::none,  type::control },
	{ 0x000F, script::none,  type::control },
	{ 0x0010, script::none,  type::control },
	{ 0x0011, script::none,  type::control },
	{ 0x0012, script::none,  type::control },
	{ 0x0013, script::none,  type::control },
	{ 0x0014, script::none,  type::control },
	{ 0x0015, script::none,  type::control },
	{ 0x0016, script::none,  type::control },
	{ 0x0017, script::none,  type::control },
	{ 0x0018, script::none,  type::control },
	{ 0x0019, script::none,  type::control },
	{ 0x001A, script::none,  type::control },
	{ 0x001B, script::none,  type::control },
	{ 0x001C, script::none,  type::control },
	{ 0x001D, script::none,  type::control },
	{ 0x001E, script::none,  type::control },
	{ 0x001F, script::none,  type::control },
	/* 0x0020: ASCII Punctuation And Symbols */
	{ ' ',    script::latin, type::space       }, /* SPACE */
	{ '!',    script::latin, type::punctuation }, /* EXCLAMATION MARK */
	{ '"',    script::latin, type::symbol      }, /* QUOTATION MARK */
	{ '#',    script::latin, type::symbol      }, /* NUMBER SIGN */
	{ '$',    script::latin, type::symbol      }, /* DOLLAR SIGN */
	{ '%',    script::latin, type::symbol      }, /* PERCENT SIGN */
	{ '&',    script::latin, type::symbol      }, /* AMPERSAND */
	{ '\'',   script::latin, type::symbol      }, /* APOSTROPHE */
	{ '(',    script::latin, type::symbol      }, /* LEFT PARENTHESIS */
	{ ')',    script::latin, type::symbol      }, /* RIGHT PARENTHESIS */
	{ '*',    script::latin, type::symbol      }, /* ASTERISK */
	{ '+',    script::latin, type::symbol      }, /* PLUS SIGN */
	{ ',',    script::latin, type::punctuation }, /* COMMA */
	{ '-',    script::latin, type::symbol      }, /* HYPHEN-MINUS */
	{ '.',    script::latin, type::punctuation }, /* FULL STOP */
	{ '/',    script::latin, type::symbol      }, /* SOLIDUS */
	/* 0x0030: ASCII Digits */
	{ 0,      script::latin, type::number }, /* DIGIT ZERO */
	{ 1,      script::latin, type::number }, /* DIGIT ONE */
	{ 2,      script::latin, type::number }, /* DIGIT TWO */
	{ 3,      script::latin, type::number }, /* DIGIT THREE */
	{ 4,      script::latin, type::number }, /* DIGIT FOUR */
	{ 5,      script::latin, type::number }, /* DIGIT FIVE */
	{ 6,      script::latin, type::number }, /* DIGIT SIX */
	{ 7,      script::latin, type::number }, /* DIGIT SEVEN */
	{ 8,      script::latin, type::number }, /* DIGIT EIGHT */
	{ 9,      script::latin, type::number }, /* DIGIT NINE */
	/* 0x003A: ASCII Punctuation And Symbols */
	{ ':',    script::latin, type::punctuation }, /* COLON */
	{ ';',    script::latin, type::punctuation }, /* SEMICOLON */
	{ '<',    script::latin, type::symbol      }, /* LESS-THAN SIGN */
	{ '=',    script::latin, type::symbol      }, /* EQUALS SIGN */
	{ '>',    script::latin, type::symbol      }, /* GREATER-THAN SIGN */
	{ '?',    script::latin, type::punctuation }, /* QUESTION MARK */
	{ '@',    script::latin, type::symbol      }, /* COMMERCIAL AT */
	/* 0x0041: Uppercase Latin Alphabet */
	{ 'a',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER A */
	{ 'b',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER B */
	{ 'c',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER C */
	{ 'd',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER D */
	{ 'e',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER E */
	{ 'f',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER F */
	{ 'g',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER G */
	{ 'h',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER H */
	{ 'i',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER I */
	{ 'j',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER J */
	{ 'k',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER K */
	{ 'l',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER L */
	{ 'm',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER M */
	{ 'n',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER N */
	{ 'o',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER O */
	{ 'p',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER P */
	{ 'q',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER Q */
	{ 'r',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER R */
	{ 's',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER S */
	{ 't',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER T */
	{ 'u',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER U */
	{ 'v',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER V */
	{ 'w',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER W */
	{ 'x',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER X */
	{ 'y',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER Y */
	{ 'z',    script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER Z */
	/* 0x005B: ASCII Punctuation And Symbols */
	{ '[',    script::latin, type::symbol }, /* LEFT SQUARE BRACKET */
	{ '\\',   script::latin, type::symbol }, /* REVERSE SOLIUS */
	{ ']',    script::latin, type::symbol }, /* RIGHT SQUARE BRACKET */
	{ '^',    script::latin, type::symbol }, /* CIRCUMFLEX ACCENT */
	{ '_',    script::latin, type::symbol }, /* LOW LINE */
	{ '`',    script::latin, type::symbol }, /* GRAVE ACCENT */
	/* 0x0061: Lowercase Latin Alphabet */
	{ 'a',    script::latin, type::letter }, /* LATIN SMALL LETTER A */
	{ 'b',    script::latin, type::letter }, /* LATIN SMALL LETTER B */
	{ 'c',    script::latin, type::letter }, /* LATIN SMALL LETTER C */
	{ 'd',    script::latin, type::letter }, /* LATIN SMALL LETTER D */
	{ 'e',    script::latin, type::letter }, /* LATIN SMALL LETTER E */
	{ 'f',    script::latin, type::letter }, /* LATIN SMALL LETTER F */
	{ 'g',    script::latin, type::letter }, /* LATIN SMALL LETTER G */
	{ 'h',    script::latin, type::letter }, /* LATIN SMALL LETTER H */
	{ 'i',    script::latin, type::letter }, /* LATIN SMALL LETTER I */
	{ 'j',    script::latin, type::letter }, /* LATIN SMALL LETTER J */
	{ 'k',    script::latin, type::letter }, /* LATIN SMALL LETTER K */
	{ 'l',    script::latin, type::letter }, /* LATIN SMALL LETTER L */
	{ 'm',    script::latin, type::letter }, /* LATIN SMALL LETTER M */
	{ 'n',    script::latin, type::letter }, /* LATIN SMALL LETTER N */
	{ 'o',    script::latin, type::letter }, /* LATIN SMALL LETTER O */
	{ 'p',    script::latin, type::letter }, /* LATIN SMALL LETTER P */
	{ 'q',    script::latin, type::letter }, /* LATIN SMALL LETTER Q */
	{ 'r',    script::latin, type::letter }, /* LATIN SMALL LETTER R */
	{ 's',    script::latin, type::letter }, /* LATIN SMALL LETTER S */
	{ 't',    script::latin, type::letter }, /* LATIN SMALL LETTER T */
	{ 'u',    script::latin, type::letter }, /* LATIN SMALL LETTER U */
	{ 'v',    script::latin, type::letter }, /* LATIN SMALL LETTER V */
	{ 'w',    script::latin, type::letter }, /* LATIN SMALL LETTER W */
	{ 'x',    script::latin, type::letter }, /* LATIN SMALL LETTER X */
	{ 'y',    script::latin, type::letter }, /* LATIN SMALL LETTER Y */
	{ 'z',    script::latin, type::letter }, /* LATIN SMALL LETTER Z */
	/* 0x007B: ASCII Punctuation And Symbols */
	{ '{',    script::latin, type::symbol }, /* LEFT CURLY BRACKET */
	{ '|',    script::latin, type::symbol }, /* VERTICAL LINE */
	{ '}',    script::latin, type::symbol }, /* RIGHT CURLY BRACKET */
	{ '~',    script::latin, type::symbol }, /* TILDE */
	/* 0x007F: Control Character */
	{ 0x007F, script::latin, type::control },
};

/** @see http://www.unicode.org/charts/PDF/U0080.pdf - C1 Controls And Latin-1 Supplement */
static unicode::codepoint_data unicode_c1_controls_and_latin_1_supplement[] =
{
	/* 0x0080: C1 Controls */
	{ 0x0080, script::none,  type::control },
	{ 0x0081, script::none,  type::control },
	{ 0x0082, script::none,  type::control },
	{ 0x0083, script::none,  type::control },
	{ 0x0084, script::none,  type::control },
	{ 0x0085, script::none,  type::control },
	{ 0x0086, script::none,  type::control },
	{ 0x0087, script::none,  type::control },
	{ 0x0088, script::none,  type::control },
	{ 0x0089, script::none,  type::control },
	{ 0x008A, script::none,  type::control },
	{ 0x008B, script::none,  type::control },
	{ 0x008C, script::none,  type::control },
	{ 0x008D, script::none,  type::control },
	{ 0x008E, script::none,  type::control },
	{ 0x008F, script::none,  type::control },
	{ 0x0090, script::none,  type::control },
	{ 0x0091, script::none,  type::control },
	{ 0x0092, script::none,  type::control },
	{ 0x0093, script::none,  type::control },
	{ 0x0094, script::none,  type::control },
	{ 0x0095, script::none,  type::control },
	{ 0x0096, script::none,  type::control },
	{ 0x0097, script::none,  type::control },
	{ 0x0098, script::none,  type::control },
	{ 0x0099, script::none,  type::control },
	{ 0x009A, script::none,  type::control },
	{ 0x009B, script::none,  type::control },
	{ 0x009C, script::none,  type::control },
	{ 0x009D, script::none,  type::control },
	{ 0x009E, script::none,  type::control },
	{ 0x009F, script::none,  type::control },
	/* 0x00A0: Latin-1 Punctuation And Symbols */
	{ ' ',    script::latin, type::space       | type::skip }, /* NO-BREAK SPACE */
	{ '!',    script::latin, type::punctuation }, /* INVERTED EXCLAMATION MARK */
	{ 0x00A2, script::latin, type::symbol      }, /* CENT SIGN */
	{ 0x00A3, script::latin, type::symbol      }, /* POUND SIGN */
	{ 0x00A4, script::latin, type::symbol      }, /* CURRENCY SIGN */
	{ 0x00A5, script::latin, type::symbol      }, /* YEN SIGN */
	{ 0x00A6, script::latin, type::symbol      }, /* BROKEN BAR */
	{ 0x00A7, script::latin, type::symbol      }, /* SECTION SIGN */
	{ 0x00A8, script::latin, type::symbol      }, /* DIAERESIS */
	{ 0x00A9, script::latin, type::symbol      }, /* COPYRIGHT SIGN */
	{ 0x00AA, script::latin, type::symbol      }, /* FEMANINE ORDINAL INDICATOR */
	{ 0x00AB, script::latin, type::symbol      }, /* LEFT-POINTING DOUBLE ANGLE QUOTATION MARK */
	{ 0x00AC, script::latin, type::symbol      }, /* NOT SIGN */
	{ 0x00AD, script::latin, type::space       | type::skip }, /* SOFT HYPHEN */
	{ 0x00AE, script::latin, type::symbol      }, /* REGISTERED SIGN */
	{ 0x00AF, script::latin, type::symbol      }, /* MACRON */
	{ 0x00B0, script::latin, type::symbol      }, /* DEGREE SIGN */
	{ 0x00B1, script::latin, type::symbol      }, /* PLUS-MINUS SIGN */
	{ 0x00B2, script::latin, type::symbol      }, /* SUPERSCRIPT TWO */
	{ 0x00B3, script::latin, type::symbol      }, /* SUPERSCRIPT THREE */
	{ 0x00B4, script::latin, type::symbol      }, /* ACUTE ACCENT */
	{ 0x00B5, script::latin, type::symbol      }, /* MICRO SIGN */
	{ 0x00B6, script::latin, type::space       }, /* PILCRO SIGN */
	{ 0x00B7, script::latin, type::symbol      }, /* MIDDLE DOT */
	{ 0x00B8, script::latin, type::symbol      }, /* CEDILLA */
	{ 0x00B9, script::latin, type::symbol      }, /* SUPERSCRIPT ONE */
	{ 0x00BA, script::latin, type::symbol      }, /* MASCULINE ORDINAL INDICATOR */
	{ 0x00BB, script::latin, type::symbol      }, /* RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK */
	{ 0x00BC, script::latin, type::symbol      }, /* VULGAR FRACTION ONE QUARTER (1/4) */
	{ 0x00BD, script::latin, type::symbol      }, /* VULGAR FRACTION ONE HALF (1/2) */
	{ 0x00BE, script::latin, type::symbol      }, /* VULGAR FRACTION THREE QUARTERS (3/4) */
	{ '?',    script::latin, type::symbol      }, /* INVERTED QUESTION MARK */
	/* 0x00C0: Letters */
	{ 'a',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER A WITH GRAVE */
	{ 'a',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER A WITH ACUTE */
	{ 'a',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER A WITH CIRCUMFLEX */
	{ 'a',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER A WITH TILDE */
	{ 'a',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER A WITH DIAERESIS */
	{ 'a',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER A WITH RING ABOVE */
	{ 0x00E6, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER AE */
	{ 'c',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER C WITH CEDILLA */
	{ 'e',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER E WITH GRAVE */
	{ 'e',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER E WITH ACUTE */
	{ 'e',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER E WITH CIRCUMFLEX */
	{ 'e',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER E WITH DIAERESIS */
	{ 'i',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER I WITH GRAVE */
	{ 'i',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER I WITH ACUTE */
	{ 'i',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER I WITH CIRCUMFLEX */
	{ 'i',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER I WITH DIAERESIS */
	{ 0x00F0, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER ETH */
	{ 'n',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER N WITH TILDE */
	{ 'o',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER O WITH GRAVE */
	{ 'o',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER O WITH ACUTE */
	{ 'o',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER O WITH CIRCUMFLEX */
	{ 'o',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER O WITH TILDE */
	{ 'o',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER O WITH DIAERESIS */
	/* 0x00D7: Mathematical Operator */
	{ 0x00D7, script::latin, type::symbol }, /* MULTIPLICATION SIGN */
	/* 0x00D8: Letters */
	{ 'o',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER O WITH STROKE */
	{ 'u',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER U WITH GRAVE */
	{ 'u',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER U WITH ACUTE */
	{ 'u',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER U WITH CIRCUMFLEX */
	{ 'u',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER U WITH DIAERESIS */
	{ 'y',    script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER Y WITH ACUTE */
	{ 0x00FE, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER THORN */
	{ 's',    script::latin, type::letter }, /* LATIN SMALL LETTER SHARP S */
	{ 'a',    script::latin, type::letter }, /* LATIN SMALL LETTER A WITH GRAVE */
	{ 'a',    script::latin, type::letter }, /* LATIN SMALL LETTER A WITH ACUTE */
	{ 'a',    script::latin, type::letter }, /* LATIN SMALL LETTER A WITH CIRCUMFLEX */
	{ 'a',    script::latin, type::letter }, /* LATIN SMALL LETTER A WITH TILDE */
	{ 'a',    script::latin, type::letter }, /* LATIN SMALL LETTER A WITH DIAERESIS */
	{ 'a',    script::latin, type::letter }, /* LATIN SMALL LETTER A WITH RING ABOVE */
	{ 0x00E6, script::latin, type::letter }, /* LATIN SMALL LETTER AE */
	{ 'c',    script::latin, type::letter }, /* LATIN SMALL LETTER C WITH CEDILLA */
	{ 'e',    script::latin, type::letter }, /* LATIN SMALL LETTER E WITH GRAVE */
	{ 'e',    script::latin, type::letter }, /* LATIN SMALL LETTER E WITH ACUTE */
	{ 'e',    script::latin, type::letter }, /* LATIN SMALL LETTER E WITH CIRCUMFLEX */
	{ 'e',    script::latin, type::letter }, /* LATIN SMALL LETTER E WITH DIAERESIS */
	{ 'i',    script::latin, type::letter }, /* LATIN SMALL LETTER I WITH GRAVE */
	{ 'i',    script::latin, type::letter }, /* LATIN SMALL LETTER I WITH ACUTE */
	{ 'i',    script::latin, type::letter }, /* LATIN SMALL LETTER I WITH CIRCUMFLEX */
	{ 'i',    script::latin, type::letter }, /* LATIN SMALL LETTER I WITH DIAERESIS */
	{ 0x00F0, script::latin, type::letter }, /* LATIN SMALL LETTER ETH */
	{ 'n',    script::latin, type::letter }, /* LATIN SMALL LETTER N WITH TILDE */
	{ 'o',    script::latin, type::letter }, /* LATIN SMALL LETTER O WITH GRAVE */
	{ 'o',    script::latin, type::letter }, /* LATIN SMALL LETTER O WITH ACUTE */
	{ 'o',    script::latin, type::letter }, /* LATIN SMALL LETTER O WITH CIRCUMFLEX */
	{ 'o',    script::latin, type::letter }, /* LATIN SMALL LETTER O WITH TILDE */
	{ 'o',    script::latin, type::letter }, /* LATIN SMALL LETTER O WITH DIAERESIS */
	/* 0x00F7: Mathematical Operator */
	{ 0x00F7, script::latin, type::symbol }, /* DIVISION SIGN */
	/* 0x00F8: Letters */
	{ 'o',    script::latin, type::letter }, /* LATIN SMALL LETTER O WITH STROKE */
	{ 'u',    script::latin, type::letter }, /* LATIN SMALL LETTER U WITH GRAVE */
	{ 'u',    script::latin, type::letter }, /* LATIN SMALL LETTER U WITH ACUTE */
	{ 'u',    script::latin, type::letter }, /* LATIN SMALL LETTER U WITH CIRCUMFLEX */
	{ 'u',    script::latin, type::letter }, /* LATIN SMALL LETTER U WITH DIAERESIS */
	{ 'y',    script::latin, type::letter }, /* LATIN SMALL LETTER Y WITH ACUTE */
	{ 0x00FE, script::latin, type::letter }, /* LATIN SMALL LETTER THORN */
	{ 'y',    script::latin, type::letter }, /* LATIN SMALL LETTER Y WITH DIAERESIS */
};

/** @see http://www.unicode.org/charts/PDF/U1D360.pdf - Counting Rod Units */
static unicode::codepoint_data unicode_counting_rod_units[] =
{
	{ 1,       script::counting_rod, type::number  }, /* COUNTING ROD DIGIT ONE */
	{ 2,       script::counting_rod, type::number  }, /* COUNTING ROD DIGIT TWO */
	{ 3,       script::counting_rod, type::number  }, /* COUNTING ROD DIGIT THREE */
	{ 4,       script::counting_rod, type::number  }, /* COUNTING ROD DIGIT FOUR */
	{ 5,       script::counting_rod, type::number  }, /* COUNTING ROD DIGIT FIVE */
	{ 6,       script::counting_rod, type::number  }, /* COUNTING ROD DIGIT SIX */
	{ 7,       script::counting_rod, type::number  }, /* COUNTING ROD DIGIT SEVEN */
	{ 8,       script::counting_rod, type::number  }, /* COUNTING ROD DIGIT EIGHT */
	{ 9,       script::counting_rod, type::number  }, /* COUNTING ROD DIGIT NINE */
	{ 10,      script::counting_rod, type::number  }, /* COUNTING ROD TENS DIGIT ONE */
	{ 20,      script::counting_rod, type::number  }, /* COUNTING ROD TENS DIGIT TWO */
	{ 30,      script::counting_rod, type::number  }, /* COUNTING ROD TENS DIGIT THREE */
	{ 40,      script::counting_rod, type::number  }, /* COUNTING ROD TENS DIGIT FOUR */
	{ 50,      script::counting_rod, type::number  }, /* COUNTING ROD TENS DIGIT FIVE */
	{ 60,      script::counting_rod, type::number  }, /* COUNTING ROD TENS DIGIT SIX */
	{ 70,      script::counting_rod, type::number  }, /* COUNTING ROD TENS DIGIT SEVEN */
	{ 80,      script::counting_rod, type::number  }, /* COUNTING ROD TENS DIGIT EIGHT */
	{ 90,      script::counting_rod, type::number  }, /* COUNTING ROD TENS DIGIT NINE */
	{ 0x1D372, script::none, type::unused },
	{ 0x1D373, script::none, type::unused },
	{ 0x1D374, script::none, type::unused },
	{ 0x1D375, script::none, type::unused },
	{ 0x1D376, script::none, type::unused },
	{ 0x1D377, script::none, type::unused },
	{ 0x1D378, script::none, type::unused },
	{ 0x1D379, script::none, type::unused },
	{ 0x1D37A, script::none, type::unused },
	{ 0x1D37B, script::none, type::unused },
	{ 0x1D37C, script::none, type::unused },
	{ 0x1D37D, script::none, type::unused },
	{ 0x1D37E, script::none, type::unused },
	{ 0x1D37F, script::none, type::unused },
};

/** @see http://www.evertype.com/standards/csur/tengwar.html - Tengwar */
static unicode::codepoint_data unicode_tengwar[] =
{
	{ 0x0E000, script::tengwar, type::letter }, /* TENGWAR LETTER TINCO */
	{ 0x0E001, script::tengwar, type::letter }, /* TENGWAR LETTER PARMA */
	{ 0x0E002, script::tengwar, type::letter }, /* TENGWAR LETTER CALMA */
	{ 0x0E003, script::tengwar, type::letter }, /* TENGWAR LETTER QUESSE */
	{ 0x0E004, script::tengwar, type::letter }, /* TENGWAR LETTER ANDO */
	{ 0x0E005, script::tengwar, type::letter }, /* TENGWAR LETTER UMBAR */
	{ 0x0E006, script::tengwar, type::letter }, /* TENGWAR LETTER ANGA */
	{ 0x0E007, script::tengwar, type::letter }, /* TENGWAR LETTER UNGWE */
	{ 0x0E008, script::tengwar, type::letter }, /* TENGWAR LETTER THUULE (suule) */
	{ 0x0E009, script::tengwar, type::letter }, /* TENGWAR LETTER FORMEN */
	{ 0x0E00A, script::tengwar, type::letter }, /* TENGWAR LETTER HARMA (aha) */
	{ 0x0E00B, script::tengwar, type::letter }, /* TENGWAR LETTER HWESTA */
	{ 0x0E00C, script::tengwar, type::letter }, /* TENGWAR LETTER ANTO */
	{ 0x0E00D, script::tengwar, type::letter }, /* TENGWAR LETTER AMPA */
	{ 0x0E00E, script::tengwar, type::letter }, /* TENGWAR LETTER ANCA */
	{ 0x0E00F, script::tengwar, type::letter }, /* TENGWAR LETTER UNQUE */
	{ 0x0E010, script::tengwar, type::letter }, /* TENGWAR LETTER NUUMEN */
	{ 0x0E011, script::tengwar, type::letter }, /* TENGWAR LETTER MALTA */
	{ 0x0E012, script::tengwar, type::letter }, /* TENGWAR LETTER NOLDO (ngoldo) */
	{ 0x0E013, script::tengwar, type::letter }, /* TENGWAR LETTER NWALME (ngwalme) */
	{ 0x0E014, script::tengwar, type::letter }, /* TENGWAR LETTER OORE */
	{ 0x0E015, script::tengwar, type::letter }, /* TENGWAR LETTER VALA */
	{ 0x0E016, script::tengwar, type::letter }, /* TENGWAR LETTER ANNA */
	{ 0x0E017, script::tengwar, type::letter }, /* TENGWAR LETTER VILYA (wilya) */
	{ 0x0E018, script::tengwar, type::letter }, /* TENGWAR LETTER ROOMEN */
	{ 0x0E019, script::tengwar, type::letter }, /* TENGWAR LETTER ARDA */
	{ 0x0E01A, script::tengwar, type::letter }, /* TENGWAR LETTER LAMBE */
	{ 0x0E01B, script::tengwar, type::letter }, /* TENGWAR LETTER ALDA */
	{ 0x0E01C, script::tengwar, type::letter }, /* TENGWAR LETTER SILME */
	{ 0x0E01D, script::tengwar, type::letter }, /* TENGWAR LETTER SILME NUQUERNA */
	{ 0x0E01E, script::tengwar, type::letter }, /* TENGWAR LETTER AARE (aaze, esse) */
	{ 0x0E01F, script::tengwar, type::letter }, /* TENGWAR LETTER AARE NUQUERNA (aaze n., esse n.) */
	{ 0x0E020, script::tengwar, type::letter }, /* TENGWAR LETTER HYARMEN */
	{ 0x0E021, script::tengwar, type::letter }, /* TENGWAR LETTER HWESTA SINDARINWA */
	{ 0x0E022, script::tengwar, type::letter }, /* TENGWAR LETTER YANTA */
	{ 0x0E023, script::tengwar, type::letter }, /* TENGWAR LETTER UURE */
	{ 0x0E024, script::tengwar, type::letter }, /* TENGWAR LETTER HALLA */
	{ 0x0E025, script::tengwar, type::letter }, /* TENGWAR LETTER SHORT CARRIER */
	{ 0x0E026, script::tengwar, type::letter }, /* TENGWAR LETTER LONG CARRIER */
	{ 0x0E027, script::tengwar, type::letter }, /* TENGWAR LETTER ANNA SINDARINWA */
	{ 0x0E028, script::tengwar, type::letter }, /* TENGWAR LETTER EXTENDED THUULE */
	{ 0x0E029, script::tengwar, type::letter }, /* TENGWAR LETTER EXTENDED FORMEN */
	{ 0x0E02A, script::tengwar, type::letter }, /* TENGWAR LETTER EXTENDED HARMA */
	{ 0x0E02B, script::tengwar, type::letter }, /* TENGWAR LETTER EXTENDED HWESTA */
	{ 0x0E02C, script::tengwar, type::letter }, /* TENGWAR LETTER EXTENDED ANTO */
	{ 0x0E02D, script::tengwar, type::letter }, /* TENGWAR LETTER EXTENDED AMPA */
	{ 0x0E02E, script::tengwar, type::letter }, /* TENGWAR LETTER EXTENDED ANCA */
	{ 0x0E02F, script::tengwar, type::letter }, /* TENGWAR LETTER EXTENDED UNQUE */
	{ 0x0E030, script::tengwar, type::letter }, /* TENGWAR LETTER STEMLESS OORE (digit zero) */
	{ 0x0E031, script::tengwar, type::letter }, /* TENGWAR LETTER STEMLESS VALA */
	{ 0x0E032, script::tengwar, type::letter }, /* TENGWAR LETTER STEMLESS ANNA */
	{ 0x0E033, script::tengwar, type::letter }, /* TENGWAR LETTER STEMLESS VILYA (digit one) */
	{ 0x0E034, script::none,    type::unused },
	{ 0x0E035, script::none,    type::unused },
	{ 0x0E036, script::none,    type::unused },
	{ 0x0E037, script::none,    type::unused },
	{ 0x0E038, script::none,    type::unused },
	{ 0x0E039, script::none,    type::unused },
	{ 0x0E03A, script::none,    type::unused },
	{ 0x0E03B, script::none,    type::unused },
	{ 0x0E03C, script::none,    type::unused },
	{ 0x0E03D, script::none,    type::unused },
	{ 0x0E03E, script::none,    type::unused },
	{ 0x0E03F, script::none,    type::unused },
	{ 0x0E040, script::tengwar, type::letter }, /* TENGWAR SIGN THREE DOTS ABOVE */
	{ 0x0E041, script::tengwar, type::letter }, /* TENGWAR SIGN THREE DOTS BELOW */
	{ 0x0E042, script::tengwar, type::letter }, /* TENGWAR SIGN TWO DOTS ABOVE */
	{ 0x0E043, script::tengwar, type::letter }, /* TENGWAR SIGN TWO DOTS BELOW */
	{ 0x0E044, script::tengwar, type::letter }, /* TENGWAR SIGN AMATICSE (dot above) */
	{ 0x0E045, script::tengwar, type::letter }, /* TENGWAR SIGN NUNTICSE (dot below) */
	{ 0x0E046, script::tengwar, type::letter }, /* TENGWAR SIGN ACUTE (andaith, long mark) */
	{ 0x0E047, script::tengwar, type::letter }, /* TENGWAR SIGN DOUBLE ACUTE */
	{ 0x0E048, script::tengwar, type::letter }, /* TENGWAR SIGN RIGHT CURL */
	{ 0x0E049, script::tengwar, type::letter }, /* TENGWAR SIGN DOUBLE RIGHT CURL */
	{ 0x0E04A, script::tengwar, type::letter }, /* TENGWAR SIGN LEFT CURL */
	{ 0x0E04B, script::tengwar, type::letter }, /* TENGWAR SIGN DOUBLE LEFT CURL */
	{ 0x0E04C, script::tengwar, type::letter }, /* TENGWAR SIGN NASALIZER */
	{ 0x0E04D, script::tengwar, type::letter }, /* TENGWAR SIGN DOUBLER */
	{ 0x0E04E, script::tengwar, type::letter }, /* TENGWAR SIGN TILDE */
	{ 0x0E04F, script::tengwar, type::letter }, /* TENGWAR SIGN BREVE */
	{ ',',     script::tengwar, type::punctuation }, /* TENGWAR PUSTA (putta, stop) */
	{ '.',     script::tengwar, type::punctuation }, /* TENGWAR DOUBLE PUSTA (putta) */
	{ '!',     script::tengwar, type::punctuation }, /* TENGWAR EXCLAMATION MARK */
	{ '?',     script::tengwar, type::punctuation }, /* TENGWAR QUESTION MARK */
	{ 0x0E054, script::tengwar, type::letter }, /* TENGWAR SECTION MARK */
	{ 0x0E055, script::tengwar, type::letter }, /* TENGWAR LONG SECTION MARK */
	{ 0x0E056, script::tengwar, type::letter }, /* TENGWAR SIGN LONG CARRIER BELOW */
	{ 0x0E057, script::tengwar, type::letter }, /* TENGWAR SIGN DOUBLE ACUTE BELOW */
	{ 0x0E058, script::tengwar, type::letter }, /* TENGWAR SIGN RIGHT CURL BELOW */
	{ 0x0E059, script::none,    type::unused },
	{ 0x0E05A, script::tengwar, type::letter }, /* TENGWAR SIGN LEFT CURL BELOW */
	{ 0x0E05B, script::none,    type::unused },
	{ 0x0E05C, script::tengwar, type::letter }, /* TENGWAR SIGN LEFT FOLLOWING SILME */
	{ 0x0E05D, script::tengwar, type::letter }, /* TENGWAR SIGN RIGHT FOLLOWING SILME */
	{ 0x0E05E, script::none,    type::unused },
	{ 0x0E05F, script::none,    type::unused },
	{ 0x0E060, script::none,    type::unused },
	{ 0x0E061, script::none,    type::unused },
	{ 2,       script::tengwar, type::number }, /* TENGWAR DIGIT TWO */
	{ 3,       script::tengwar, type::number }, /* TENGWAR DIGIT THREE */
	{ 4,       script::tengwar, type::number }, /* TENGWAR DIGIT FOUR */
	{ 5,       script::tengwar, type::number }, /* TENGWAR DIGIT FIVE */
	{ 6,       script::tengwar, type::number }, /* TENGWAR DIGIT SIX */
	{ 7,       script::tengwar, type::number }, /* TENGWAR DIGIT SEVEN */
	{ 8,       script::tengwar, type::number }, /* TENGWAR DIGIT EIGHT */
	{ 9,       script::tengwar, type::number }, /* TENGWAR DIGIT NINE */
	{ 10,      script::tengwar, type::number }, /* TENGWAR DUODECIMAL DIGIT TEN */
	{ 11,      script::tengwar, type::number }, /* TENGWAR DUODECIMAL DIGIT ELEVEN */
	{ 0x0E06C, script::tengwar, type::symbol }, /* TENGWAR DECIMAL BASE MARK */
	{ 0x0E06D, script::tengwar, type::symbol }, /* TENGWAR DUODECIMAL BASE MARK */
	{ 0x0E06E, script::tengwar, type::symbol }, /* TENGWAR DUODECIMAL LEAST SIGNIFICANT DIGIT MARK */
	{ 0x0E06F, script::none,    type::unused },
	{ 0x0E070, script::none,    type::unused },
	{ 0x0E071, script::none,    type::unused },
	{ 0x0E072, script::none,    type::unused },
	{ 0x0E073, script::none,    type::unused },
	{ 0x0E074, script::none,    type::unused },
	{ 0x0E075, script::none,    type::unused },
	{ 0x0E076, script::none,    type::unused },
	{ 0x0E077, script::none,    type::unused },
	{ 0x0E078, script::none,    type::unused },
	{ 0x0E079, script::none,    type::unused },
	{ 0x0E07A, script::none,    type::unused },
	{ 0x0E07B, script::none,    type::unused },
	{ 0x0E07C, script::none,    type::unused },
	{ 0x0E07D, script::none,    type::unused },
	{ 0x0E07E, script::none,    type::unused },
	{ 0x0E07F, script::none,    type::unused },
};

/** @see http://www.evertype.com/standards/csur/klingon.html - Klingon */
static unicode::codepoint_data unicode_klingon[] =
{
	{ 0x0F8D0, script::klingon, type::letter }, /* KLINGON LETTER A */
	{ 0x0F8D1, script::klingon, type::letter }, /* KLINGON LETTER B */
	{ 0x0F8D2, script::klingon, type::letter }, /* KLINGON LETTER CH */
	{ 0x0F8D3, script::klingon, type::letter }, /* KLINGON LETTER D */
	{ 0x0F8D4, script::klingon, type::letter }, /* KLINGON LETTER E */
	{ 0x0F8D5, script::klingon, type::letter }, /* KLINGON LETTER GH */
	{ 0x0F8D6, script::klingon, type::letter }, /* KLINGON LETTER H */
	{ 0x0F8D7, script::klingon, type::letter }, /* KLINGON LETTER I */
	{ 0x0F8D8, script::klingon, type::letter }, /* KLINGON LETTER J */
	{ 0x0F8D9, script::klingon, type::letter }, /* KLINGON LETTER L */
	{ 0x0F8DA, script::klingon, type::letter }, /* KLINGON LETTER M */
	{ 0x0F8DB, script::klingon, type::letter }, /* KLINGON LETTER N */
	{ 0x0F8DC, script::klingon, type::letter }, /* KLINGON LETTER NG */
	{ 0x0F8DD, script::klingon, type::letter }, /* KLINGON LETTER O */
	{ 0x0F8DE, script::klingon, type::letter }, /* KLINGON LETTER P */
	{ 0x0F8DF, script::klingon, type::letter }, /* KLINGON LETTER Q */
	{ 0x0F8E0, script::klingon, type::letter }, /* KLINGON LETTER QH */
	{ 0x0F8E1, script::klingon, type::letter }, /* KLINGON LETTER R */
	{ 0x0F8E2, script::klingon, type::letter }, /* KLINGON LETTER S */
	{ 0x0F8E3, script::klingon, type::letter }, /* KLINGON LETTER T */
	{ 0x0F8E4, script::klingon, type::letter }, /* KLINGON LETTER TLH */
	{ 0x0F8E5, script::klingon, type::letter }, /* KLINGON LETTER U */
	{ 0x0F8E6, script::klingon, type::letter }, /* KLINGON LETTER V */
	{ 0x0F8E7, script::klingon, type::letter }, /* KLINGON LETTER W */
	{ 0x0F8E8, script::klingon, type::letter }, /* KLINGON LETTER Y */
	{ 0x0F8E9, script::klingon, type::letter }, /* KLINGON LETTER GLOTTAL STOP */
	{ 0x0F8EA, script::none,    type::unused },
	{ 0x0F8EB, script::none,    type::unused },
	{ 0x0F8EC, script::none,    type::unused },
	{ 0x0F8ED, script::none,    type::unused },
	{ 0x0F8EE, script::none,    type::unused },
	{ 0x0F8EF, script::none,    type::unused },
	{ 0,       script::klingon, type::number }, /* KLINGON DIGIT ZERO */
	{ 1,       script::klingon, type::number }, /* KLINGON DIGIT ONE */
	{ 2,       script::klingon, type::letter }, /* KLINGON DIGIT TWO */
	{ 3,       script::klingon, type::letter }, /* KLINGON DIGIT THREE */
	{ 4,       script::klingon, type::letter }, /* KLINGON DIGIT FOUR */
	{ 5,       script::klingon, type::letter }, /* KLINGON DIGIT FIVE */
	{ 6,       script::klingon, type::letter }, /* KLINGON DIGIT SIX */
	{ 7,       script::klingon, type::letter }, /* KLINGON DIGIT SEVEN */
	{ 8,       script::klingon, type::letter }, /* KLINGON DIGIT EIGHT */
	{ 9,       script::klingon, type::letter }, /* KLINGON DIGIT NINE */
	{ 0x0F8FA, script::none,    type::unused },
	{ 0x0F8FB, script::none,    type::unused },
	{ 0x0F8FC, script::none,    type::unused },
	{ ',',     script::klingon, type::punctuation }, /* KLINGON COMMA */
	{ '.',     script::klingon, type::punctuation }, /* KLINGON FULL STOP */
	{ 0x0F8FF, script::klingon, type::symbol }, /* KLINGON MUMMIFICATION GLYPH */
};

static unicode_range_data data[] = /* Unicode 6.0 */
{
	{ 0x00000, 0x0007F, unicode_c0_controls_and_basic_latin },
	{ 0x00080, 0x000FF, unicode_c1_controls_and_latin_1_supplement },
	{ 0x0E000, 0x0E07F, unicode_tengwar }, /* unofficial - private (reserved) area */
	{ 0x0F8D0, 0x0F8FF, unicode_klingon }, /* unofficial - private (reserved) area */

/* SMP: Supplementary Multilingual Plane */

	/*{ 0x1CC00, 0x1CC7F, unicode_tengwar }, /* proposed - http://www.unicode.org/roadmaps/smp */
	{ 0x1D360, 0x1D37F, unicode_counting_rod_units },
};
