#!/bin/bash

cmake -Bbuild/release -H. -DCMAKE_BUILD_TYPE=Release
cd $PWD/build/release
make

