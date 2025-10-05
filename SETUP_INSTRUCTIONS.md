# 🚀 GANPI - Complete Setup Instructions

## 📋 Prerequisites (Required Dependencies)

Before running `build_windows.bat`, you need these tools installed:

### 1. **CMake** (Required)
- **Download:** https://cmake.org/download/
- **Version:** 3.16 or later
- **Installation:** Choose "Add CMake to system PATH" during installation

### 2. **Visual Studio Build Tools** (Required)
Choose ONE of these options:

#### Option A: Visual Studio 2022 (Recommended)
- **Download:** https://visualstudio.microsoft.com/downloads/
- **Workload:** Select "Desktop development with C++"
- **Components:** Make sure "MSVC v143 compiler toolset" is included

#### Option B: Visual Studio Build Tools 2022 (Lighter)
- **Download:** https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022
- **Workload:** Select "C++ build tools"
- **Components:** Include "MSVC v143 compiler toolset"

## 🎯 Quick Setup (One Command!)

1. **Extract** the GANPI project to any folder
2. **Open Command Prompt** as Administrator (recommended)
3. **Navigate** to the GANPI folder:
   ```cmd
   cd C:\path\to\GANPI
   ```
4. **Run the setup script:**
   ```cmd
   build_windows.bat
   ```

The script will:
- ✅ Check all dependencies
- ✅ Build GANPI from source
- ✅ Set up configuration
- ✅ Show you the PATH to add manually

## 🔧 Setting Up PATH Environment Variable

After running `build_windows.bat`, you need to add GANPI to your PATH:

### **Method 1: Using System Properties (Recommended)**

1. **Press Windows Key + R**, type `sysdm.cpl`, press Enter
2. **Click "Advanced" tab**
3. **Click "Environment Variables"**
4. **In "User variables" section**, find and select `Path`
5. **Click "Edit"**
6. **Click "New"**
7. **Add this path:** `C:\path\to\GANPI\build\Release`
   - Replace `C:\path\to\GANPI` with your actual GANPI folder path
   - Example: `C:\Users\YourName\Desktop\GANPI\build\Release`
8. **Click "OK"** on all dialogs
9. **Restart Command Prompt**

### **Method 2: Using Command Line (Alternative)**

```cmd
# Run this command as Administrator:
setx PATH "%PATH%;C:\path\to\GANPI\build\Release"

# Replace C:\path\to\GANPI with your actual path
# Example:
setx PATH "%PATH%;C:\Users\YourName\Desktop\GANPI\build\Release"
```

**Note:** You'll need to restart Command Prompt after using `setx`.

## 🧪 Testing Your Installation

After PATH setup:

1. **Open a NEW Command Prompt** (important!)
2. **Navigate to any folder:**
   ```cmd
   cd C:\Users\YourName\Documents
   ```
3. **Test GANPI:**
   ```cmd
   ganpi "list all files"
   ganpi --help
   ```

If `ganpi` is not recognized, the PATH wasn't set correctly. Double-check the path you added.

## 🔧 Troubleshooting

### "CMake not found"
- Install CMake from https://cmake.org/download/
- Make sure to add it to PATH during installation
- Restart Command Prompt after installation

### "Visual Studio Build Tools not found"
- Install Visual Studio 2022 or Build Tools 2022
- Make sure to include "C++ build tools" workload
- Restart Command Prompt after installation

### "Build failed"
- Make sure you're running Command Prompt as Administrator
- Check that all prerequisites are installed
- Try running `build_windows.bat` again

### "GANPI not recognized as command"
- The PATH setup might have failed
- Try restarting Command Prompt
- Or manually add to PATH: `C:\path\to\GANPI\build\Release`

## 🎉 What You Get

After successful setup, you can use GANPI from anywhere:

```cmd
# File operations
ganpi "create a backup of all my documents"
ganpi "find all PDF files and zip them"

# System utilities  
ganpi "show me the top 10 processes using RAM"
ganpi "check disk space usage"

# Development
ganpi "initialize a git repository here"
ganpi "start a local web server on port 8080"

# Fun commands
ganpi "tell me a joke"
ganpi "download a random meme"
```

## 📁 Project Structure

```
GANPI/
├── build_windows.bat          # Main setup script
├── SETUP_INSTRUCTIONS.md      # This file
├── .ganpi_config              # Configuration file (auto-created)
├── src/                       # Source code
├── include/                   # Header files
├── CMakeLists.txt            # Build configuration
└── build/                    # Build output (created by script)
    └── Release/
        └── ganpi.exe         # Your executable
```

## 🆘 Need Help?

If you encounter any issues:
1. Make sure all prerequisites are installed
2. Run Command Prompt as Administrator
3. Check that you're in the correct GANPI directory
4. Try running `build_windows.bat` again

The setup script provides detailed error messages to help diagnose any problems!
