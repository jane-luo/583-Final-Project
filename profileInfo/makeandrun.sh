!/bin/bash


cd "build"
make

cd "../benchmarks"
sh run.sh test

sh viz.sh test
sh viz.sh test.fplicm

rm -f default.profraw *_prof *_fplicm *.bc *.profdata *_output *.ll