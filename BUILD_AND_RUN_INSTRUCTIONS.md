# GANPI - Build and Run Instructions

## ✅ Compilation Successful!

The project has been successfully compiled.

---

## 📦 Build Commands

### Using the Build Script (Recommended for Windows)

```bash
# From the project root directory:
./build.bat
```

### Manual Build (Cross-platform)

```bash
# 1. Create build directory
mkdir build
cd build

# 2. Configure with CMake (using MinGW on Windows)
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# 3. Build the project
cmake --build . --config Release

# 4. The executable will be in: build/ganpi.exe
```

---

## 🚀 Running GANPI

### From the build directory:

```bash
./ganpi.exe --help              # Show help information
./ganpi.exe --interactive       # Start interactive mode
./ganpi.exe "your command here" # Execute a single command
```

### From anywhere (add to PATH):

```bash
# Copy the executable to a directory in your PATH, or run from project root:
build/ganpi.exe "list all files"
```

---

## 📝 Configuration

The program stores its configuration in: `C:\Users\javie\.ganpi_config`

**Config file format:**

```
GEMINI_API_KEY=your_api_key_here
MODEL=gemini-pro
```

---

## 💡 Example Commands

```bash
# File Management
ganpi.exe "list all files in current directory"
ganpi.exe "find all PDF files"
ganpi.exe "move txt files to Documents"

# Development
ganpi.exe "start HTTP server on port 8080"

# System Information
ganpi.exe "show top 10 processes using most RAM"
```

---

## 🔧 Key Fixes Applied

1. **Config File Location**: Changed to read from user's home directory (`%USERPROFILE%\.ganpi_config`)
2. **File I/O**: Switched to C-style file operations for better Windows compatibility
3. **Memory Management**: Changed from reference to pointer for Config singleton
4. **Error Handling**: Added robust exception handling throughout

---

## 📂 Project Structure

```
GANPI/
├── build/              # Build output directory
│   └── ganpi.exe      # Compiled executable
├── src/               # Source files
├── include/           # Header files
├── CMakeLists.txt     # CMake configuration
└── .ganpi_config      # Configuration file (copy to home directory)
```

---

## 🐛 Troubleshooting

**If the program can't find the config file:**

- Make sure `.ganpi_config` is in your home directory: `C:\Users\javie\.ganpi_config`
- Or create it by running ganpi.exe once and following the setup prompts

**If you need to rebuild:**

```bash
rm -rf build
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

---

**Status:** ✅ Project compiled and working successfully!
**Location:** `C:/Users/javie/OneDrive/Attachments/coding projects/Stormhacks/GANPI/build/ganpi.exe`
