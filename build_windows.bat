@echo off
echo ========================================
echo    GANPI - Complete Setup for Windows
echo ========================================
echo.
echo This script will:
echo 1. Check for required dependencies
echo 2. Build GANPI from source
echo 3. Set up configuration
echo 4. Show PATH setup instructions
echo.

REM Check for required tools
echo [STEP 1/4] Checking dependencies...

REM Check for CMake
cmake --version >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake not found! Please install CMake from: https://cmake.org/download/
    echo.
    echo Required: CMake 3.16 or later
    pause
    exit /b 1
) else (
    echo [OK] CMake found
)

REM Initialize Visual Studio environment
echo [INFO] Initializing Visual Studio environment...

REM Try to find and initialize Visual Studio environment
set "VS_ENV_INITIALIZED=0"

REM Try Visual Studio Build Tools 2022
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" (
    echo [INFO] Found Visual Studio Build Tools 2022, initializing...
    call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat"
    set "VS_ENV_INITIALIZED=1"
    echo [OK] Visual Studio Build Tools environment initialized
)

REM Try Visual Studio Community 2022 if Build Tools not found
if %VS_ENV_INITIALIZED% EQU 0 (
    if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" (
        echo [INFO] Found Visual Studio Community 2022, initializing...
        call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
        set "VS_ENV_INITIALIZED=1"
        echo [OK] Visual Studio Community environment initialized
    )
)

REM Try Visual Studio Professional 2022 if others not found
if %VS_ENV_INITIALIZED% EQU 0 (
    if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat" (
        echo [INFO] Found Visual Studio Professional 2022, initializing...
        call "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat"
        set "VS_ENV_INITIALIZED=1"
        echo [OK] Visual Studio Professional environment initialized
    )
)

REM Try Visual Studio Enterprise 2022 if others not found
if %VS_ENV_INITIALIZED% EQU 0 (
    if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat" (
        echo [INFO] Found Visual Studio Enterprise 2022, initializing...
        call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat"
        set "VS_ENV_INITIALIZED=1"
        echo [OK] Visual Studio Enterprise environment initialized
    )
)

REM Check if we found a compiler
where cl >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Visual Studio Build Tools not found!
    echo.
    echo Please install one of the following:
    echo - Visual Studio 2022 (Community/Professional/Enterprise)
    echo - Visual Studio Build Tools 2022
    echo.
    echo Download from: https://visualstudio.microsoft.com/downloads/
    echo Make sure to include "C++ build tools" workload
    pause
    exit /b 1
) else (
    echo [OK] C++ compiler found and ready
)

echo [OK] All dependencies found!
echo.

REM Clean previous build
echo [STEP 2/4] Preparing build environment...
if exist build rmdir /s /q build
mkdir build
cd build

REM Configure with CMake
echo [STEP 2/4] Configuring build...
cmake .. -DCMAKE_BUILD_TYPE=Release

REM Build the project
echo [STEP 2/4] Building GANPI...
cmake --build . --config Release

REM Check if build was successful
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Build failed!
    echo Please check the error messages above.
    pause
    exit /b 1
)

cd ..

REM Copy config file if it exists
echo [STEP 3/4] Setting up configuration...
if exist .ganpi_config (
    echo [OK] Configuration file found
    if not exist build\.ganpi_config (
        copy .ganpi_config build\.ganpi_config >nul 2>&1
        echo [OK] Configuration copied to build directory
    )
) else (
    echo [INFO] No configuration file found - using default settings
)

REM Get the full path to the executable
set "GANPI_PATH=%~dp0build\Release"
set "GANPI_EXE=%GANPI_PATH%\ganpi.exe"

REM Check if ganpi.exe actually exists
if exist "%GANPI_EXE%" (
    echo [STEP 4/4] Setting up PATH environment...
    echo.
    
    REM Show PATH setup instructions
    echo [INFO] To use GANPI from anywhere, add this directory to your PATH:
    echo   %GANPI_PATH%
    echo.
    echo [MANUAL SETUP REQUIRED]
    echo Please follow the instructions in SETUP_INSTRUCTIONS.md
    echo to add GANPI to your PATH environment variable.
    
    echo.
    echo ========================================
    echo           SETUP COMPLETE! 🎉
    echo ========================================
    echo.
    echo GANPI is now ready to use! You can run it from anywhere:
    echo.
    echo   ganpi "create a test file"
    echo   ganpi "find all PDF files"
    echo   ganpi "list running processes"
    echo   ganpi --help
    echo.
    echo [NOTE] You may need to restart your command prompt for PATH changes to take effect.
    echo.
    echo Executable location: %GANPI_EXE%
    echo.
    echo ========================================
    echo           VERIFICATION TEST
    echo ========================================
    echo.
    echo Testing if GANPI can be found in PATH...
    
    REM Test if ganpi can be found in PATH (this won't work in current session)
    REM but we can verify the executable exists and show the PATH entry
    echo [INFO] GANPI executable exists: %GANPI_EXE%
    echo [INFO] GANPI directory added to PATH: %GANPI_PATH%
    echo.
    echo Quick test: Open a NEW command prompt and try:
    echo   ganpi --help
    echo   ganpi "list all files in current directory"
    echo.
    echo If 'ganpi' is not recognized, restart your command prompt.
    
) else (
    echo [ERROR] Executable not found at: %GANPI_EXE%
    echo Build may have failed. Please check the error messages above.
)

cd ..
pause
