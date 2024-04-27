#!/bin/bash

cd "build"
make
cd "../benchmarks"

BENCHMARKS="../benchmarks"
RUN="run.sh"
OUTPUT="../profileOutput.txt"

if [ -d "$BENCHMARKS" ]; then
	find "$BENCHMARKS" -mindepth 1 -maxdepth 3 -type d | while read -r folder; do
		echo "Folder: $(basename $folder)" >> $OUTPUT
		for file in "$folder"/*.c ; do
			if [ -f "$file" ]; then
				echo "File: $(basename $file)" >> $OUTPUT
				echo $(basename $file) ${file%.*}
				# output=$(sh "$RUN" "${file%.*}" 2>&1)  # Capture both stdout and stderr
                # # Check if the output contains any warning or error messages
                # if echo "$output" | grep -qE "(warning|error)"; then
                #     echo "Removing file: $(basename "$file")"
                #     rm "$file"
                # fi
				sh $RUN ${file%.*}
			else
				echo "\n" >> $OUTPUT
			fi
		done
	done
	echo "\n" >> $OUTPUT
else
	echo "directory not found: $BENCHMARKS"
fi