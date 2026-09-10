#!/bin/bash

compiler="clang"
bin_name="leditor"
cflags="-Wall -Wextra -std=c99"
ldflags="-lSDL3 -lSDL3_ttf -lfontconfig"
srcfiles=$(find src -type f -name "*.c")

$compiler $srcfiles -o bin/$bin_name $cflags $ldflags
