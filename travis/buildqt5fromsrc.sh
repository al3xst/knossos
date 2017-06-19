#!/bin/bash

set +x +e

TAR=qt-everywhere-opensource-src-5.8.0
wget http://download.qt.io/archive/qt/5.8/5.8.0/single/$TAR.tar.xz
echo Unpacking....
tar -xf $TAR.tar.xz
cd $TAR

NOFB="-no-directfb -no-linuxfb -no-eglfs -no-kms"
SQL="-no-sql-mysql -no-sql-sqlite"
NOSSE="-no-sse2 -no-sse3 -no-ssse3 -no-sse4.1 -no-sse4.2 -no-avx -no-avx2"
#CONF_PREFIX="-developer-build"
#CONF_PREFIX="-prefix $PWD/qtbase"
./configure $CONF_PREFIX -nomake examples -nomake tests -opensource -confirm-license \
  -no-gtkstyle -no-glib -no-cups -no-nis \
        $SQL $NOFB $NOSSE 

make -j
make install
#checkinstall
