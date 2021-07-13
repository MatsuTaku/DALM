#!/bin/sh

cd $(dirname $0)
CMAKE_ROOT=$PWD/..
echo $CMAKE_ROOT

if [ ! -e bin ]; then
    mkdir bin
fi
cd bin
cmake -DCMAKE_CXX_COMPILER=c++-10 -DDALM_NEW_XCHECK=OFF -DDALM_EL_SKIP=OFF -DDALM_CNV_XCHECK=OFF -DDALM_PLAIN_XCHECK=OFF $CMAKE_ROOT
cmake --build .
cd ..

if [ ! -e pbin ]; then
    mkdir pbin
fi
cd pbin
cmake -DCMAKE_CXX_COMPILER=c++-10 -DDALM_NEW_XCHECK=OFF -DDALM_EL_SKIP=OFF -DDALM_CNV_XCHECK=OFF -DDALM_PLAIN_XCHECK=ON $CMAKE_ROOT
cmake --build .
cd ..

if [ ! -e nbin ]; then
    mkdir nbin
fi
cd nbin
cmake -DCMAKE_CXX_COMPILER=c++-10 -DDALM_NEW_XCHECK=ON -DDALM_EL_SKIP=OFF -DDALM_CNV_XCHECK=OFF -DDALM_PLAIN_XCHECK=OFF $CMAKE_ROOT
cmake --build .
cd ..

if [ ! -e n1bin ]; then
    mkdir n1bin
fi
cd n1bin
cmake -DCMAKE_CXX_COMPILER=c++-10 -DDALM_NEW_XCHECK=ON -DDALM_EL_SKIP=ON -DDALM_CNV_XCHECK=OFF -DDALM_PLAIN_XCHECK=OFF $CMAKE_ROOT
cmake --build .
cd ..

if [ ! -e ncnvbin ]; then
    mkdir ncnvbin
fi
cd ncnvbin
cmake -DCMAKE_CXX_COMPILER=c++-10 -DDALM_NEW_XCHECK=ON -DDALM_EL_SKIP=ON -DDALM_CNV_XCHECK=ON -DDALM_PLAIN_XCHECK=OFF $CMAKE_ROOT
cmake --build .
cd ..

