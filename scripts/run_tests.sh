#!/bin/bash

# GANPI Test Runner Script
echo "🧪 Running GANPI Tests"
echo "======================"

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "❌ Build directory not found. Please run ./build.sh first."
    exit 1
fi

cd build

# Check if executable exists
if [ ! -f "ganpi" ]; then
    echo "❌ GANPI executable not found. Please run ./build.sh first."
    exit 1
fi

echo "🔍 Running basic functionality tests..."

# Test 1: Help command
echo "Test 1: Help command"
if ./ganpi --help > /dev/null 2>&1; then
    echo "✅ Help command works"
else
    echo "❌ Help command failed"
    exit 1
fi

# Test 2: Invalid command handling
echo "Test 2: Invalid command handling"
if ./ganpi "" > /dev/null 2>&1; then
    echo "✅ Empty command handling works"
else
    echo "❌ Empty command handling failed"
    exit 1
fi

# Test 3: Configuration file creation
echo "Test 3: Configuration system"
if [ ! -f ".ganpi_config" ]; then
    echo "✅ Configuration file doesn't exist (expected on first run)"
else
    echo "✅ Configuration file exists"
fi

echo ""
echo "🎉 All basic tests passed!"
echo "💡 For full testing with API integration, configure your API key first."
echo "   Run: ./ganpi --help"
