#!/bin/bash

PATH2LIB="../build/combinedPass/combinedPass.so"
PASS="combined"
UNOPTIMIZED="unoptimizedOutput.txt"
OPTIMIZED="staticOutput.txt"
CFG_DOT="${1}_cfg.dot"
CFG_PDF="${1}_cfg.pdf"

rm -f *.bc *_output

# Convert source code to bitcode (IR).
clang -emit-llvm -c ${1}.c -Xclang -disable-O0-optnone -o ${1}.bc

# Apply LLVM passes to add static metadata
opt -load-pass-plugin="${PATH2LIB}" -passes="${PASS}" ${1}.bc -o ${1}.static.bc

# Generate binary executable
clang ${1}.static.bc -o ${1}_static

# Produce output from binary to check correctness
./${1}_static > static_output

# Measure performance
echo -e "Performance of code with static metadata"
time ./${1}_static > /dev/null
# { time ./${1}_static ; } 2> ${OPTIMIZED}
echo -e "\n\n"

# Cleanup: Remove this if you want to retain the created files. And you do need to.
# rm -f default.profraw *_prof *_fplicm *.bc *.profdata *_output *.ll


# #!/bin/bash

# PATH2LIB="../build/combinedPass/combinedPass.so"
# PASS="combined"
# UNOPTIMIZED="unoptimizedOutput.txt"
# OPTIMIZED="staticOutput.txt"

# # Delete outputs from previous runs. Update this when you want to retain some files.
# rm -f *.bc *_output

# # Convert source code to bitcode (IR).
# clang -emit-llvm -c ${1}.c -Xclang -disable-O0-optnone -o ${1}.bc

# # Canonicalize natural loops (Ref: llvm.org/doxygen/LoopSimplify_8h_source.html)
# opt -passes='loop-simplify' ${1}.bc -o ${1}.ls.bc



# # Include metadata from static analyzer
# opt -load-pass-plugin="${PATH2LIB}" -passes="${PASS}" ${1}.ls.bc -o ${1}.static.bc

# # Generate binary executable without optimization
# clang ${1}.ls.bc -o ${1}_no_static

# # Generate binary executable with static analyzer metadata embedded
# clang ${1}.static.bc -o ${1}_static

# # Produce output from binary to check correctness
# ./${1}_with_static_metadata > static_output

# # Measure performance
# # echo -e "1. Performance of code without optimization"
# # { time ./${1}_no_optimization ; } 2> ${UNOPTIMIZED}
# # echo -e "\n\n"

# # echo -e "2. Performance of code with static analyzer metadata"
# # { time ./${1}_with_static_metadata ; } 2> ${OPTIMIZED}
# # echo -e "\n\n"

# echo -e "1. Performance of code without optimization"
# { time ./${1}_no_static ; } > /dev/null
# echo -e "\n\n"

# echo -e "2. Performance of code with static analyzer metadata"
# { time ./${1}_static ; } > /dev/null
# echo -e "\n\n"

# #!/bin/bash

# PATH2LIB="../build/combinedPass/combinedPass.so"
# PASS="combined"

# # Delete outputs from previous runs.
# # rm -f *.bc *.dot .*.dot

# # Convert source code to bitcode (IR).
# clang -emit-llvm -c ${1}.c -Xclang -disable-O0-optnone -o ${1}.bc

# # Canonicalize natural loops (Ref: llvm.org/doxygen/LoopSimplify_8h_source.html)
# opt -passes='loop-simplify' ${1}.bc -o ${1}.ls.bc

# # Run the pass
# opt -disable-output -load-pass-plugin="${PATH2LIB}" -passes="${PASS}" ${1}.bc

# # Generate CFG visualizations
# opt -disable-output -passes="dot-cfg" ${1}.bc
# cat .main.dot | dot -Tpdf > dot/main.pdf

# # rm -f *.bc *.dot .*.dot
