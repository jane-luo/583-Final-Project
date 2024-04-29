#!/bin/bash

cd "build"
make
cd "../benchmarks"

BENCHMARKS="../benchmarks"
RUN="run.sh"
UNOPTIMIZED="unoptimizedOutput.txt"
OPTIMIZED="profileOutput.txt"

if [ -d "$BENCHMARKS" ]; then
	find "$BENCHMARKS" -mindepth 1 -maxdepth 3 -type d | while read -r folder; do
		echo "Folder: $(basename $folder)" >> $OPTIMIZED
		echo "Folder: $(basename $folder)" >> $UNOPTIMIZED
		for file in "$folder"/*.c ; do
			if [ -f "$file" ]; then
				output=$(sh "$RUN" "${file%.*}" 2>&1)  # Capture both stdout and stderr
                # Check if the output contains any warning or error messages
                if echo "$output" | grep -qE "(warning|error)"; then
                    # echo "Removing file: $(basename "$file")"
                    # rm "$file"
                    break
                fi
				echo "File: $(basename $file)" >> $OPTIMIZED
				echo "File: $(basename $file)" >> $UNOPTIMIZED
				echo $(basename $file) ${file%.*}
				sh $RUN ${file%.*}
			else
				echo "\n" >> $OPTIMIZED
				echo "\n" >> $UNOPTIMIZED
			fi
		done
	done
	echo "\n" >> $OPTIMIZED
else
	echo "directory not found: $BENCHMARKS"
fi