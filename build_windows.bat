@echo off
echo 🧠 Building GANPI for Windows (Simplified Version)
echo ===================================================

REM Clean previous build
if exist build rmdir /s /q build
mkdir build
cd build

REM Configure with Windows-specific CMake
echo 📋 Configuring build...
cmake .. -f ../CMakeLists_windows.txt -DCMAKE_BUILD_TYPE=Release

REM Build the project
echo 🔨 Building...
cmake --build . --config Release

REM Check if build was successful
if %ERRORLEVEL% EQU 0 (
    echo.
    echo ✅ Build successful!
    echo 🚀 You can now run: ganpi.exe --help
    echo.
    echo 💡 Quick test: ganpi.exe "list all files in current directory"
) else (
    echo.
    echo ❌ Build failed!
    echo Please check the error messages above.
    exit /b 1
)

cd ..
