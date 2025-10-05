@echo off
echo Building GANPI for Windows
echo =============================

REM Clean previous build
if exist build rmdir /s /q build
mkdir build
cd build

REM Configure with CMake using MinGW generator
echo Configuring build...
cmake .. -DCMAKE_BUILD_TYPE=Release

REM Build the project
echo Building...
cmake --build . --config Release

REM Check if build was successful first
if %ERRORLEVEL% EQU 0 (
    echo.
    echo [SUCCESS] Build successful!
    echo.
    
    REM Copy config file if it exists in parent directory
    if exist ..\.ganpi_config (
        echo Copying config file...
        copy ..\.ganpi_config .ganpi_config >nul 2>&1
        copy ..\.ganpi_config ..\.ganpi_config >nul 2>&1
    )
    
    REM Get the full path to the executable
    set "GANPI_PATH=%~dp0build\Release"
    set "GANPI_EXE=%GANPI_PATH%\ganpi.exe"
    
    REM Check if ganpi.exe actually exists
    if exist "%GANPI_EXE%" (
        echo.
        echo [SETUP] Adding GANPI to system PATH...
        echo.
        
        REM Add to user PATH (permanent)
        echo Adding to user PATH: %GANPI_PATH%
        setx PATH "%PATH%;%GANPI_PATH%" >nul 2>&1
        
        if %ERRORLEVEL% EQU 0 (
            echo [SUCCESS] GANPI added to PATH successfully!
            echo.
            echo You can now use 'ganpi' from anywhere:
            echo   ganpi "create a test file"
            echo   ganpi "find all PDF files"
            echo   ganpi --help
            echo.
            echo [NOTE] You may need to restart your command prompt for PATH changes to take effect.
        ) else (
            echo [WARNING] Could not add to PATH automatically.
            echo You can manually add this path to your PATH environment variable:
            echo   %GANPI_PATH%
        )
    ) else (
        echo [ERROR] Executable not found at: %GANPI_EXE%
    )
    
    echo.
    echo Executable location: %GANPI_EXE%
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
