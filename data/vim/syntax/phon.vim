" Vim syntax file
" Language:	Phoneme Transcription
" Filenames:    *.phon
" Maintainer:	Reece H. Dunn <msclrhd@gmail.com>
" Last Change:	2014 Mar 05

" Quit when a (custom) syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn keyword	phonUnused		ctl orl
syn keyword	phonConsonant		con vls vcd
syn keyword	phonConsonant		blb lbd dnt alv pla rfx pal vel uvl phr glt
syn keyword	phonConsonant		alp lbp lbv epg
syn keyword	phonConsonant		nas stp frc sib apr trl flp lat clk imp
syn keyword	phonDiacritic		ejc
syn keyword	phonVowel		vwl
syn keyword	phonVowel		unr rnd
syn keyword	phonVowel		fnt cnt bck
syn keyword	phonVowel		hgh smh umd mid lmd sml low
syn keyword	phonDiacritic		tie
syn keyword	phonSuprasegmental	pau sbr lnk fbr ibr glr glf ust dst
syn keyword	phonSuprasegmental	st1 st2 st3
syn keyword	phonSuprasegmental	est hlg lng
syn keyword	phonSuprasegmental	ts1 ts2 ts3 ts4 ts5
syn keyword	phonSuprasegmental	tm1 tm2 tm3 tm4 tm5
syn keyword	phonSuprasegmental	te1 te2 te3 te4 te5
syn keyword	phonDiacritic		syl nsy
syn keyword	phonDiacritic		asp unx nzr ltr
syn keyword	phonDiacritic		mrm slv stv crv
syn keyword	phonDiacritic		dzd apc lmn lgl adv ret czd mcz rsd lwr
syn keyword	phonDiacritic		mrd lrd lzd pzd vzd fzd vfz nzd rzd atr rtr

syn match	phonUnicodeEscape	'\\u[0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F]'
syn match	phonEscape		'\\#'
syn match	phonDirective		'\.[a-z]*'

syn region	phonComment		start="#" end="$" keepend contains=@Spell
syn region	phonTranscription	start="/" end="/" contains=phonUnused,phonConsonant,phonVowel,phonSuprasegmental,phonDiacritic

" Define the default highlighting.
" Only used when an item doesn't have highlighting yet

hi def link phonComment			Comment
hi def link phonTranscription		String
hi def link phonUnused			Constant
hi def link phonConsonant		Type
hi def link phonVowel			Type
hi def link phonSuprasegmental		Identifier
hi def link phonDiacritic		Identifier
hi def link phonUnicodeEscape		Special
hi def link phonEscape			Special
hi def link phonDirective		PreProc

let b:current_syntax = "phon"
" vim: ts=8
