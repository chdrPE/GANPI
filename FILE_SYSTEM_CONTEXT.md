# File System Context Feature

## ✨ New Feature: Automatic Context Gathering

GANPI now **automatically analyzes your file system** before suggesting commands, giving Gemini AI (or the command parser) context about what files and directories actually exist!

---

## 🎯 What This Does

Before generating any command suggestion, GANPI now:

1. **Detects directories mentioned** in your query (test, dir1, downloads, etc.)
2. **Lists the contents** of those directories
3. **Shows current working directory**
4. **Displays this context** to help generate accurate commands

---

## 📝 Example Output

### Your Command:

```bash
./ganpi.exe "move files in test to dir1 in test"
```

### What You'll See:

```
🧠 G.A.N.P.I. - Gemini-Assisted Natural Processing Interface

🔧 Initializing GANPI...
✅ GANPI initialized successfully!

🧠 Processing: "move files in test to dir1 in test"

📂 Analyzing file system context...

=== FILE SYSTEM CONTEXT ===
Current directory: /c/Users/javie/OneDrive/Attachments/coding projects/Stormhacks/GANPI

Contents of test/:
total 8
drwxr-xr-x 1 javie 197609   0 Oct  4 16:30 .
drwxr-xr-x 1 javie 197609   0 Oct  4 16:30 ..
drwxr-xr-x 1 javie 197609   0 Oct  4 16:30 dir1
-rw-r--r-- 1 javie 197609  15 Oct  4 16:30 file1.txt
-rw-r--r-- 1 javie 197609  15 Oct  4 16:30 file2.txt

Contents of dir1/:
total 0
drwxr-xr-x 1 javie 197609 0 Oct  4 16:30 .
drwxr-xr-x 1 javie 197609 0 Oct  4 16:30 ..
===========================

🔧 GANPI suggests this command:
   $ find test/ -maxdepth 1 -type f -exec mv {} test/dir1/ \;

🔍 Command to execute:
   find test/ -maxdepth 1 -type f -exec mv {} test/dir1/ \;

   Execute? (Y/n):
```

---

## 🔍 How It Works

### 1. **Directory Detection**

The system automatically detects directory names in your query using pattern matching:

```cpp
// Recognized directory names:
- test, dir1, dir2
- downloads, download
- documents, document
- backup, temp
- home, desktop
```

### 2. **Content Listing**

For each detected directory, it runs:

```bash
ls -la [directory] 2>/dev/null | head -20
```

This shows:

- All files and subdirectories
- File permissions
- File sizes
- Modification times
- First 20 items only (to avoid clutter)

### 3. **Current Directory**

Always includes:

```bash
pwd  # Current working directory
```

### 4. **Context Display**

All this information is displayed in an organized format before the command suggestion.

---

## 💡 Benefits

### ✅ **Accurate Commands**

Gemini AI can see what files actually exist and generate more precise commands

### ✅ **Prevents Errors**

You can verify the context matches your expectation before executing

### ✅ **Better Understanding**

See exactly what the AI "sees" when generating commands

### ✅ **Debugging Help**

If a command seems wrong, you can see what context led to it

---

## 🎯 Use Cases

### 1. **Moving Files**

```bash
./ganpi.exe "move files in test to dir1"
```

**Context shows:**

- What files are in `test/`
- If `dir1/` exists and what's in it
- Current directory path

**Result:** Accurate move command that won't fail

---

### 2. **Organizing Files**

```bash
./ganpi.exe "organize my downloads folder"
```

**Context shows:**

- All files in `downloads/`
- File types present
- Current organization

**Result:** Smart organization based on actual file types

---

### 3. **Finding Files**

```bash
./ganpi.exe "find all PDFs in documents"
```

**Context shows:**

- Current documents structure
- Subdirectories that might contain PDFs

**Result:** Targeted search in relevant directories

---

### 4. **Current Directory Operations**

```bash
./ganpi.exe "list all files in current directory"
```

**Context shows:**

- Current directory contents
- Working directory path

**Result:** Appropriate listing command

---

## 🔧 Technical Implementation

### File: `src/gemini_client_simple.cpp` & `src/gemini_client.cpp`

**Helper Functions:**

```cpp
// Execute command and capture output
std::string executeAndCapture(const std::string& cmd);

// Gather file system context
std::string getFileSystemContext(const std::string& query);
```

**Integration:**

```cpp
std::string GeminiClient::interpretCommand(const std::string& natural_language) {
    // 1. Gather context
    std::string context = getFileSystemContext(natural_language);

    // 2. Display to user
    std::cout << "📂 Analyzing file system context..." << std::endl;
    std::cout << context << std::endl;

    // 3. Use context for command generation
    // (In full version, sent to Gemini API)
    // (In simple version, used for pattern matching)
}
```

---

## 📊 Context Format

```
=== FILE SYSTEM CONTEXT ===
Current directory: /path/to/current/dir

Contents of [mentioned_dir1]/:
[ls -la output]

Contents of [mentioned_dir2]/:
[ls -la output]

Current directory contents:
[ls -la output]
===========================
```

---

## ⚙️ Customization

### Add More Recognized Directories

Edit `src/gemini_client_simple.cpp` or `src/gemini_client.cpp`:

```cpp
std::regex dir_pattern(R"(\b(test|dir1|dir2|downloads?|documents?|backup|temp|home|desktop|YOUR_DIR)\b)");
```

### Change Context Depth

Modify the listing command:

```cpp
// Show more items:
executeAndCapture("ls -la " + dir + " 2>/dev/null | head -50");

// Show subdirectories too:
executeAndCapture("ls -laR " + dir + " 2>/dev/null | head -100");

// Show only files (no details):
executeAndCapture("ls " + dir + " 2>/dev/null");
```

---

## 🚀 For Gemini API Users

When using the actual Gemini API (`gemini_client.cpp`), this context is **automatically included in the prompt**:

```cpp
std::string GeminiClient::buildPrompt(const std::string& user_input,
                                      const std::string& fs_context) {
    return R"(You are a command-line assistant...

)" + fs_context + R"(

User request: )" + user_input + R"(

Based on the file system context above, generate the appropriate shell command:)";
}
```

This gives Gemini AI **full awareness** of your file system state!

---

## ⚠️ Privacy & Security

### What's Collected:

- Directory listings of mentioned directories only
- Current working directory path
- File names, sizes, permissions (standard `ls -la` output)

### What's NOT Collected:

- File contents
- Files in non-mentioned directories
- System-wide information

### Data Usage:

- Displayed locally in your terminal
- Sent to Gemini API only if using full version
- Never stored persistently

---

## 🧪 Testing

```bash
# Test with test directory
mkdir -p test test/dir1
echo "test file" > test/file1.txt
./ganpi.exe "show me what's in test"

# Test with nested directories
./ganpi.exe "move files in test to dir1 in test"

# Test with current directory
./ganpi.exe "list files in current directory"

# Test with multiple directories
./ganpi.exe "copy files from downloads to backup"
```

---

## 📈 Improvements Over Previous Version

| Aspect           | Before       | After              |
| ---------------- | ------------ | ------------------ |
| File awareness   | ❌ Blind     | ✅ Full context    |
| Command accuracy | ⚠️ Generic   | ✅ Specific        |
| Error prevention | ❌ None      | ✅ Preview context |
| User confidence  | ⚠️ Low       | ✅ High            |
| Debugging        | ❌ Difficult | ✅ Easy            |

---

## 🎉 Summary

GANPI now:

- **Sees what you see** in your file system
- **Understands the context** of your request
- **Generates smarter commands** based on reality
- **Shows you what it knows** before suggesting

This makes command generation much more **accurate**, **reliable**, and **transparent**!

---

**Status:** ✅ Implemented
**Build:** Successful  
**Files Modified:**

- `src/gemini_client_simple.cpp`
- `src/gemini_client.cpp`
- `include/ganpi.h`
