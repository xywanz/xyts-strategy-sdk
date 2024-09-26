#!/bin/bash

cd `dirname $0`/../lib

for ((i=2; i<=$#; i++)); do
    target="lib${!i}.so"
    if [ ! -e "$target" ]; then
        ln -s "lib${1}.so" "$target"
    fi
done
