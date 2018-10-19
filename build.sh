#!/bin/bash

set -euo pipefail

_gcc_dir=obj.gcc
_clang_dir=obj.clang

mkdir -p "$_gcc_dir"
pushd "$_gcc_dir"
export C=gcc
export CXX=g++
cmake -DCMAKE_BUILD_TYPE=Release ..
make
popd

mkdir -p "$_clang_dir"
pushd "$_clang_dir"
export C=clang
export CXX=clang++
cmake -DCMAKE_BUILD_TYPE=Release ..
make
popd
