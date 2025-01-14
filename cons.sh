#!/bin/bash

cmake -S . -B build
cmake --build build/ 
# copy compile commands
cp build/compile_commands.json .

if [ $# -ne 0 ] && [ $1 = "run" ]; then
    ./build/chitralok res/lenna.jpg
fi
