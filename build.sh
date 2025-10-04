#!/bin/bash

# GANPI Build Script
echo "🧠 Building GANPI - Gemini-Assisted Natural Processing Interface"
echo "================================================================"

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "📋 Configuring build..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
echo "🔨 Building..."
make -j$(nproc)

# Check if build was successful
if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Build successful!"
    echo "🚀 You can now run: ./ganpi --help"
    echo ""
    echo "💡 Quick test: ./ganpi \"list all files in current directory\""
else
    echo ""
    echo "❌ Build failed!"
    echo "Please check the error messages above and ensure all dependencies are installed."
    exit 1
fi
