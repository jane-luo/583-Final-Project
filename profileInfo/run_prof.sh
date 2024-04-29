#!/bin/bash

# Ensure LLVM tools are in your PATH
PATH2LIB="../../llvm-project/build/lib/profileInfoPass.so"
PASS=profileInfo

# Define your program's source and executable names
SOURCE="your_program.c"
INSTRUMENTED_EXEC="your_program_instrumented"
OPTIMIZED_EXEC="your_program_optimized"
PROFILE_RAW="default.profraw"
PROFILE_DATA="default.profdata"

# 1. Compile with instrumentation
clang -fprofile-instr-generate -o $INSTRUMENTED_EXEC $SOURCE

# Check for compilation success
if [ $? -ne 0 ]; then
    echo "Compilation with instrumentation failed."
    exit 1
fi

# 2. Run the instrumented executable
./$INSTRUMENTED_EXEC
if [ $? -ne 0 ]; then
    echo "Execution of instrumented program failed."
    exit 1
fi

# 3. Merge the profiling data
llvm-profdata merge -sparse $PROFILE_RAW -o $PROFILE_DATA
if [ $? -ne 0 ]; then
    echo "Profiling data merge failed."
    exit 1
fi

# 4. Recompile the program with profile use
clang -fprofile-instr-use=$PROFILE_DATA -o $OPTIMIZED_EXEC $SOURCE
if [ $? -ne 0 ]; then
    echo "Recompilation with profile data failed."
    exit 1
fi

# 5. Run the custom LLVM pass (ensure your pass is built and loadable)
opt -load /path/to/your/pass/BBFreqPass.so -profinfo -S $SOURCE > /dev/null
if [ $? -ne 0 ]; then
    echo "Running LLVM pass failed."
    exit 1
fi

# 6. Generate CFGs including branch frequencies
opt -dot-cfg-only -profinfo -S $SOURCE > /dev/null
if [ $? -ne 0 ]; then
    echo "CFG generation failed."
    exit 1
fi

echo "Process completed successfully. CFG files generated."
