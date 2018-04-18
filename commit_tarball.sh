#!/bin/bash
DIR=$1
if test "x$DIR" = "x"; then
    echo "No DIR parameter. Aborting."
    exit 1;
fi

VERSION=`grep "AC_INIT" configure.ac | awk -F '[' '{print $3;}' | sed -e 's_)__g;s_\]__g'`
echo "VERSION: $VERSION"

make distclean || true

./rmfiles.sh && autoreconf --install && ./configure && make dist GZIP_ENV="--no-name"
if test  "x$?" != "x0"; then
    exit 1;
fi

cp emdros-$VERSION.tar.gz $DIR && cd $DIR && echo "$VERSION" > latest && git pull && git add emdros-$VERSION.tar.gz latest && git commit -m "Released new Emdros tarball, version $VERSION."  && git push
if test "x$?" != "x0"; then
    exit 1;
fi

exit 0;
