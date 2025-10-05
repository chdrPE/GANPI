@echo off
echo 🧠 Building GANPI for Windows
echo =============================

REM Clean previous build
if exist build rmdir /s /q build
mkdir build
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
    echo Please check the error messages above.
    echo Make sure you have Visual Studio Build Tools installed.
    pause
    exit /b 1
)

cd ..
pause
