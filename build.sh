#!/bin/bash

PACKAGE=cainteoir-engine

doclean() {
	rm -vf ../{libcainteoir{0,-dev},cainteoir,metadata}_*.deb
	rm -vf ../${PACKAGE}_*.{tar.gz,dsc,build,changes}
	git clean -fxd
}

dodist() {
	( ./autogen.sh && ./configure --prefix=/usr && make dist ) || exit 1
	tar -xf ${PACKAGE}-*.tar.gz || exit 1
	pushd ${PACKAGE}-* || exit 1
	( ./configure --prefix=/usr && make && make check && popd ) || \
		( popd && exit 1 )
}

dodebsrc() {
	debuild -S -sa || exit 1
	lintian -Ivi ../${PACKAGE}_*.dsc
}

dodeb() {
	debuild -us -uc || exit 1
	lintian -Ivi ../${PACKAGE}_*.dsc
}

dorelease() {
	debuild -S -sa || exit 1
	sudo pbuilder build ../${PACKAGE}_*.dsc || exit 1
	lintian -Ivi ../${PACKAGE}_*.dsc
}

doinstall() {
	sudo dpkg --install ../{libcainteoir{0,-dev},cainteoir,metadata}_*.deb
}

douninstall() {
	yes | sudo apt-get remove libcainteoir{0,-dev} cainteoir metadata
}

doppa() {
	# Ubuntu supports a single upload building on multiple distros, but only if the
	# package is in bazaar and hosted on Launchpad. The only other way to specify
	# the target version of Ubuntu to upload to is to specify the distro name in the
	# changelog.
	#
	# Ubuntu actually ignores the distro name when building the package, but the
	# |dput| command does not.
	#
	# In addition to this, it is advised that a version identifier is used for ppa
	# files, so a "~<distro-name>1" is appended.

	DISTRO=$1
	sed -i -e "s/) unstable;/~${DISTRO}1) ${DISTRO};/" debian/changelog
	dodebsrc || (sed -i -e "s/~${DISTRO}1) ${DISTRO};/) unstable;/" debian/changelog && exit 1)
	sed -i -e "s/~${DISTRO}1) ${DISTRO};/) unstable;/" debian/changelog

	dput ppa:msclrhd-gmail/cainteoir ../${PACKAGE}_*~${DISTRO}1_source.changes
}

doallppa() {
	# NOTE: lucid is missing the autopoint package
	for DISTRO in maverick natty oneiric ; do
		doppa ${DISTRO}
	done
}

case "$1" in
	clean)     doclean ;;
	deb)       dodeb ;;
	debsrc)    dodebsrc ;;
	dist)      dodist ;;
	ppa)       doppa $2 ;;
	allppa)    doallppa ;;
	release)   dorelease ;;
	install)   doinstall ;;
	uninstall) douninstall ;;
	help|*)
		echo "usage: `basename $0` <command>

where <command> is one of:

    clean          Clean the build tree and generated files.
    deb            Create a (development build) debian binary package.
    debsrc         Create a debian source package.
    dist           Create (and test) a distribution source tarball.
    help           Show this help screen.
    install        Installs the built debian packages.
    ppa            Publish to the Cainteoir Text-to-Speech Ubuntu PPA.
    release        Create a (release build) debian binary package.
    uninstall      Uninstalls the debian packages installed by 'install'.

To publish in source form (for GNU/Linux distributions), run:
    `basename $0` dist

To build and install locally on a Debian system, run:
    `basename $0` deb
    `basename $0` install

To publish to Debian, run:
    `basename $0` release

To publish to the Ubuntu PPA for a specific distribution, run:
    `basename $0` release
    `basename $0` ppa <distro-name>
"
		;;
esac
