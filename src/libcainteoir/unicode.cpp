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
			latin,
			none,
		};
	}

	namespace classification
	{
		enum type
		{
			/* core types */
			core        = 0x00000FFF,
			invalid     = 0x00000000, /* no character associated with this codepoint */
			control     = 0x00000001, /* control character */
			newline     = 0x00000002,
			space       = 0x00000004,
			punctuation = 0x00000008,
			number      = 0x00000010,
			letter      = 0x00000020,
			symbol      = 0x00000040,
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

/** @see http://www.unicode.org/charts/PDF/U0000.pdf - C0 Controls And Basic Latin
  * @see http://www.unicode.org/charts/PDF/U0080.pdf - C1 Controls And Latin-1 Supplement
  */
static unicode::codepoint_data unicode_0x00000[] =
{
	/* 0x0000: C0 Controls And Basic Latin -- C0 Controls */
	{ 0x0000, script::none,  type::control },
	{ 0x0001, script::none,  type::control },
	{ 0x0002, script::none,  type::control },
	{ 0x0003, script::none,  type::control },
	{ 0x0004, script::none,  type::control },
	{ 0x0005, script::none,  type::control },
	{ 0x0006, script::none,  type::control },
	{ 0x0007, script::none,  type::control },
	{ 0x0008, script::none,  type::control },
	{ 0x0020, script::none,  type::space   }, /* CHARACTER TABULATION */
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
	/* 0x0020: C0 Controls And Basic Latin -- ASCII Punctuation And Symbols */
	{ 0x0020, script::latin, type::space       }, /* SPACE */
	{ 0x0021, script::latin, type::punctuation }, /* EXCLAMATION MARK */
	{ 0x0022, script::latin, type::symbol      }, /* QUOTATION MARK */
	{ 0x0023, script::latin, type::symbol      }, /* NUMBER SIGN */
	{ 0x0024, script::latin, type::symbol      }, /* DOLLAR SIGN */
	{ 0x0025, script::latin, type::symbol      }, /* PERCENT SIGN */
	{ 0x0026, script::latin, type::symbol      }, /* AMPERSAND */
	{ 0x0027, script::latin, type::symbol      }, /* APOSTROPHE */
	{ 0x0028, script::latin, type::symbol      }, /* LEFT PARENTHESIS */
	{ 0x0029, script::latin, type::symbol      }, /* RIGHT PARENTHESIS */
	{ 0x002A, script::latin, type::symbol      }, /* ASTERISK */
	{ 0x002B, script::latin, type::symbol      }, /* PLUS SIGN */
	{ 0x002C, script::latin, type::punctuation }, /* COMMA */
	{ 0x002D, script::latin, type::symbol      }, /* HYPHEN-MINUS */
	{ 0x002E, script::latin, type::punctuation }, /* FULL STOP */
	{ 0x002F, script::latin, type::symbol      }, /* SOLIDUS */
	/* 0x0030: C0 Controls And Basic Latin -- ASCII Digits */
	{ 0, script::latin, type::number }, /* DIGIT ZERO */
	{ 1, script::latin, type::number }, /* DIGIT ONE */
	{ 2, script::latin, type::number }, /* DIGIT TWO */
	{ 3, script::latin, type::number }, /* DIGIT THREE */
	{ 4, script::latin, type::number }, /* DIGIT FOUR */
	{ 5, script::latin, type::number }, /* DIGIT FIVE */
	{ 6, script::latin, type::number }, /* DIGIT SIX */
	{ 7, script::latin, type::number }, /* DIGIT SEVEN */
	{ 8, script::latin, type::number }, /* DIGIT EIGHT */
	{ 9, script::latin, type::number }, /* DIGIT NINE */
	/* 0x003A: C0 Controls And Basic Latin -- ASCII Punctuation And Symbols */
	{ 0x003A, script::latin, type::punctuation }, /* COLON */
	{ 0x003B, script::latin, type::punctuation }, /* SEMICOLON */
	{ 0x003C, script::latin, type::symbol      }, /* LESS-THAN SIGN */
	{ 0x003D, script::latin, type::symbol      }, /* EQUALS SIGN */
	{ 0x003E, script::latin, type::symbol      }, /* GREATER-THAN SIGN */
	{ 0x003F, script::latin, type::punctuation }, /* QUESTION MARK */
	{ 0x0040, script::latin, type::symbol      }, /* COMMERCIAL AT */
	/* 0x0041: C0 Controls And Basic Latin -- Uppercase Latin Alphabet */
	{ 0x0061, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER A */
	{ 0x0062, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER B */
	{ 0x0063, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER C */
	{ 0x0064, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER D */
	{ 0x0065, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER E */
	{ 0x0066, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER F */
	{ 0x0067, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER G */
	{ 0x0068, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER H */
	{ 0x0069, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER I */
	{ 0x006A, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER J */
	{ 0x006B, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER K */
	{ 0x006C, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER L */
	{ 0x006D, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER M */
	{ 0x006E, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER N */
	{ 0x006F, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER O */
	{ 0x0070, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER P */
	{ 0x0071, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER Q */
	{ 0x0072, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER R */
	{ 0x0073, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER S */
	{ 0x0074, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER T */
	{ 0x0075, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER U */
	{ 0x0076, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER V */
	{ 0x0077, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER W */
	{ 0x0078, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER X */
	{ 0x0079, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER Y */
	{ 0x007A, script::latin, type::letter | type::capital }, /* LATIN CAPTIAL LETTER Z */
	/* 0x005B: C0 Controls And Basic Latin -- ASCII Punctuation And Symbols */
	{ 0x005B, script::latin, type::symbol }, /* LEFT SQUARE BRACKET */
	{ 0x005C, script::latin, type::symbol }, /* REVERSE SOLIUS */
	{ 0x005D, script::latin, type::symbol }, /* RIGHT SQUARE BRACKET */
	{ 0x005E, script::latin, type::symbol }, /* CIRCUMFLEX ACCENT */
	{ 0x005F, script::latin, type::symbol }, /* LOW LINE */
	{ 0x0060, script::latin, type::symbol }, /* GRAVE ACCENT */
	/* 0x0061: C0 Controls And Basic Latin -- Lowercase Latin Alphabet */
	{ 0x0061, script::latin, type::letter }, /* LATIN SMALL LETTER A */
	{ 0x0062, script::latin, type::letter }, /* LATIN SMALL LETTER B */
	{ 0x0063, script::latin, type::letter }, /* LATIN SMALL LETTER C */
	{ 0x0064, script::latin, type::letter }, /* LATIN SMALL LETTER D */
	{ 0x0065, script::latin, type::letter }, /* LATIN SMALL LETTER E */
	{ 0x0066, script::latin, type::letter }, /* LATIN SMALL LETTER F */
	{ 0x0067, script::latin, type::letter }, /* LATIN SMALL LETTER G */
	{ 0x0068, script::latin, type::letter }, /* LATIN SMALL LETTER H */
	{ 0x0069, script::latin, type::letter }, /* LATIN SMALL LETTER I */
	{ 0x006A, script::latin, type::letter }, /* LATIN SMALL LETTER J */
	{ 0x006B, script::latin, type::letter }, /* LATIN SMALL LETTER K */
	{ 0x006C, script::latin, type::letter }, /* LATIN SMALL LETTER L */
	{ 0x006D, script::latin, type::letter }, /* LATIN SMALL LETTER M */
	{ 0x006E, script::latin, type::letter }, /* LATIN SMALL LETTER N */
	{ 0x006F, script::latin, type::letter }, /* LATIN SMALL LETTER O */
	{ 0x0070, script::latin, type::letter }, /* LATIN SMALL LETTER P */
	{ 0x0071, script::latin, type::letter }, /* LATIN SMALL LETTER Q */
	{ 0x0072, script::latin, type::letter }, /* LATIN SMALL LETTER R */
	{ 0x0073, script::latin, type::letter }, /* LATIN SMALL LETTER S */
	{ 0x0074, script::latin, type::letter }, /* LATIN SMALL LETTER T */
	{ 0x0075, script::latin, type::letter }, /* LATIN SMALL LETTER U */
	{ 0x0076, script::latin, type::letter }, /* LATIN SMALL LETTER V */
	{ 0x0077, script::latin, type::letter }, /* LATIN SMALL LETTER W */
	{ 0x0078, script::latin, type::letter }, /* LATIN SMALL LETTER X */
	{ 0x0079, script::latin, type::letter }, /* LATIN SMALL LETTER Y */
	{ 0x007A, script::latin, type::letter }, /* LATIN SMALL LETTER Z */
	/* 0x007B: C0 Controls And Basic Latin -- ASCII Punctuation And Symbols */
	{ 0x007B, script::latin, type::symbol }, /* LEFT CURLY BRACKET */
	{ 0x007C, script::latin, type::symbol }, /* VERTICAL LINE */
	{ 0x007D, script::latin, type::symbol }, /* RIGHT CURLY BRACKET */
	{ 0x007E, script::latin, type::symbol }, /* TILDE */
	/* 0x007F: C0 Controls And Basic Latin -- Control Character */
	{ 0x007F, script::latin, type::control },
	/* 0x0080: C1 Controls And Latin-1 Supplement -- C1 Controls */
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
	/* 0x00A0: C1 Controls And Latin-1 Supplement -- Latin-1 Punctuation And Symbols */
	{ 0x0020, script::latin, type::space       | type::skip }, /* NO-BREAK SPACE */
	{ 0x00A1, script::latin, type::punctuation }, /* INVERTED EXCLAMATION MARK */
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
	{ 0x00BF, script::latin, type::symbol      }, /* INVERTED QUESTION MARK */
	/* 0x00C0: C1 Controls And Latin-1 Supplement -- Letters */
	{ 0x0061, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER A WITH GRAVE */
	{ 0x0061, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER A WITH ACUTE */
	{ 0x0061, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER A WITH CIRCUMFLEX */
	{ 0x0061, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER A WITH TILDE */
	{ 0x0061, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER A WITH DIAERESIS */
	{ 0x0061, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER A WITH RING ABOVE */
	{ 0x00E6, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER AE */
	{ 0x0063, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER C WITH CEDILLA */
	{ 0x0065, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER E WITH GRAVE */
	{ 0x0065, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER E WITH ACUTE */
	{ 0x0065, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER E WITH CIRCUMFLEX */
	{ 0x0065, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER E WITH DIAERESIS */
	{ 0x0069, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER I WITH GRAVE */
	{ 0x0069, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER I WITH ACUTE */
	{ 0x0069, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER I WITH CIRCUMFLEX */
	{ 0x0069, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER I WITH DIAERESIS */
	{ 0x00F0, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER ETH */
	{ 0x006E, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER N WITH TILDE */
	{ 0x006F, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER O WITH GRAVE */
	{ 0x006F, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER O WITH ACUTE */
	{ 0x006F, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER O WITH CIRCUMFLEX */
	{ 0x006F, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER O WITH TILDE */
	{ 0x006F, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER O WITH DIAERESIS */
	/* 0x00D7: C1 Controls And Latin-1 Supplement -- Mathematical Operator */
	{ 0x00D7, script::latin, type::symbol }, /* MULTIPLICATION SIGN */
	/* 0x00D8: C1 Controls And Latin-1 Supplement -- Letters */
	{ 0x006F, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER O WITH STROKE */
	{ 0x0075, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER U WITH GRAVE */
	{ 0x0075, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER U WITH ACUTE */
	{ 0x0075, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER U WITH CIRCUMFLEX */
	{ 0x0075, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER U WITH DIAERESIS */
	{ 0x0079, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER Y WITH ACUTE */
	{ 0x00FE, script::latin, type::letter | type::capital }, /* LATIN CAPITAL LETTER THORN */
	{ 0x00DF, script::latin, type::letter }, /* LATIN SMALL LETTER SHARP S */
	{ 0x0061, script::latin, type::letter }, /* LATIN SMALL LETTER A WITH GRAVE */
	{ 0x0061, script::latin, type::letter }, /* LATIN SMALL LETTER A WITH ACUTE */
	{ 0x0061, script::latin, type::letter }, /* LATIN SMALL LETTER A WITH CIRCUMFLEX */
	{ 0x0061, script::latin, type::letter }, /* LATIN SMALL LETTER A WITH TILDE */
	{ 0x0061, script::latin, type::letter }, /* LATIN SMALL LETTER A WITH DIAERESIS */
	{ 0x0061, script::latin, type::letter }, /* LATIN SMALL LETTER A WITH RING ABOVE */
	{ 0x00E6, script::latin, type::letter }, /* LATIN SMALL LETTER AE */
	{ 0x0063, script::latin, type::letter }, /* LATIN SMALL LETTER C WITH CEDILLA */
	{ 0x0065, script::latin, type::letter }, /* LATIN SMALL LETTER E WITH GRAVE */
	{ 0x0065, script::latin, type::letter }, /* LATIN SMALL LETTER E WITH ACUTE */
	{ 0x0065, script::latin, type::letter }, /* LATIN SMALL LETTER E WITH CIRCUMFLEX */
	{ 0x0065, script::latin, type::letter }, /* LATIN SMALL LETTER E WITH DIAERESIS */
	{ 0x0069, script::latin, type::letter }, /* LATIN SMALL LETTER I WITH GRAVE */
	{ 0x0069, script::latin, type::letter }, /* LATIN SMALL LETTER I WITH ACUTE */
	{ 0x0069, script::latin, type::letter }, /* LATIN SMALL LETTER I WITH CIRCUMFLEX */
	{ 0x0069, script::latin, type::letter }, /* LATIN SMALL LETTER I WITH DIAERESIS */
	{ 0x00F0, script::latin, type::letter }, /* LATIN SMALL LETTER ETH */
	{ 0x006E, script::latin, type::letter }, /* LATIN SMALL LETTER N WITH TILDE */
	{ 0x006F, script::latin, type::letter }, /* LATIN SMALL LETTER O WITH GRAVE */
	{ 0x006F, script::latin, type::letter }, /* LATIN SMALL LETTER O WITH ACUTE */
	{ 0x006F, script::latin, type::letter }, /* LATIN SMALL LETTER O WITH CIRCUMFLEX */
	{ 0x006F, script::latin, type::letter }, /* LATIN SMALL LETTER O WITH TILDE */
	{ 0x006F, script::latin, type::letter }, /* LATIN SMALL LETTER O WITH DIAERESIS */
	/* 0x00F7: C1 Controls And Latin-1 Supplement -- Mathematical Operator */
	{ 0x00F7, script::latin, type::symbol }, /* DIVISION SIGN */
	/* 0x00F8: C1 Controls And Latin-1 Supplement -- Letters */
	{ 0x006F, script::latin, type::letter }, /* LATIN SMALL LETTER O WITH STROKE */
	{ 0x0075, script::latin, type::letter }, /* LATIN SMALL LETTER U WITH GRAVE */
	{ 0x0075, script::latin, type::letter }, /* LATIN SMALL LETTER U WITH ACUTE */
	{ 0x0075, script::latin, type::letter }, /* LATIN SMALL LETTER U WITH CIRCUMFLEX */
	{ 0x0075, script::latin, type::letter }, /* LATIN SMALL LETTER U WITH DIAERESIS */
	{ 0x0079, script::latin, type::letter }, /* LATIN SMALL LETTER Y WITH ACUTE */
	{ 0x00FE, script::latin, type::letter }, /* LATIN SMALL LETTER THORN */
	{ 0x0079, script::latin, type::letter }, /* LATIN SMALL LETTER Y WITH DIAERESIS */
};

static unicode_range_data data[] = /* Unicode 6.0 */
{
	{ 0x00000, 0x000FF, unicode_0x00000 },
};
