" Vim syntax file
" Language:	Cainteoir Text-to-Speech Data Files
" Filenames:    *.phon, *.features
" Maintainer:	Reece H. Dunn <msclrhd@gmail.com>
" Last Change:	2015 Mar 2

" Quit when a (custom) syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" Phoneme Features

syn keyword	cainteoirFeatUnused		ctl orl
syn keyword	cainteoirFeatConsonant		con vls vcd
syn keyword	cainteoirFeatConsonant		blb lbd dnt alv pla rfx pal vel uvl phr glt
syn keyword	cainteoirFeatConsonant		alp lbp lbv epg
syn keyword	cainteoirFeatConsonant		nas stp frc sib apr trl flp lat clk imp
syn keyword	cainteoirFeatDiacritic		ejc
syn keyword	cainteoirFeatVowel		vwl
syn keyword	cainteoirFeatVowel		unr rnd
syn keyword	cainteoirFeatVowel		fnt cnt bck
syn keyword	cainteoirFeatVowel		hgh smh umd mid lmd sml low
syn keyword	cainteoirFeatDiacritic		tie
syn keyword	cainteoirFeatSuprasegmental	pau sbr lnk fbr ibr glr glf ust dst
syn keyword	cainteoirFeatSuprasegmental	st1 st2 st3
syn keyword	cainteoirFeatSuprasegmental	est hlg lng
syn keyword	cainteoirFeatSuprasegmental	ts1 ts2 ts3 ts4 ts5
syn keyword	cainteoirFeatSuprasegmental	tm1 tm2 tm3 tm4 tm5
syn keyword	cainteoirFeatSuprasegmental	te1 te2 te3 te4 te5
syn keyword	cainteoirFeatDiacritic		syl nsy
syn keyword	cainteoirFeatDiacritic		asp unx nzr ltr
syn keyword	cainteoirFeatDiacritic		mrm slv stv crv
syn keyword	cainteoirFeatDiacritic		dzd apc lmn lgl adv ret czd mcz rsd lwr
syn keyword	cainteoirFeatDiacritic		mrd lrd lzd pzd vzd fzd vfz nzd rzd atr rtr rtc

syn match	cainteoirEscape			'\\.'
syn match	cainteoirUnicodeEscape		'\\u[0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F]'

syn match	cainteoirDirective		'\.[a-z]*'

syn region	cainteoirComment		start="#" end="$" keepend contains=@Spell

syn region	cainteoirTranscription		start="/" end="/" contains=cainteoirFeatUnused,cainteoirFeatConsonant,cainteoirFeatVowel,cainteoirFeatSuprasegmental,cainteoirFeatDiacritic

" Define the default highlighting.
" Only used when an item doesn't have highlighting yet

hi def link cainteoirComment			Comment
hi def link cainteoirTranscription		String
hi def link cainteoirFeatUnused			Constant
hi def link cainteoirFeatConsonant		Type
hi def link cainteoirFeatVowel			Type
hi def link cainteoirFeatSuprasegmental		Identifier
hi def link cainteoirFeatDiacritic		Identifier
hi def link cainteoirUnicodeEscape		Special
hi def link cainteoirEscape			Special
hi def link cainteoirDirective			PreProc

let b:current_syntax = "cainteoir"
" vim: ts=8
