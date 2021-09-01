#!/bin/bash

rm -f lib/nix/*.a

while IFS='' read -r LINE || [ -n "${LINE}" ]; do
        echo "${LINE}"
        eval "${LINE}"
done < build/nix/_src.txt

while IFS='' read -r LINE || [ -n "${LINE}" ]; do
        echo "${LINE}"
        eval "${LINE}"
done < build/nix/_static.txt

rm -f bin/nix/*.o