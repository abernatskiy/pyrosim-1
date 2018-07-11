#!/bin/sh

MAKEOPTS="-j2"
CONFIGUREOPTS="--enable-double-precision --with-cylinder-cylinder=libccd --with-box-cylinder=libccd --with-capsule-cylinder=libccd"

#echo -n "Downloading ode-0.12..." &&
#wget https://sourceforge.net/projects/opende/files/ODE/0.12/ode-0.12.tar.bz2 > /tmp/odewget 2>&1 &&
#echo "done" &&

echo "Changing into simulator directory"
cd ./pyrosim/simulator

echo -n "Unpacking ode-0.12.tar.bz2..." &&
tar -xjf ode-0.12.tar.bz2 &&
echo "done" &&

mkdir -p ./buildlogs

echo -n "Configuring ode-0.12..." &&
cd ode-0.12 &&
./configure $CONFIGUREOPTS > ../buildlogs/odeconfigure 2>&1 &&
cd .. &&
echo "done" &&

echo -n "Spacifying the background..." &&
cp ../../space_stuff/drawstuff-blackness.cpp ode-0.12/drawstuff/src/drawstuff.cpp &&
echo "done" &&

echo -n "Building ode-0.12..." &&
cd ode-0.12 &&
make $MAKEOPTS > ../buildlogs/odemake 2>&1 &&
cd .. &&
echo "done" &&

echo -n "Building simulator..." &&
make $MAKEOPTS > ./buildlogs/pyrosimmake 2>&1 &&
echo "done"
