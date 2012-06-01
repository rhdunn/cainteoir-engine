# Numbers

Numbers can be written in many different scripts, such as latin, roman,
japanese, malayalam, braille and ogham. Some of these scripts may have
special rules for processing them (e.g. how to interpret the tens, hundreds
and thousands).

English can support very large numbers:

  1.  http://en.wikipedia.org/wiki/Names_of_large_numbers
  2.  http://home.kpn.nl/vanadovv/BignumbyN.html
  3.  http://en.wikipedia.org/wiki/American_and_British_English_differences#Numbers

The eSpeak text-to-speech engine is poor at handling large numbers greater
than billion (e.g. it pronounces "1 trillion" as "1 thousand billion" and
then reverts to speaking the numbers individually). It can handle numbers
with commas in better (e.g. "1,000"), pronounding "1 quadrillion" as
"1 million billion", but pronounces "1 quintillion" incorrectly as
"1 million"!

The number handling in Cainteoir can do better than the eSpeak version,
while being extensible to handle new very large number names (> 10^15)
easily.

## Triples

Triples are formed by speaking a number in groups of 3 (grouped right
to left, read left to right with an optional language-dependent
thousands separator). For example, "12063" can be grouped "12 (0)63"
and is pronounced "twelve thousand and sixty three" in British
English and "twelve thousand sixty three" in American English.

This is the traditional way of speaking ordinal (first, second, ...)
and cardinal (one, two, ...) numbers. The algorithm is the same, the
words used is different.

## Pairs

Pairs are formed by speaking a number in groups of 2. For example,
"1976" is grouped "19 76" and is spoken "nineteen seventy six" in
English.

This is the traditional way of speaking dates and times.

## Digits

Digits are formed by speaking each digit individually. For example,
"102" is grouped "102" and is spoken "one oh two" in English.

Also, if a digit occurs twice, it is usually preceeded by the word
'double', e.g. "66" can be pronounced as "double six" in English.
Likewise, a digit occuring three times is usually preceeded by the
word 'triple', e.g. "777" can be pronounced as "triple seven" in
English.

This is usually used when pronouncing telephone numbers and the
fractional part of a decimal number (e.g. "12.459" can be
pronounced as "twelve point four five nine" in English).

## Number Iterators

A number iterator should provide a way of assessing the magnitude
of the number, i.e. the number of decimal digits that make up the
number. This allows the number conversion algorithm to correctly
lookup the initial base prefix (e.g. septillion).

The number iterator should also provide access to iterating over
triples, pairs or digits as a sequence of short numbers. This
makes iterating over the number and converting it to words
efficient and easy to understand/maintain.
