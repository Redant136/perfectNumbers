#!/bin/bash

cd "./gmp-6.2.1"
./configure
make
make check
make install