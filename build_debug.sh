#!/bin/bash

cmake -Bbuild/debug -H. -DCMAKE_BUILD_TYPE=Debug
cd $PWD/build/debug
make

