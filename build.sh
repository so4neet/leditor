#!/bin/bash

compiler="clang"
bin_name="leditor"
cflags="-Wall -Wextra -std=c99"
ldflags="-lSDL2"
srcfiles=$(find src -type f -name "*.c")

$compiler $srcfiles -o bin/$bin_name $cflags $ldflags
