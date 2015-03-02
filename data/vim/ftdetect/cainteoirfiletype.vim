" Cached versions of the global values to preserve user settings.
let s:current_fileformats   = ''
let s:current_fileencodings = ''

function! s:detect_cainteoir(line)
  if !exists("b:cainteoirsyntax")
    let text = getline(a:line)
    if !match(text, '^.phonemeset\t')
      let b:cainteoirsyntax = "cainteoir"
    elseif !match(text, '^.author\t')
      let b:cainteoirsyntax = "cainteoir"
    elseif !match(text, '^.description\t')
      let b:cainteoirsyntax = "cainteoir"
    elseif !match(text, '^.copyright\t')
      let b:cainteoirsyntax = "cainteoir"
    elseif !match(text, '^.license\t')
      let b:cainteoirsyntax = "cainteoir"
    endif
  endif
endfunction

function! s:filetype_cainteoir(mode, detection)
  if !exists("b:cainteoirsyntax")
    if a:detection == "force"
      let b:cainteoirsyntax = "cainteoir"
    else
      call s:detect_cainteoir(1)
      call s:detect_cainteoir(2)
      call s:detect_cainteoir(3)
      call s:detect_cainteoir(4)
      call s:detect_cainteoir(5)
    endif
  endif

  if exists("b:cainteoirsyntax")
    if a:mode == "pre"
      let s:current_fileformats   = &g:fileformats
      let s:current_fileencodings = &g:fileencodings
      set fileencodings=utf-8 fileformats=unix
      setlocal filetype=cainteoir
    elseif a:mode == "post"
      let &g:fileformats   = s:current_fileformats
      let &g:fileencodings = s:current_fileencodings
    endif
  endif
endfunction

" Cainteoir Text-to-Speech dictionary files

au BufRead     *.dict call s:filetype_cainteoir("pre",  "auto")
au BufReadPost *.dict call s:filetype_cainteoir("post", "auto")

" Cainteoir Text-to-Speech phoneme transcription scheme files

au BufNewFile  *.phon setlocal filetype=cainteoir fileencoding=utf-8 fileformat=unix
au BufRead     *.phon call s:filetype_cainteoir("pre",  "force")
au BufReadPost *.phon call s:filetype_cainteoir("post", "force")

" Cainteoir Text-to-Speech phoneme-to-phoneme mapping file

au BufNewFile  *.ptp setlocal filetype=cainteoir fileencoding=utf-8 fileformat=unix
au BufRead     *.ptp call s:filetype_cainteoir("pre",  "force")
au BufReadPost *.ptp call s:filetype_cainteoir("post", "force")

" Cainteoir Text-to-Speech phoneme feature test files

au BufNewFile  *.features setlocal filetype=cainteoir fileencoding=utf-8 fileformat=unix
au BufRead     *.features call s:filetype_cainteoir("pre",  "force")
au BufReadPost *.features call s:filetype_cainteoir("post", "force")
