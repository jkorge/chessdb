@echo off

echo g++ -std=c++17 -O3 -mlzcnt -mbmi -mpopcnt -march=x86-64 -flto -fuse-linker-plugin -Llib/win -o bin/win/engine src/engine.cpp icon.res -lchess -lparsestream
g++ -std=c++17 -O3 -mlzcnt -mbmi -mpopcnt -march=x86-64 -flto -fuse-linker-plugin -Llib/win -o bin/win/engine src/engine.cpp icon.res -lchess -lparsestream