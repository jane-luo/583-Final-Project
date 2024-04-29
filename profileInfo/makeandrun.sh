#!/bin/bash

TEST="automotive/qsort/qsort_large"

cd "build"
make

cd "../benchmarks"
sh run.sh ${TEST}

sh viz.sh ${TEST}
sh viz.sh ${TEST}.fplicm

rm -f default.profraw *_prof *_fplicm *.bc *.profdata *_output *.ll