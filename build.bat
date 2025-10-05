@echo off
REM GANPI Build Script for Windows
echo 🧠 Building GANPI - Gemini-Assisted Natural Processing Interface
echo ================================================================

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with CMake
echo 📋 Configuring build...
cmake .. -DCMAKE_BUILD_TYPE=Release

REM Build the project
echo 🔨 Building...
cmake --build . --config Release

REM Check if build was successful
if %ERRORLEVEL% EQU 0 (
    echo.
    echo ✅ Build successful!
    echo 🚀 You can now run: Release\ganpi.exe --help
    echo.
    echo 💡 Quick test: Release\ganpi.exe "list all files in current directory"
    echo.
    echo 📁 Executable location: build\Release\ganpi.exe
) else (
    echo.
    echo ❌ Build failed!
    echo Please check the error messages above and ensure all dependencies are installed.
    echo.
    echo 📦 For Windows, you may need to install:
    echo    - Visual Studio Build Tools or Visual Studio
    echo    - vcpkg for package management
    echo    - CMake
    exit /b 1
)

cd ..
pause
