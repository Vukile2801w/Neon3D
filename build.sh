#!/bin/bash

if [ "$1" == "--delete" ]; then
    rm -rf build
fi

cmake -S . -B build -G Ninja
cmake --build build