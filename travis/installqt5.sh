#!/bin/bash

echo Downloading Qt
set -v
set -e

#DOWNLOAD_URL=https://download.qt.io/archive/qt/5.8/5.8.0/qt-opensource-mac-x64-clang-5.8.0.dmg
DOWNLOAD_URL=http://mirror.netcologne.de/qtproject/archive/qt/5.8/5.8.0/qt-opensource-mac-x64-clang-5.8.0.dmg
INSTALLER=`basename $DOWNLOAD_URL`
INSTALLER_NAME=${INSTALLER%.*}
ENVFILE=qt-5.8.0-osx.env
APPFILE=/Volumes/${INSTALLER_NAME}/${INSTALLER_NAME}.app/Contents/MacOS/${INSTALLER_NAME}
echo $INSTALLER_NAME
echo $APPFILE

wget -c $DOWNLOAD_URL
hdiutil mount ${INSTALLER}
ls /Volumes
echo Installing Qt
chmod +x travis/extract-qt-installer
./travis/extract-qt-installer $APPFILE $PWD/Qt

echo Create $ENVFILE
cat << EOF > $ENVFILE
export PATH=$PWD/Qt/5.8/clang_64/bin:$PATH
EOF
