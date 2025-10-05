@echo off
echo Building GANPI for Windows
echo =============================

REM Clean previous build
if exist build rmdir /s /q build
mkdir build
cd build

REM Configure with CMake using MinGW generator
echo Configuring build...
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

REM Build the project
echo Building...
cmake --build . --config Release

REM Copy config file if it exists in parent directory
if exist ..\.ganpi.config (
    echo Copying config file...
    copy ..\.ganpi.config .ganpi.config >nul 2>&1
)

REM Check if build was successful
if %ERRORLEVEL% EQU 0 (
    echo.
    echo [SUCCESS] Build successful!
    echo You can now run: .\ganpi.exe --help
    echo.
    echo Quick test: .\ganpi.exe "list all files in current directory"
    echo.
    echo Executable location: build\ganpi.exe
) else (
    echo.
    echo [ERROR] Build failed!
    echo Please check the error messages above.
    echo Make sure you have Visual Studio Build Tools installed.
    pause
    exit /b 1
)

cd ..
pause
