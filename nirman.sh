#!/bin/bash

cmake -S . -B build
cmake --build build/ 

if [ $# -ne 0 ] && [ $1 = "run" ]; then
    ./build/chitralok
fi
