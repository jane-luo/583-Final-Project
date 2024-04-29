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
# clang ${1}.ls.bc -o ${1}_no_optimization

# # Generate binary executable with static analyzer metadata embedded
# clang ${1}.static.bc -o ${1}_with_static_metadata

# # Produce output from binary to check correctness
# ./${1}_with_static_metadata > static_metadata_output

# # Measure performance
# echo -e "1. Performance of code without optimization"
# { time ./${1}_no_optimization ; } 2> ${UNOPTIMIZED}
# echo -e "\n\n"

# echo -e "2. Performance of code with static analyzer metadata"
# { time ./${1}_with_static_metadata ; } 2> ${OPTIMIZED}
# echo -e "\n\n"


#!/bin/bash

PATH2LIB="../build/combinedPass/combinedPass.so"
PASS="combined"

# Delete outputs from previous runs.
# rm -f *.bc *.dot .*.dot

# Convert source code to bitcode (IR).
clang -emit-llvm -c ${1}.c -Xclang -disable-O0-optnone -o ${1}.bc

# Canonicalize natural loops (Ref: llvm.org/doxygen/LoopSimplify_8h_source.html)
opt -passes='loop-simplify' ${1}.bc -o ${1}.ls.bc

# Run the pass
opt -disable-output -load-pass-plugin="${PATH2LIB}" -passes="${PASS}" ${1}.bc

# Generate CFG visualizations
opt -disable-output -passes="dot-cfg" ${1}.bc
cat .${1}.dot | dot -Tpdf > ../dot/${1}.pdf

# rm -f *.bc *.dot .*.dot