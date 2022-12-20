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
# There are then four cases:
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
# 4) Otherwise, try to clone the GitHub emdros/emdros repo, then make
# a tarball, and try to deb-build it.
#
# The last option is useful if the script is used in a context where
# it is downloaded by itself, then invoked without the rest of the
# sources.
#


#
# Parameter 1 is used as the EMDROS_VERSION if present.
# 
if test "x$1" != "x"; then
    EMDROS_VERSION="$1"
fi

if test "x$EMDROS_VERSION" = "x" -a test -f configure.ac ; then
    # Find the Emdros version from configure.ac
    EMDROS_VERSION=`grep "AC_INIT" configure.ac | awk -F ',' '{print \$2}' | sed -e 's_\\[__g;s_\\]__g;s_,__g'`
fi

echo "EMDROS_VERSION = '${EMDROS_VERSION}'"

# Do we have a tarball in the current directory?
if test -f emdros-${EMDROS_VERSION}.tar.gz; then
    # Yes, we do have the tarball.
    
    # Unpack it, and build the package.
    tar xfzv emdros-${EMDROS_VERSION}.tar.gz && ( cd emdros-${EMDROS_VERSION} && dpkg-buildpackage -rfakeroot -d -us -uc && cd .. )
elif test -f ../emdros-${EMDROS_VERSION}.tar.gz; then
    # We have the tarball in the parent directory.

    # Copy it to the current directory
    cp ../emdros-${EMDROS_VERSION}.tar.gz .

    # Unpack it, and build the package.
    tar xfzv emdros-${EMDROS_VERSION}.tar.gz && ( cd emdros-${EMDROS_VERSION} && dpkg-buildpackage -rfakeroot -d -us -uc && cd .. )
elif test -f configure.ac; then
    # No, we don't have the tarball, but we do have sources.  Make the
    # tarball. Then try to build a .deb from it.
    
    # First make sure we are pristine
    make distclean
    ./rmfiles.sh

    # Then rebuild, make the tarball, unpack it, and build the .deb
    autoreconf -i && ./configure && make dist && tar xfzv emdros-${EMDROS_VERSION}.tar.gz && ( cd emdros-${EMDROS_VERSION} && dpkg-buildpackage -rfakeroot -d -us -uc && cd .. )
else
    # No, we don't have the tarball, not do we have sources.  git
    # clone the repoi. Make the tarball. Then try to build a .deb from
    # it.

    # Get the PID of the subshell in which this script is running.
    PID="$$"

    # Make a build dir
    BUILD_DIR=${HOME}/build/emdros-build-$PID
    mkdir -p ${BUILD_DIR}
    cd ${BUILD_DIR}

    echo ""
    echo "Did not find tarball or sources."
    echo "... Cloning into ${BUILD_DIR}/."
    echo ""

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
    autoreconf -i && ./configure && make dist && tar xfzv emdros-${EMDROS_VERSION}.tar.gz && ( cd emdros-${EMDROS_VERSION} && dpkg-buildpackage -rfakeroot -d -us -uc && cd .. )

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

