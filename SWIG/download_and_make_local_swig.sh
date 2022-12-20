#!/bin/bash
#
# This script downloads a specific version of SWIG, makes it, and
# installs it in ${HOME}/opt/swig-${SWIG_VERSION}, and adds symlinks
# to this swig to ${HOME}/opt/bin
#
# You should then add ${HOME}/opt/bin to your PATH, e.g., in
# ~/.bashrc.
#
# USAGE:
#
# ./download_and_make_local_swig.sh SWIG_VERSION
#
# The swig version to get must be given as the first parameter.
# Other parameters are ignored.
#

# 
if test "x$1" = "x"; then
    echo "ERROR: Must give SWIG_VERSION as the first parameter."
    echo "       ./download_and_make_local_swig.sh SWIG_VERSION"
    echo ""
    exit 1;
else
    SWIG_VERSION="$1"
fi

PID="$$"

BUILD_DIR=${HOME}/build/swig-$PID
mkdir -p ${BUILD_DIR}

cd ${BUILD_DIR}
SWIG_TAR_FILENAME=swig-${SWIG_VERSION}.tar.gz
rm -f ${SWIG_TAR_FILENAME}
rm -f download
wget https://sourceforge.net/projects/swig/files/swig/swig-${SWIG_VERSION}/${SWIG_TAR_FILENAME}/download

tar xfvz download
cd swig-${SWIG_VERSION}

SWIG_PREFIX=${HOME}/opt/swig-${SWIG_VERSION}

rm -rf ${SWIG_PREFIX}

./configure --prefix=${SWIG_PREFIX} && make && make install

if test "x$?" != "x0"; then
    echo "ERROR: Compilation failed."
    exit 1
else
    echo "SUCCESS: Installed in ${SWIG_PREFIX}"
    echo ""
fi

EXEC_DIR=${HOME}/opt/bin

SYMLINK_SWIG="${SWIG_PREFIX}/bin/swig ${EXEC_DIR}/"
SYMLINK_CCACHE_SWIG="${SWIG_PREFIX}/bin/ccache-swig ${EXEC_DIR}/"

rm -f ${EXEC_DIR}/swig ${EXEC_DIR}/ccache-swig
ln -s ${SYMLINK_SWIG}
ln -s ${SYMLINK_CCACHE_SWIG}

echo "SUCCESS: Created symlinks:"
echo "ln -s ${SYMLINK_SWIG}"
echo "ln -s ${SYMLINK_CCACHE_SWIG}"
echo ""








