#!/bin/bash

# GANPI Dependencies Installation Script
echo "🧠 Installing GANPI Dependencies"
echo "================================="

# Detect OS
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Linux
    echo "🐧 Detected Linux system"
    
    if command -v apt &> /dev/null; then
        echo "📦 Installing dependencies with apt (Ubuntu/Debian)..."
        sudo apt update
        sudo apt install -y \
            build-essential \
            cmake \
            libcurl4-openssl-dev \
            nlohmann-json3-dev \
            pkg-config \
            git
    elif command -v yum &> /dev/null; then
        echo "📦 Installing dependencies with yum (CentOS/RHEL)..."
        sudo yum groupinstall -y "Development Tools"
        sudo yum install -y \
            cmake \
            libcurl-devel \
            nlohmann-json-devel \
            pkg-config \
            git
    elif command -v pacman &> /dev/null; then
        echo "📦 Installing dependencies with pacman (Arch Linux)..."
        sudo pacman -S --needed \
            base-devel \
            cmake \
            curl \
            nlohmann-json \
            pkg-config \
            git
    else
        echo "❌ Unsupported package manager. Please install dependencies manually:"
        echo "   - build-essential / base-devel"
        echo "   - cmake"
        echo "   - libcurl development libraries"
        echo "   - nlohmann-json"
        echo "   - pkg-config"
        echo "   - git"
        exit 1
    fi

elif [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS
    echo "🍎 Detected macOS system"
    
    if command -v brew &> /dev/null; then
        echo "📦 Installing dependencies with Homebrew..."
        brew install \
            cmake \
            curl \
            nlohmann-json \
            pkg-config
    else
        echo "❌ Homebrew not found. Please install Homebrew first:"
        echo "   /bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""
        exit 1
    fi

elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
    # Windows with MSYS2/Cygwin
    echo "🪟 Detected Windows system with MSYS2/Cygwin"
    
    if command -v pacman &> /dev/null; then
        echo "📦 Installing dependencies with pacman (MSYS2)..."
        pacman -S --needed \
            mingw-w64-x86_64-toolchain \
            mingw-w64-x86_64-cmake \
            mingw-w64-x86_64-curl \
            mingw-w64-x86_64-nlohmann-json \
            mingw-w64-x86_64-pkg-config \
            git
    else
        echo "❌ Please install dependencies manually or use vcpkg"
    fi

else
    echo "❌ Unsupported operating system: $OSTYPE"
    echo "Please install dependencies manually:"
    echo "   - C++17 compiler"
    echo "   - CMake 3.16+"
    echo "   - libcurl"
    echo "   - nlohmann-json"
    echo "   - pkg-config"
    exit 1
fi

echo ""
echo "✅ Dependencies installed successfully!"
echo "🚀 You can now build GANPI with: ./build.sh"
