#!/bin/bash
#
# You can use this script to build a .deb on Ubuntu / Debian.
#
# Run it, either directly from a cloned git repo, or from an untar'ed
# release tarball.
#

# Find the Emdros version from configure.ac
EMDROS_VERSION=`grep "AC_INIT" configure.ac | awk -F ',' '{print \$2}' | sed -e 's_\\[__g;s_\\]__g;s_,__g'`
echo "EMDROS_VERSION = ${EMDROS_VERSION}"

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
else
    # No, we don't have the tarball.  Make it.
    
    # First make sure we are pristine
    make distclean
    ./rmfiles.sh

    # Then rebuild, make the tarball, unpack it, and build the .deb
    autoreconf -i && ./configure && make dist && tar xfzv emdros-${EMDROS_VERSION}.tar.gz && ( cd emdros-${EMDROS_VERSION} && dpkg-buildpackage -rfakeroot -d -us -uc && cd .. )
fi

