#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

CXX_LIBS="-ldl ${SCRIPT_DIR}/libgmp.a -pthread"

g++ -O3 --std=c++11 ${CXX_LIBS} main.cpp -o main