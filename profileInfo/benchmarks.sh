#!/bin/bash

cd "build"
make
cd "../benchmarks"

BENCHMARKS="/n/eecs583a/home/janeluo/583-Final-Project/profileInfo/benchmarks"
RUN="/n/eecs583a/home/janeluo/583-Final-Project/profileInfo/benchmarks/run.sh"
UNOPTIMIZED="/n/eecs583a/home/janeluo/583-Final-Project/profileInfo/benchmarks/unoptimizedOutput.txt"
OPTIMIZED="/n/eecs583a/home/janeluo/583-Final-Project/profileInfo/benchmarks/profileOutput.txt"

# List of C files in MiBench
file="../benchmarks/consumer/jpeg/jpeg-6a/cjpeg"

sh run.sh ${file}

# if [ -d "$BENCHMARKS" ]; then
# 	find "$BENCHMARKS" -mindepth 1 -maxdepth 3 -type d | while read -r folder; do
# 		echo "Folder: $(basename $folder)" >> $OPTIMIZED
# 		echo "Folder: $(basename $folder)" >> $UNOPTIMIZED

# 		for file in "$folder"/*.c ; do
# 			if [ -f "$file" ]; then
# 				output=$(sh "$RUN" "${file%.*}" 2>&1)  # Capture both stdout and stderr
#                 # Check if the output contains any warning or error messages
#                 if echo "$output" | grep -qE "(warning|error)"; then
#                     # echo "Removing file: $(basename "$file")"
#                     # rm "$file"
#                     continue
#                 fi
# 				echo "File: $(basename $file)" >> $OPTIMIZED
# 				echo "File: $(basename $file)" >> $UNOPTIMIZED
# 				echo $(basename $file) ${file%.*}
# 				sh $RUN ${file%.*}
# 			else
# 				echo "\n" >> $OPTIMIZED
# 				echo "\n" >> $UNOPTIMIZED
# 			fi
# 		done
# 	done
# 	echo "\n" >> $OPTIMIZED
# else
# 	echo "directory not found: $BENCHMARKS"
# fi