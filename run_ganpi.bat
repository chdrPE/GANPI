@echo off
echo 🧠 Starting GANPI...
echo ==================

REM Change to build directory where the executable and config are located
cd build

REM Run ganpi with all passed arguments
.\ganpi.exe %*

REM Return to original directory
cd ..
