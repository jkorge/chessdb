#!/bin/bash

echo "g++ -std=c++17 -O3 -mlzcnt -mbmi -mpopcnt -march=x86-64 -flto -fuse-linker-plugin -Llib/nix -o bin/nix/engine src/engine.cpp -lchess -lparsestream -lpthread"
g++ -std=c++17 -O3 -mlzcnt -mbmi -mpopcnt -march=x86-64 -flto -fuse-linker-plugin -Llib/nix -o bin/nix/engine src/engine.cpp -lchess -lparsestream -lpthread