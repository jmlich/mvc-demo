#!/bin/bash

cmake -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
cmake --build build

ctest --test-dir build --output-on-failure
gcovr --root . --filter src/ --html-details build/coverage.html

cp salary.json build/
cd build
LANG=cs_CZ ./mvc-demo
