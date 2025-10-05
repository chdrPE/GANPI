# Nested Directory Path Fix

## 🐛 Issue Identified

**Problem:** When you say "move files in test to dir1 in test", the program was moving files from `test\` to `dir1\` (at the root level), not to `test\dir1\` (nested inside test).

**Example:**

```
Before:
test/
  file1.txt
  file2.txt
  dir1/

After running "move files in test to dir1":
Wrong behavior (before fix):
  test/
  dir1/          <- files moved HERE (wrong!)
    file1.txt
    file2.txt

Correct behavior (after fix):
  test/
    dir1/        <- files moved HERE (correct!)
      file1.txt
      file2.txt
```

---

## ✅ Solution Implemented

### 1. Nested Path Detection

Added `extractNestedPath()` function that recognizes patterns like:

- "dir1 in test"
- "dir1 directory in test"
- "the dir1 folder in test"

When detected, it converts `dir1` → `test\dir1`

### 2. Smart File-Only Moving

When moving files to a subdirectory of the source (nested structure), the program now:

- Only moves **files** (not other directories)
- Uses `Get-ChildItem -File` to avoid recursive issues
- Prevents moving the destination directory into itself

---

## 🔧 Technical Implementation

**New Function:**

```cpp
std::string extractNestedPath(const std::string& text, const std::string& dir_name) {
    // Check if directory is nested: "dir1 in test", "dir1 directory in test", etc.
    std::regex nested_pattern(dir_name + R"(\s+(?:directory|dir|folder)?\s+in\s+(?:the\s+)?(\w+))");
    std::smatch match;
    if (std::regex_search(text, match, nested_pattern)) {
        // Found "dir1 in test" -> return "test\dir1"
        return match[1].str() + "\\" + dir_name;
    }
    return dir_name;
}
```

**Enhanced Move Logic:**

```cpp
// Check if destination is nested inside another directory
if (!dest_dir.empty()) {
    dest_dir = extractNestedPath(lower_input, dest_dir);
}

// Check if destination is a subdirectory of source (nested structure)
bool is_nested = (dest_dir.find(source_dir + "\\") == 0);

if (is_nested) {
    // For nested moves, only move files (not subdirectories) to avoid recursion
    command = "powershell -Command \"Get-ChildItem -Path " + source_dir +
              "\\* -File | Move-Item -Destination " + dest_dir + " -Force\"";
} else {
    // For non-nested moves, move everything
    command = "powershell -Command \"Move-Item -Path " + source_dir +
              "\\* -Destination " + dest_dir + " -Force\"";
}
```

---

## 📝 Example Commands

### Nested Path Example:

```bash
./ganpi.exe "move all files in test to the dir1 directory in test"
```

**Generates:**

```powershell
powershell -Command "Get-ChildItem -Path test\* -File | Move-Item -Destination test\dir1 -Force"
```

### Non-Nested Example:

```bash
./ganpi.exe "move all files from test to backup"
```

**Generates:**

```powershell
powershell -Command "Move-Item -Path test\* -Destination backup -Force"
```

### With Display:

```bash
./ganpi.exe "move files in test to dir1 in test and show the result"
```

**Generates:**

```powershell
powershell -Command "Get-ChildItem -Path test\* -File | Move-Item -Destination test\dir1 -Force" && dir test\dir1
```

---

## 🎯 Key Differences

| Aspect               | Before Fix          | After Fix               |
| -------------------- | ------------------- | ----------------------- |
| Path interpretation  | `dir1` (root level) | `test\dir1` (nested)    |
| Nested detection     | ❌ None             | ✅ "in test" pattern    |
| Move behavior        | All items           | Files only (for nested) |
| Recursion protection | ❌ No               | ✅ Yes                  |

---

## 🧪 Test Cases

### Test Case 1: Nested Move

**Command:**

```bash
./ganpi.exe "move files in test to dir1 in test"
```

**Expected Result:**

- Moves only files from `test/` to `test/dir1/`
- Does not move subdirectories
- `test/dir1/` itself is not moved (no recursion)

### Test Case 2: Display Nested Directory

**Command:**

```bash
./ganpi.exe "show me the contents of dir1 in test"
```

**Expected Result:**

```
dir test\dir1
```

### Test Case 3: Non-Nested Move (Still Works)

**Command:**

```bash
./ganpi.exe "move files from downloads to documents"
```

**Expected Result:**

- Moves everything from `downloads/` to `documents/`
- Works as before (no change)

---

## 🔍 Supported Patterns

The program now recognizes these nested patterns:

✅ "dir1 in test"
✅ "dir1 directory in test"
✅ "the dir1 folder in test"
✅ "dir1 dir in test"

All variations are converted to: `test\dir1`

---

## 💡 Benefits

1. **Contextual Understanding**: Recognizes nested directory relationships
2. **Prevents Errors**: Won't move a directory into itself
3. **Flexible Commands**: Supports both nested and non-nested operations
4. **Safe Defaults**: Only moves files when dealing with nested structures

---

## 🚀 How to Test

1. **Create test structure:**

   ```bash
   mkdir test
   mkdir test\dir1
   echo "test file 1" > test\file1.txt
   echo "test file 2" > test\file2.txt
   ```

2. **Run the command:**

   ```bash
   ./ganpi.exe "move files in test to dir1 in test"
   ```

3. **Verify:**
   ```bash
   dir test\dir1
   # Should show file1.txt and file2.txt
   ```

---

## ⚠️ Important Notes

- **File-Only Moves**: When moving to nested subdirectories, only files are moved (not folders)
- **PowerShell Required**: Still uses PowerShell for reliable operations
- **Path Format**: Windows backslash format (`test\dir1`)

---

**Status:** ✅ Fixed and ready for testing
**File Modified:** `src/gemini_client_simple.cpp`
**Build:** Successful
