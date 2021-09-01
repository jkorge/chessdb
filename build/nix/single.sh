#!/bin/bash

declare -i libchk=0
if [[ "$1" = "parsestream" ]]
then
        libchk=1
fi

if [[ libchk -eq 1 ]]
then
        echo "g++ -std=c++17 -O3 -Winline -flto -fno-fat-lto-objects -o bin/nix/$1.o -c src/$1.cpp"
        g++ -std=c++17 -O3 -Winline -flto -fno-fat-lto-objects -o bin/nix/$1.o -c src/$1.cpp
else
        echo "g++ -std=c++17 -O3 -Winline -mlzcnt -mbmi -mpopcnt -march=x86-64 -flto -fno-fat-lto-objects -o bin/nix/$1.o -c src/$1.cpp"
        g++ -std=c++17 -O3 -Winline -mlzcnt -mbmi -mpopcnt -march=x86-64 -flto -fno-fat-lto-objects -o bin/nix/$1.o -c src/$1.cpp
fi