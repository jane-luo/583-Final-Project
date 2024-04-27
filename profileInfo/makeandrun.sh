#!/bin/bash

cd "build"
make

cd "../benchmarks"
sh run.sh test