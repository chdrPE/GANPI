# Unix-Style Commands Update

## ✅ Changed to Unix Commands

The program now generates **Unix-style commands** (`ls`, `mv`, `rm`, etc.) instead of Windows commands. This works perfectly in **Git Bash** and is more portable!

---

## 📝 Command Mapping

| Operation      | Old (Windows)                 | New (Unix)                     |
| -------------- | ----------------------------- | ------------------------------ |
| List files     | `dir`                         | `ls -la`                       |
| Move files     | `move` or PowerShell          | `mv`                           |
| Find files     | `dir *.pdf /s`                | `find . -name '*.pdf' -type f` |
| Delete files   | `del /s /q`                   | `rm -rf`                       |
| Zip files      | `powershell Compress-Archive` | `zip`                          |
| Path separator | `\` (backslash)               | `/` (forward slash)            |

---

## 🎯 Example Commands

### 1. **Move Files (Nested)**

```bash
./ganpi.exe "move files in test to dir1 in test"
```

**Generates:**

```bash
find test/ -maxdepth 1 -type f -exec mv {} dir1/ \;
```

- Uses `find` to get only files (not directories)
- Moves them one by one to avoid recursion issues
- Unix-style paths with forward slashes

---

### 2. **Move Files (Non-Nested)**

```bash
./ganpi.exe "move files from downloads to backup"
```

**Generates:**

```bash
mv downloads/* backup/
```

- Simple `mv` command
- Moves all files and directories

---

### 3. **Move and Display**

```bash
./ganpi.exe "move files in test to dir1 in test and show the result"
```

**Generates:**

```bash
find test/ -maxdepth 1 -type f -exec mv {} test/dir1/ \; && ls -la test/dir1
```

- Moves files using `find`
- Then lists contents with `ls -la`

---

### 4. **List Directory Contents**

```bash
./ganpi.exe "show me what's in dir1"
```

**Generates:**

```bash
ls -la dir1
```

**Nested directory:**

```bash
./ganpi.exe "list files in dir1 in test"
```

**Generates:**

```bash
ls -la test/dir1
```

---

### 5. **Find PDF Files**

```bash
./ganpi.exe "find all PDF files"
```

**Generates:**

```bash
find . -name '*.pdf' -type f
```

- Searches recursively from current directory
- Only finds files (not directories)

---

### 6. **Zip Files**

```bash
./ganpi.exe "zip all PDF files"
```

**Generates:**

```bash
zip documents.zip *.pdf
```

---

### 7. **Delete Temporary Files**

```bash
./ganpi.exe "delete all temporary files"
```

**Generates:**

```bash
rm -rf *.tmp
```

---

### 8. **Start HTTP Server**

```bash
./ganpi.exe "start HTTP server"
```

**Generates:**

```bash
python -m http.server 8080
```

- Same on all platforms!

---

## 💡 Benefits of Unix Commands

### ✅ **Works in Git Bash**

All commands work natively in your Git Bash environment on Windows

### ✅ **Cross-Platform**

Same commands work on:

- Windows (with Git Bash/WSL)
- macOS
- Linux

### ✅ **More Powerful**

Unix commands offer more flexibility:

- `find` with complex filters
- `ls -la` for detailed listings
- Proper wildcard handling

### ✅ **Standard Syntax**

Industry-standard commands that developers know

---

## 🔧 Key Technical Changes

**File Modified:** `src/gemini_client_simple.cpp`

### Path Separators

```cpp
// Before:
return match[1].str() + "\\" + dir_name;  // Windows backslash

// After:
return match[1].str() + "/" + dir_name;   // Unix forward slash
```

### Move Commands

```cpp
// Before (PowerShell):
"powershell -Command \"Move-Item -Path test\\* -Destination dir1 -Force\""

// After (Unix):
"mv test/* dir1/"
```

### List Commands

```cpp
// Before:
"dir " + dir_name

// After:
"ls -la " + dir_name
```

### Find Commands

```cpp
// Before:
"dir *.pdf /s"

// After:
"find . -name '*.pdf' -type f"
```

---

## 🧪 Quick Test Examples

```bash
# Test in your build directory:
cd build

# Test move command
./ganpi.exe "move files from test to backup"
# Output: mv test/* backup/

# Test list command
./ganpi.exe "list files in current directory"
# Output: ls -la

# Test nested paths
./ganpi.exe "show contents of dir1 in test"
# Output: ls -la test/dir1

# Test find
./ganpi.exe "find all PDF files"
# Output: find . -name '*.pdf' -type f

# Test compound operations
./ganpi.exe "move files in test to dir1 in test and display contents"
# Output: find test/ -maxdepth 1 -type f -exec mv {} test/dir1/ \; && ls -la test/dir1
```

---

## 📊 Comparison: Before vs After

### Before (Windows):

```bash
./ganpi.exe "move files in test to dir1"
# Generated: powershell -Command "Move-Item -Path test\* -Destination dir1 -Force"
# Issues: Long, Windows-specific, PowerShell required
```

### After (Unix):

```bash
./ganpi.exe "move files in test to dir1"
# Generated: mv test/* dir1/
# Benefits: Short, clean, cross-platform, works in Git Bash
```

---

## ⚠️ Important Notes

1. **Git Bash Compatibility**: All commands work in Git Bash on Windows
2. **Forward Slashes**: Uses `/` instead of `\` for paths
3. **Standard Tools**: Uses `ls`, `mv`, `rm`, `find`, etc.
4. **No PowerShell**: No longer dependent on PowerShell

---

## 🚀 Ready to Use!

The program is now rebuilt with Unix-style commands. Try it out:

```bash
cd build
./ganpi.exe "move files in test to dir1 in test"
```

You'll see clean, Unix-style commands that work perfectly in your Git Bash environment! 🎉

---

**Status:** ✅ Complete
**Build:** Successful
**Compatibility:** Git Bash, WSL, macOS, Linux
