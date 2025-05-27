#!/bin/bash

echo "Compiling Tetris..."
g++ main.cpp src/*.cpp -Iinclude -o tetris.out

if [ $? -ne 0 ]; then
    echo "Compilation failed."
else
    echo "Compilation successful. Output: tetris.out"
fi
