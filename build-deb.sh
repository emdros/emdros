#!/bin/bash
#
# You can use this script to build a .deb on Ubuntu / Debian.
#
# Run it, either directly from a cloned git repo, or from an untar'ed
# release tarball.
#
# USAGE:
#
# ./build-deb.sh [EMDROS_VERSION]
#
# If an argument is given, this is used as the EMDROS_VERSION. It is
# used to try to find a tarball with the name
# emdros-${EMDROS_VERSION}.tar.gz.
#
# If no argument is given, and the file configure.ac is present in the
# current directory, the EMDROS_VERSION is extracted from that file.
#
# There are then five cases:
#
# 1) If the file 'emdros-${EMDROS_VERSION}.tar.gz' exists, it is
# unpacked, and the script attempts to build a .deb out of it.
#
# 2) Otherwise, if the file '../emdros-${EMDROS_VERSION}.tar.gz' exists, it is
# unpacked, and the script attempts to build a .deb out of it.
#
# 3) Otherwise, if the file configure.ac exists in the current
# directory, then take the sources there, clean them up, make a new
# tarball, and try to deb-build that.
#
# 4) Otherwise, if the EMDROS_VERSION given is non-empty, the script
# attempts to download an official release for that version from the
# Emdros website. If this succeeds, it deb-builds that.
#
# 5) Otherwise, try to clone the GitHub emdros/emdros repo, then make
# a tarball, and try to deb-build it.
#
# The last two options are useful if the script is used in a context
# where it is downloaded by itself, then invoked without the rest of
# the sources.
#
#
# You can pass switches to the configure script by setting the
# environment variable:
#
# - EMDROS_CONFIGURE_SWITCHES
#
# before invocation of this script.

# For example:
#
# export EMDROS_CONFIGURE_SWITCHES="--with-mysql=no --with-postgresql=no --with-swig-language-nodejs=yes" && bash build-deb.sh
#
# See the file INSTALL in the root of the sources for other configure
# switches.
#
#



#
# Parameter 1 is used as the EMDROS_VERSION if present.
# 
if test "x$1" != "x"; then
    EMDROS_VERSION="$1"
fi

if test "x$EMDROS_VERSION" = "x" -a -f configure.ac ; then
    # Find the Emdros version from configure.ac
    EMDROS_VERSION=`grep "AC_INIT" configure.ac | awk -F ',' '{print \$2}' | sed -e 's_\\[__g;s_\\]__g;s_,__g'`
fi

echo "EMDROS_VERSION = '${EMDROS_VERSION}'"

function build_tarball {
    cd ${BUILD_DIR}
    if test -f emdros-${EMDROS_VERSION}.tar.gz; then
	tar xfzv emdros-${EMDROS_VERSION}.tar.gz && ( cd emdros-${EMDROS_VERSION} && dpkg-buildpackage -rfakeroot -d -us -uc && cd .. )
    else
	"FAILURE: Could not find ${BUILD_DIR}/emdros-${EMDROS_VERSION}.tar.gz"
	exit 1;
    fi
}

function test_is_gzipped_tarball_delete_if_not {
    # See if the tarball exists
    if test -f emdros-${EMDROS_VERSION}.tar.gz; then
	# Yes, it exists. See if it is a gzip file.
	FILE_EMDROS_TARBALL_IS_GZIP=`file emdros-${EMDROS_VERSION}.tar.gz | grep "gzip compressed data"`
	if test "x${FILE_EMDROS_TARBALL_IS_GZIP}" = "x"; then
	    # No, it was not a gzip file. Probably a failed download.
	    #
	    # Remove it.
	    rm -f emdros-${EMDROS_VERSION}.tar.gz
	fi
    fi
}

function download_tarball {
    # Do we have an Emdros version?
    if test "x${EMDROS_VERSION}" != "x"; then
	# Yes.

	# So, first attempt to download the sources from the Emdros
	# website.

	# First, we see if we have curl available
	WHICH_CURL=`which curl`
	NO_CURL_IN=`echo ${WHICH_CURL} | grep "no curl in"`
	if test "x${WHICH_CURL}" != "x" -a "x${NO_CURL_IN}" = "x"; then
	    # We have curl. Attempt to download
	    ${WHICH_CURL} -L -o emdros-$EMDROS_VERSION.tar.gz https://emdros.org/downloads/emdros/emdros-$EMDROS_VERSION.tar.gz
	    test_is_gzipped_tarball_delete_if_not;
	else
	    # Did not have curl. Do we have wget?
	    WHICH_WGET=`which wget`
	    NO_WGET_IN=`echo ${WHICH_WGET} | grep "no wget in"`
	    if test "x${WHICH_WGET}" != "x" -a "x${NO_WGET_IN}" = "x"; then
		# We have wget. Attempt to download
		${WHICH_WGET} https://emdros.org/downloads/emdros/emdros-$EMDROS_VERSION.tar.gz
		test_is_gzipped_tarball_delete_if_not;
	    fi
	fi
    fi
}

# Get the PID of the subshell in which this script is running.
PID="$$"

# Make a build dir
BUILD_DIR=${HOME}/build/emdros-build-$PID
mkdir -p ${BUILD_DIR}


# Do we have a tarball in the current directory?
if test -f emdros-${EMDROS_VERSION}.tar.gz; then
    # Yes, we do have the tarball.

    # Copy it into BUILD_DIR
    cp emdros-${EMDROS_VERSION}.tar.gz ${BUILD_DIR}/
    
    # Unpack it, and build the package.
    build_tarball;
elif test -f ../emdros-${EMDROS_VERSION}.tar.gz; then
    # We have the tarball in the parent directory.

    # Copy it into BUILD_DIR
    cp ../emdros-${EMDROS_VERSION}.tar.gz ${BUILD_DIR}/

    # Unpack it, and build the package.
    build_tarball;
elif test -f configure.ac; then
    # No, we don't have the tarball, but we do have sources.  Make the
    # tarball. Then try to build a .deb from it.
    
    # First make sure we are pristine
    make distclean
    ./rmfiles.sh

    # Then rebuild, make the tarball, unpack it, and build the .deb
    autoreconf -i && ./configure ${EMDROS_CONFIGURE_SWITCHES} && make dist && tar xfzv emdros-${EMDROS_VERSION}.tar.gz && ( cd emdros-${EMDROS_VERSION} && dpkg-buildpackage -rfakeroot -d -us -uc && cd .. )
else
    # No, we don't have the tarball, nor do we have sources.

    download_tarball;

    # Were we able to download a tarball?
    if test -f emdros-${EMDROS_VERSION}.tar.gz; then
	# Yes, we now have the tarball.

	# Copy it into BUILD_DIR
	cp emdros-${EMDROS_VERSION}.tar.gz ${BUILD_DIR}/
	
	# Unpack it, and build the package.
	build_tarball;
    else
	# No, we did not download a tarball.

	# So, git clone the repo. Make the tarball. Then try to build a .deb from
	# it.

	echo ""
	echo "Did not find tarball or sources."
	echo "... Cloning into ${BUILD_DIR}/."
	echo ""

	cd ${BUILD_DIR}

	git clone https://github.com/emdros/emdros.git

	cd emdros
	
	# Find the emdros version
	if test -f configure.ac ; then
	    # Find the Emdros version from configure.ac
	    EMDROS_VERSION=`grep "AC_INIT" configure.ac | awk -F ',' '{print \$2}' | sed -e 's_\\[__g;s_\\]__g;s_,__g'`
	fi
	
	# First make sure we are pristine
	make distclean
	./rmfiles.sh
	
	# Then rebuild, make the tarball, unpack it, and build the .deb
	autoreconf -i && ./configure ${EMDROS_CONFIGURE_SWITCHES} && make dist && tar xfzv emdros-${EMDROS_VERSION}.tar.gz && ( cd emdros-${EMDROS_VERSION} && dpkg-buildpackage -rfakeroot -d -us -uc && cd .. )
	
	DEB_FILE=`ls ${BUILD_DIR}/emdros/*.deb`
	
	echo ""
	echo "SUCCESS"
	echo "Built in ${BUILD_DIR}/emdros"
	echo ""
	echo "Perhaps do:"
	echo ""
	echo "sudo dpkg -i ${DEB_FILE}"
	echo ""
    fi
fi

