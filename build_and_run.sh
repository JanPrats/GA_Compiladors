#!/bin/bash

set -e  # stop on first error

echo "🧹 Cleaning old files..."
rm -rf build
rm -f scanner
rm -f prova.cscn

echo "📁 Creating build directory..."
mkdir build
cd build

echo "⚙️ Running CMake..."
cmake ..

echo "🔨 Building project..."
make

echo "📦 Moving scanner executable..."
cd src/scanner
mv scanner ../../../

echo "🚀 Running scanner..."
cd ../../../
./scanner prova.c

echo "✅ Done"
