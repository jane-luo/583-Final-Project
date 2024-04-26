#!/bin/bash

PATH2LIB="../build/combinedPass/combinedPass.so"
PASS="combined"

# Delete outputs from previous runs.
rm -f *.bc *.dot .*.dot

# Convert source code to bitcode (IR).
clang -emit-llvm -c ${1}.c -Xclang -disable-O0-optnone -o ${1}.bc

# Canonicalize natural loops (Ref: llvm.org/doxygen/LoopSimplify_8h_source.html)
opt -passes='loop-simplify' ${1}.bc -o ${1}.ls.bc

opt -disable-output -load-pass-plugin="${PATH2LIB}" -passes="${PASS}" ${1}.bc

# Generate CFG visualizations
opt -disable-output -passes="dot-cfg" ${1}.bc
cat .main.dot | dot -Tpdf > dot/main.pdf

rm -f *.bc *.dot .*.dot

# #!/bin/bash

# PATH2LIB="../build/hazardAvoidancePass/hazardAvoidancePass.so"
# PASS="hazardAvoidance"

# # Delete outputs from previous runs. Update this when you want to retain some files.
# rm -f default.profraw *_prof *_superblocks *.bc *.profdata *_output *.ll

# # Convert source code to bitcode (IR).
# clang -emit-llvm -c ${1}.c -Xclang -disable-O0-optnone -o ${1}.bc

# # Canonicalize natural loops (Ref: llvm.org/doxygen/LoopSimplify_8h_source.html)
# opt -passes='loop-simplify' ${1}.bc -o ${1}.ls.bc

# # Instrument profiler passes.
# opt -passes='pgo-instr-gen,instrprof' ${1}.ls.bc -o ${1}.ls.prof.bc

# # Note: We are using the New Pass Manager for these passes! 

# # Generate binary executable with profiler embedded
# clang -fprofile-instr-generate ${1}.ls.prof.bc -o ${1}_prof

# # When we run the profiler embedded executable, it generates a default.profraw file that contains the profile data.
# ./${1}_prof > correct_output

# # Converting it to LLVM form. This step can also be used to combine multiple profraw files,
# # in case you want to include different profile runs together.
# llvm-profdata merge -o ${1}.profdata default.profraw

# # The "Profile Guided Optimization Use" pass attaches the profile data to the bc file.
# opt -passes="pgo-instr-use" -o ${1}.profdata.bc -pgo-test-profile-file=${1}.profdata < ${1}.ls.prof.bc > /dev/null

# # We now use the profile augmented bc file as input to your pass.
# opt -load-pass-plugin="${PATH2LIB}" -passes="${PASS}" ${1}.profdata.bc -o ${1}.superblocks.bc > /dev/null

# # Generate binary excutable before superblock formation: Unoptimzed code
# clang ${1}.ls.bc -o ${1}_no_superblocks
# # Generate binary executable after superblock formation: Optimized code
# clang ${1}.superblocks.bc -o ${1}_superblocks

# # Produce output from binary to check correctness
# ./${1}_superblocks > superblocks_output

# echo -e "\n=== Program Correctness Validation ==="
# if [ "$(diff correct_output superblocks_output)" != "" ]; then
#     echo -e ">> Outputs do not match\n"
# else
#     echo -e ">> Outputs match\n"
#     # Measure performance
#     echo -e "1. Performance of unoptimized code"
#     time ./${1}_no_superblocks > /dev/null
#     echo -e "\n\n"
#     echo -e "2. Performance of optimized code"
#     time ./${1}_superblocks > /dev/null
#     echo -e "\n\n"
# fi