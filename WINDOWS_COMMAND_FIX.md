# Windows Command Execution Fix

## 🐛 Issue Encountered

**Error Message:**

```
Cannot move multiple files to a single file.
```

**Original Command:**

```bash
move test\* dir1\ && dir dir1
```

**Problem:** The Windows `cmd.exe` `move` command doesn't handle wildcards properly when moving multiple files. The syntax `move test\* dir1\` fails on Windows because `cmd.exe` interprets the wildcard incorrectly.

---

## ✅ Solution Implemented

### Use PowerShell for File Operations

**New Command:**

```powershell
powershell -Command "Move-Item -Path test\* -Destination dir1 -Force" && dir dir1
```

### Why This Works:

1. **PowerShell's Move-Item**: Properly handles wildcards (`*`) for multiple files
2. **-Force Parameter**: Overwrites existing files without prompting
3. **Proper Wildcard Handling**: PowerShell expands `test\*` correctly
4. **Command Chaining**: The `&&` still works to chain with `dir dir1`

---

## 🔧 Technical Changes

**File Modified:** `src/gemini_client_simple.cpp`

**Change:**

```cpp
// BEFORE (doesn't work on Windows):
std::string command = "move " + source_dir + "\\* " + dest_dir + "\\";

// AFTER (works correctly):
std::string command = "powershell -Command \"Move-Item -Path " +
                      source_dir + "\\* -Destination " + dest_dir + " -Force\"";
```

---

## 📝 Testing

### Test Command:

```bash
./ganpi.exe "Can you move all the files in the test directory into the dir1 directory and then display the contents of dir1"
```

### Generated Command:

```powershell
powershell -Command "Move-Item -Path test\* -Destination dir1 -Force" && dir dir1
```

### What It Does:

1. ✅ Moves all files from `test\` to `dir1\` using PowerShell
2. ✅ If successful, displays contents of `dir1`
3. ✅ Handles multiple files correctly
4. ✅ Overwrites existing files without prompting

---

## 🎯 Comparison: CMD vs PowerShell

| Feature          | CMD `move` | PowerShell `Move-Item` |
| ---------------- | ---------- | ---------------------- |
| Wildcard support | ❌ Buggy   | ✅ Proper              |
| Multiple files   | ❌ Fails   | ✅ Works               |
| Error handling   | ❌ Poor    | ✅ Good                |
| Force overwrite  | ❌ Limited | ✅ `-Force` flag       |

---

## 🚀 Additional PowerShell Commands Used

The program now uses PowerShell for other operations too:

### Zip Files:

```powershell
powershell Compress-Archive -Path *.pdf -DestinationPath documents.zip
```

### Copy Files:

```powershell
powershell -Command "Copy-Item -Path source\* -Destination dest -Force"
```

---

## 💡 Benefits

1. **Cross-platform consistency**: PowerShell is available on Windows 7+ and modern Windows systems
2. **Better error handling**: PowerShell provides clearer error messages
3. **More features**: Access to PowerShell's rich file manipulation cmdlets
4. **Future-proof**: PowerShell is the modern Windows scripting standard

---

## 🧪 Example Use Cases That Now Work

```bash
# Move files
./ganpi.exe "move all files from test to dir1"
# Generates: powershell -Command "Move-Item -Path test\* -Destination dir1 -Force"

# Move and display
./ganpi.exe "move files from downloads to documents and show result"
# Generates: powershell -Command "Move-Item -Path downloads\* -Destination documents -Force" && dir documents

# Complex operations
./ganpi.exe "move all txt files from temp to backup"
# Would generate appropriate PowerShell command
```

---

## ⚠️ Requirements

- **PowerShell**: Must be installed (comes pre-installed on Windows 7+)
- **Execution Policy**: May need to allow script execution (usually not an issue for `-Command`)

If you get execution policy errors, run PowerShell as Administrator and execute:

```powershell
Set-ExecutionPolicy RemoteSigned -Scope CurrentUser
```

---

## 🔮 Future Improvements

1. Add error detection for missing PowerShell
2. Provide CMD fallback for simple operations
3. Add progress indicators for large file operations
4. Support for `-WhatIf` to preview operations
5. Better handling of paths with spaces

---

**Status:** ✅ Fixed and tested
**Build:** Success
**Windows Compatibility:** ✅ Improved
