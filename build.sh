#!/bin/bash

PACKAGE=cainteoir-engine
ANDROID_VERSION=8
ANDROID_COMPILER=4.6
ANDROID_ARCHS="armeabi armeabi-v7a x86 mips"

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

doandroid() {
	./autogen.sh || exit 1
	for arch in ${ANDROID_ARCHS} ; do
		echo "===== android ${arch} ====="
		android/ndk-configure android-${ANDROID_VERSION} ${arch} ${ANDROID_COMPILER} || exit 1
		make || exit 1
		sudo make install || exit 1
	done
}

case "$1" in
	android)   doandroid ;;
	clean)     doclean ;;
	dist)      dodist ;;
	help|*)
		echo "usage: `basename $0` <command>

where <command> is one of:

    android        Build the library for android.
    clean          Clean the build tree and generated files.
    dist           Create (and test) a distribution source tarball.
    help           Show this help screen.

To publish in source form (for GNU/Linux distributions), run:
    `basename $0` dist
"
		;;
esac
