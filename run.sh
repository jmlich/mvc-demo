#!/bin/bash

mkdir build
cd build

cmake ..
cmake --build .
cp ../salary.json .

LANG=cs_CZ ./mvc-demo