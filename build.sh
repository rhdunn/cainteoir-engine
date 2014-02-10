#!/bin/bash

PACKAGE=cainteoir-engine
DPUT_PPA=cainteoir-ppa

BUILD_DIR=/opt/data/sbuild

ANDROID_VERSION=8
ANDROID_COMPILER=4.6
ANDROID_ARCHS="armeabi armeabi-v7a x86 mips"

case `uname -m` in
	x86_64)
		HOST_ARCH=amd64
		;;
	*)
		HOST_ARCH=i386
		;;
esac

pkg_list_debs() {
	BASEDIR=`dirname $1`
	grep "\.deb$" $1 2>/dev/null | grep -P " (optional|extra) " | sed -e "s,.* ,${BASEDIR}/,g"
}

pkg_list() {
	BASEDIR=`dirname $1`
	VERSION=$(dpkg-parsechangelog|sed -n 's/^Version: \(.*:\|\)//p')
	pkg_list_debs $1
	find ${BASEDIR}/${PACKAGE}_*.{dsc,tar.*} 2>/dev/null
	find ${BASEDIR}/${PACKAGE}_*_${ARCH}*.{build,changes} 2>/dev/null
}

list_rm() {
	while read FILE ; do
		rm -v ${FILE}
	done
}

list_mv() {
	DEST=$1
	while read FILE ; do
		mv -v ${FILE} ${DEST}/ 2>/dev/null
	done
}

dodist() {
	( ./autogen.sh && ./configure --prefix=/usr && make dist ) || exit 1
	tar -xf ${PACKAGE}-*.tar.gz || exit 1
	pushd ${PACKAGE}-* || exit 1
	( ./configure --prefix=/usr && make && make check && popd ) || \
		( popd && exit 1 )
}

dopredebbuild() {
	DIST=$1
	cp debian/changelog{,.downstream}
	sed -i -e "s/~unstable\([0-9]*\)) unstable;/~${DIST}\1) ${DIST};/" debian/changelog
	sed -i -e "s/(\([0-9\.\-]*\)) unstable;/(\1~${DIST}1) ${DIST};/" debian/changelog
	if [[ -e debian/$DIST.patch ]] ; then
		patch -f -p1 -i debian/$DIST.patch || touch builddeb.failed
	fi
}

dopostdebbuild() {
	DIST=$1
	if [[ -e debian/$DIST.patch ]] ; then
		patch -Rf -p1 -i debian/$DIST.patch || touch builddeb.failed
	fi
	mv debian/changelog{.downstream,}
	if [[ -e builddeb.failed ]] ; then
		rm builddeb.failed
		exit 1
	fi
}

doscanpackages() {
	pushd $1
	dpkg-scanpackages . /dev/null | gzip -9 > Packages.gz
	popd
}

doclean() {
	DIST=$1
	ARCH=$2
	git clean -fxd
	dopredebbuild ${DIST}
	pkg_list ../${PACKAGE}_*_${ARCH}.changes | list_rm
	dopostdebbuild ${DIST}
}

builddeb() {
	if [[ `which debuild` ]] ; then
		DEBUILD=debuild
	elif [[ `which dpkg-buildpackage` ]] ; then
		DEBUILD=dpkg-buildpackage
	else
		echo "debuild or dpkg-buildpackage is needed to build the deb file"
		exit 1
	fi

	DIST=$1
	ARCH=$2
	shift
	shift
	doclean ${DIST} ${ARCH}
	dopredebbuild ${DIST}
	if [[ ! -e builddeb.failed ]] ; then
		echo "... building debian packages ($@) ..."
		${DEBUILD} $@ || touch builddeb.failed
	fi
	if [[ ! -e builddeb.failed ]] ; then
		echo "... validating debian packages ..."
		lintian -Ivi ../${PACKAGE}_*.dsc || touch builddeb.failed
	fi
	dopostdebbuild ${DIST}
}

dopbuild() {
	COMMAND=$1
	ARCH=$3

	case "$2" in
		stable|wheezy)
			DIST=debian
			RELEASE=wheezy
			;;
		testing|jessie)
			DIST=debian
			RELEASE=jessie
			;;
		unstable|sid)
			DIST=debian
			RELEASE=sid
			;;
		precise|quantal|raring|saucy|trusty)
			DIST=ubuntu
			RELEASE=$2
			;;
		*)
			echo "Unknown distribution release : $1"
			exit 1
			;;
	esac

	case "${DIST}" in
		debian)
			MIRROR=ftp://mirror.ox.ac.uk/debian/
			KEYRING=/usr/share/keyrings/debian-archive-keyring.gpg
			COMPONENTS=main
			;;
		ubuntu)
			MIRROR=ftp://archive.ubuntu.com/ubuntu/
			KEYRING=/usr/share/keyrings/ubuntu-archive-keyring.gpg
			# NOTE: debfoster is located in universe (required when using sbuild) ...
			COMPONENTS=main,universe
			;;
	esac

	REF=${RELEASE}-${ARCH}
	BASE=${BUILD_DIR}/${REF}
	BASETGZ=${BASE}.tar.gz

	mkdir -pv ${BUILD_DIR}/{debs/${RELEASE},build/${RELEASE}}

	case "${COMMAND}" in
		create|update)
			if [[ -e ${BASETGZ} ]] ; then
				sudo sbuild-update -udcar ${REF}-sbuild
			else
				sudo sbuild-createchroot --arch=${ARCH} --keyring=${KEYRING} --components ${COMPONENTS} --make-sbuild-tarball=${BASETGZ} ${RELEASE} ${BASE} ${MIRROR}
			fi
			;;
		build)
			doclean ${RELEASE} ${ARCH}
			dopredebbuild ${RELEASE}
			VERSION=$(dpkg-parsechangelog|sed -n 's/^Version: \(.*:\|\)//p')
			sbuild --build=${ARCH} --chroot=${REF}-sbuild
			pkg_list_debs ../${PACKAGE}_${VERSION}_${ARCH}.changes | list_mv ${BUILD_DIR}/debs/${RELEASE}
			pkg_list ../${PACKAGE}_${VERSION}_${ARCH}.changes | list_mv ${BUILD_DIR}/build/${RELEASE}
			dopostdebbuild ${RELEASE}
			;;
	esac
}

dorelease() {
	builddeb $1 -S -sa || exit 1
	(sudo pbuilder build ../${PACKAGE}_*.dsc 2>&1 || exit 1) | tee ../${PACKAGE}_build.log
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
	# files, so a "~<distro-name>N" is appended.
	DIST=$1
	VER=`cat debian/changelog | grep ") unstable" | head -n 1 | sed -e "s/.*(//" -e "s/~unstable\([0-9]*\)) unstable;.*/~${DIST}\1/" -e "s/) unstable;.*/~${DIST}1/"`
	builddeb $DIST -S -sa || exit 1
	dput ${DPUT_PPA} ../${PACKAGE}_${VER}_source.changes
}

doallppa() {
	for DISTRO in precise quantal raring saucy trusty ; do
		doppa ${DISTRO}
	done
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

COMMAND=$1
ARG1=$2
ARG2=$3

shift
shift
shift

case "$COMMAND" in
	allppa)    doallppa ;;
	android)   doandroid ;;
	clean)     doclean ${ARG1} ${ARG2} ;;
	deb)       builddeb ${ARG1} ${HOST_ARCH} -us -uc ;;
	debsrc)    builddeb ${ARG1} source -S -sa ;;
	dist)      dodist ;;
	mkimage)   dopbuild create ${ARG1} ${ARG2} ;;
	pbuild)    dopbuild build  ${ARG1} ${ARG2} $@ ;;
	ppa)       doppa ${ARG1} ;;
	release)   dorelease ${ARG1} ;;
	install)   doinstall ;;
	uninstall) douninstall ;;
	help|*)
		echo "usage: `basename $0` <command>

where <command> is one of:

    allppa         Publish to the Cainteoir Text-to-Speech Ubuntu PPA for all supported distributions.
    android        Build the library for android.
    clean <dist> <arch>
                   Clean the build tree and generated files.
    deb <dist>     Create a (development build) debian binary package.
    debsrc <dist>  Create a debian source package.
    dist           Create (and test) a distribution source tarball.
    help           Show this help screen.
    install        Installs the built debian packages.
    mkimage <dist> <arch>
                   Create a pbuilder image.
    pbuild <dist> <arch> <pdebuild-options>
                   Build the debian package under a pbuilder environment.
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

To create signed Ubuntu raring amd64 *.deb packages for use with pbuilder, run:
    `basename $0` pbuild raring amd64 --auto-debsign
    `basename $0` mkimage raring amd64
"
		;;
esac
