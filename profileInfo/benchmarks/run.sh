#!/bin/bash

PATH2LIB="../build/profileInfoPass/profileInfoPass.so"
PASS="profileInfo"

# Delete outputs from previous runs.
rm -f *.bc *.dot .*.dot

# Convert source code to bitcode (IR).
clang -emit-llvm -c ${1}.c -Xclang -disable-O0-optnone -o ${1}.bc

# Canonicalize natural loops (Ref: llvm.org/doxygen/LoopSimplify_8h_source.html)
opt -passes='loop-simplify' ${1}.bc -o ${1}.ls.bc

opt -disable-output -load-pass-plugin="${PATH2LIB}" -passes="${PASS}" ${1}.bc

# Generate CFG visualizations
opt -disable-output -passes="dot-cfg" ${1}.bc
cat .main.dot | dot -Tpdf > ../dot/main.pdf

rm -f *.bc *.dot .*.dot