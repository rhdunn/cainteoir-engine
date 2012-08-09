#!/bin/bash

PACKAGE=cainteoir-engine

doclean() {
	git clean -fxd
}

dodist() {
	( ./autogen.sh && ./configure --prefix=/usr && make dist ) || exit 1
	tar -xf ${PACKAGE}-*.tar.gz || exit 1
	pushd ${PACKAGE}-* || exit 1
	( ./configure --prefix=/usr && make && make check && popd ) || \
		( popd && exit 1 )
}

case "$1" in
	clean)     doclean ;;
	dist)      dodist ;;
	help|*)
		echo "usage: `basename $0` <command>

where <command> is one of:

    clean          Clean the build tree and generated files.
    dist           Create (and test) a distribution source tarball.
    help           Show this help screen.

To publish in source form (for GNU/Linux distributions), run:
    `basename $0` dist
"
		;;
esac
