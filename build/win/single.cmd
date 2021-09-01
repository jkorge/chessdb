@echo off

set "_libchk=0"
if "%1" == "parsestream" set "_libchk=1"

if %_libchk% EQU 1 (
    echo g++ -std=c++17 -O3 -Winline -flto -fno-fat-lto-objects -o bin/win/%1.o -c src/%1.cpp
    g++ -std=c++17 -O3 -Winline -flto -fno-fat-lto-objects -o bin/win/%1.o -c src/%1.cpp
) else (
    echo g++ -std=c++17 -O3 -Winline -mlzcnt -mbmi -mpopcnt -march=x86-64 -flto -fno-fat-lto-objects -o bin/win/%1.o -c src/%1.cpp
    g++ -std=c++17 -O3 -Winline -mlzcnt -mbmi -mpopcnt -march=x86-64 -flto -fno-fat-lto-objects -o bin/win/%1.o -c src/%1.cpp
)