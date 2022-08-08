#!/bin/bash
VAR=$1
DIR=$(dirname "${VAR}")/
find ${DIR} -name "*.c" -o -name "*.h" -o -name "*.cpp" > cscope.files
