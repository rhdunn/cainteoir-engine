" Cached versions of the global values to preserve user settings.
let s:current_fileformats   = ''
let s:current_fileencodings = ''

function! s:detect_cainteoirdict(line)
  if !exists("b:cainteoirdictsyntax")
    let text = getline(a:line)
    if !match(text, '^.phonemeset\t')
      let b:cainteoirdictsyntax = "dict"
    elseif !match(text, '^.author\t')
      let b:cainteoirdictsyntax = "dict"
    elseif !match(text, '^.description\t')
      let b:cainteoirdictsyntax = "dict"
    elseif !match(text, '^.copyright\t')
      let b:cainteoirdictsyntax = "dict"
    elseif !match(text, '^.license\t')
      let b:cainteoirdictsyntax = "dict"
    endif
  endif
endfunction

function! s:filetype_cainteoirdict(mode)
  if !exists("b:cainteoirdictsyntax")
    call s:detect_cainteoirdict(1)
    call s:detect_cainteoirdict(2)
    call s:detect_cainteoirdict(3)
    call s:detect_cainteoirdict(4)
    call s:detect_cainteoirdict(5)
  endif

  if exists("b:cainteoirdictsyntax")
    if a:mode == "pre"
      let s:current_fileformats   = &g:fileformats
      let s:current_fileencodings = &g:fileencodings
      set fileencodings=utf-8 fileformats=unix
      setlocal filetype=dict
    elseif a:mode == "post"
      let &g:fileformats   = s:current_fileformats
      let &g:fileencodings = s:current_fileencodings
    endif
  endif
endfunction

au BufNewFile  *.dict setlocal filetype=dict fileencoding=utf-8 fileformat=unix
au BufRead     *.dict call s:filetype_cainteoirdict("pre")
au BufReadPost *.dict call s:filetype_cainteoirdict("post")
