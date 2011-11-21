#!/bin/bash

PACKAGE=cainteoir-engine

doclean() {
	rm -vf ../{libcainteoir{0,-dev},cainteoir-data,cainteoir,metadata}_*.deb
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

builddeb() {
	DIST=$1
	shift
	doclean
	sed -i -e "s/) unstable;/~${DIST}1) ${DIST};/" debian/changelog
	if [[ -e debian/$DIST.patch ]] ; then
		patch -f -p1 -i debian/$DIST.patch || touch builddeb.failed
	fi
	if [[ ! -e builddeb.failed ]] ; then
		echo "... building debian packages ($@) ..."
		debuild $@ || touch builddeb.failed
	fi
	if [[ ! -e builddeb.failed ]] ; then
		echo "... validating debian packages ..."
		lintian -Ivi ../${PACKAGE}_*.dsc || touch builddeb.failed
	fi
	if [[ -e debian/$DIST.patch ]] ; then
		patch -Rf -p1 -i debian/$DIST.patch || touch builddeb.failed
	fi
	sed -i -e "s/~${DIST}1) ${DIST};/) unstable;/" debian/changelog
	if [[ -e builddeb.failed ]] ; then
		rm builddeb.failed
		exit 1
	fi
}

dorelease() {
	buildeb -S -sa || exit 1
	sudo pbuilder build ../${PACKAGE}_*.dsc || exit 1
	lintian -Ivi ../${PACKAGE}_*.dsc
}

doinstall() {
	sudo dpkg --install ../{libcainteoir{0,-dev},cainteoir-data,cainteoir,metadata}_*.deb
}

douninstall() {
	yes | sudo apt-get remove libcainteoir{0,-dev} cainteoir-data cainteoir metadata
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
	DIST=$1
	builddeb $DIST -S -sa || exit 1
	dput ppa:msclrhd-gmail/cainteoir ../${PACKAGE}_*~${DIST}1_source.changes
}

doallppa() {
	for DISTRO in lucid maverick natty oneiric ; do
		doppa ${DISTRO}
	done
}

case "$1" in
	allppa)    doallppa ;;
	clean)     doclean ;;
	deb)       builddeb $2 -us -uc ;;
	debsrc)    builddeb $2 -S -sa ;;
	dist)      dodist ;;
	ppa)       doppa $2 ;;
	release)   dorelease ;;
	install)   doinstall ;;
	uninstall) douninstall ;;
	help|*)
		echo "usage: `basename $0` <command>

where <command> is one of:

    allppa         Publish to the Cainteoir Text-to-Speech Ubuntu PPA for all supported distributions.
    clean          Clean the build tree and generated files.
    deb <dist>     Create a (development build) debian binary package.
    debsrc <dist>  Create a debian source package.
    dist           Create (and test) a distribution source tarball.
    help           Show this help screen.
    install        Installs the built debian packages.
    ppa <dist>     Publish to the Cainteoir Text-to-Speech Ubuntu PPA for <dist>.
    release        Create a (release build) debian binary package.
    uninstall      Uninstalls the debian packages installed by 'install'.

To publish in source form (for GNU/Linux distributions), run:
    `basename $0` dist

To build and install locally on a Debian system, run:
    `basename $0` deb <dist-name>
    `basename $0` install

To publish to Debian, run:
    `basename $0` release

To publish to the Ubuntu PPA for a specific distribution, run:
    `basename $0` release
    `basename $0` ppa <dist-name>
"
		;;
esac
