#!/bin/bash

cd "build"
make

cd "../benchmarks"
sh run.sh eCalculate

# sh viz.sh test
# sh viz.sh test.superblocks

# rm -f default.profraw *_prof *_superblocks *.bc *.profdata *_output *.ll