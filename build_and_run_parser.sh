#!/bin/bash

set -e  # stop on first error

echo "🧹 Cleaning old files..."
rm -rf build
rm -f parser
rm -f prova_p3dbg.txt

echo "📁 Creating build directory..."
mkdir build
cd build

### NOT-DEBUG ###
echo "⚙️ Running CMake..."
cmake ..
#################

### DEBUG ###
# echo "⚙️ Running CMake with debug symbols..."
# cmake .. -DCMAKE_C_FLAGS="-g"
#############


echo "🔨 Building project..."
make

echo "📦 Moving parser executable..."
cd src/parser
mv parser ../../../


### NOT-DEBUG ###
echo "🚀 Running parser..."
cd ../../../
./parser prova.cscn src/parser/language.txt
#################


### DEBUG ###
# echo "🚀 Running parser with GDB..."
# cd ../../../
# gdb -batch -ex "run prova.cscn src/parser/language.txt" -ex "bt" ./parser
#############

echo "✅ Done"