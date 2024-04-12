#!/bin/bash
# Place this script in the benchmarks folder and run it using the name of the file (without the file type)

# ACTION NEEDED: If the path is different, please update it here.
PATH2LIB="/n/eecs583a/home/janeluo/583-Final-Project/hazardAvoidance/build/hazardAvoidancePass/hazardAvoidancePass.so"        # Specify your build directory in the project
FUNCNAME="hazardAvoidance"
TEST="test"

rm -f default.profraw *_prof *_fplicm *.bc *.profdata *_output *.ll

clang -emit-llvm -c ${TEST}.c -Xclang -disable-O0-optnone -o ${TEST}.ll

opt -disable-output -load-pass-plugin=${PATH2LIB} -passes=${FUNCNAME} ${TEST}.ll