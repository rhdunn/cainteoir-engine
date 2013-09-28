#!/bin/sh

mkdir -p m4
ln -svf README.md README
ln -svf docs/ReleaseNotes.md NEWS

if [ -d .git ] ; then
	git submodule update --init --recursive || exit 1
fi

autopoint || exit 1
aclocal -I m4 || exit 1

libtoolize || exit 1
autoheader || exit 1
automake --add-missing || exit 1
autoconf || exit 1
