# 🧠 GANPI - Gemini-Assisted Natural Processing Interface

**"Talk to your terminal like a human!"**

GANPI is a revolutionary CLI tool that converts natural language commands into precise shell commands using Google's Gemini AI. No more memorizing complex shell syntax - just tell GANPI what you want to do in plain English!

## ✨ Features

- 🧠 **Natural Language Processing**: Convert English commands to shell commands
- 🔒 **Safety First**: Built-in safety checks and confirmation prompts
- 🚀 **Fast Execution**: C++ performance with AI intelligence
- 🔄 **Interactive Mode**: Chat with your terminal in real-time
- 📝 **Command Preview**: See what will be executed before running
- ⚙️ **Easy Setup**: Simple API key configuration

## 🚀 Quick Start

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.16+
- libcurl development libraries
- nlohmann/json library
- A Google Gemini API key ([Get one here](https://makersuite.google.com/app/apikey))

### Installation

1. **Clone the repository**
   ```bash
   git clone https://github.com/your-username/ganpi.git
   cd ganpi
   ```

2. **Install dependencies**
   
   **Ubuntu/Debian:**
   ```bash
   sudo apt update
   sudo apt install build-essential cmake libcurl4-openssl-dev nlohmann-json3-dev
   ```
   
   **macOS:**
   ```bash
   brew install cmake curl nlohmann-json
   ```
   
   **Windows (with vcpkg):**
   ```bash
   vcpkg install curl nlohmann-json
   ```

3. **Build the project**
   ```bash
   chmod +x build.sh
   ./build.sh
   ```

4. **Run GANPI**
   ```bash
   ./build/ganpi --help
   ```

## 🎯 Usage Examples

### Basic Usage
```bash
# Single command
ganpi "Find all PDF files in Downloads and zip them into documents.zip"

# Interactive mode
ganpi --interactive

# Help
ganpi --help
```

### 🗂️ File Management Examples
```bash
# Batch organization
ganpi "Move all .txt files from Downloads to Documents/notes"
ganpi "Delete all .DS_Store files in this folder"
ganpi "Rename all .png files to have the prefix 'holiday_'"

# Archiving
ganpi "Zip all PDFs in Downloads folder into 'school_papers.zip'"
ganpi "Extract all .zip files in Downloads to a new folder called 'extracted'"
```

### 🧠 Developer Productivity
```bash
# Project setup
ganpi "Initialize a new git repository here and make the first commit"
ganpi "Start a simple HTTP server on port 8080"
ganpi "Compile all .cpp files in src and run the output binary"

# Container helpers
ganpi "List all Docker containers that are running"
ganpi "Stop all running Docker containers"
```

### 🖼️ Media Operations
```bash
# Image processing
ganpi "Resize all images in Pictures to 1080p"
ganpi "Convert all PNGs in this folder to JPEG"
ganpi "Create thumbnails for all images in the current directory"

# Video tasks
ganpi "Extract audio from video.mp4 and save it as podcast.mp3"
ganpi "Convert all .mov files to .mp4 format"
```

### 🔍 Search & Information
```bash
# Text search
ganpi "Find all files containing the word 'token' in the src directory"
ganpi "Search for TODO comments in all Python files"

# System information
ganpi "Show me the top 10 processes using the most RAM"
ganpi "How much space am I using in my home directory?"
ganpi "List all files I modified today"
```

## ⚙️ Configuration

On first run, GANPI will prompt you to enter your Gemini API key. This will be saved in a `.ganpi_config` file in your home directory.

### Manual Configuration
You can also create a `.ganpi_config` file manually:
```
GEMINI_API_KEY=your_api_key_here
MODEL=gemini-pro
```

## 🏗️ Building from Source

### Manual Build
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Cross-platform Building
```bash
# Windows (with Visual Studio)
cmake .. -G "Visual Studio 16 2019" -A x64

# macOS (with Xcode)
cmake .. -G Xcode

# Linux (with Ninja)
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
```

## 🛡️ Safety Features

GANPI includes several safety mechanisms:

- **Command Sanitization**: Filters out dangerous commands
- **Confirmation Prompts**: Asks before executing potentially risky operations
- **Dangerous Command Detection**: Identifies commands that could harm your system
- **Safe Defaults**: Conservative approach to command execution

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guidelines](CONTRIBUTING.md) for details.

### Development Setup
```bash
# Install development dependencies
sudo apt install clang-format cppcheck

# Run tests
./scripts/run_tests.sh

# Format code
./scripts/format_code.sh
```

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🎉 Hackathon Demo Tips

For hackathon presentations, try these impressive examples:

1. **File Organization Magic**
   ```bash
   ganpi "Organize my Downloads folder by file type into separate folders"
   ```

2. **Developer Workflow**
   ```bash
   ganpi "Set up a new Node.js project with Express and install all dependencies"
   ```

3. **Media Processing**
   ```bash
   ganpi "Create a slideshow video from all images in this folder"
   ```

## 🐛 Troubleshooting

### Common Issues

**Build fails with "nlohmann/json not found"**
```bash
# Install nlohmann-json
sudo apt install nlohmann-json3-dev  # Ubuntu/Debian
brew install nlohmann-json           # macOS
```

**API key validation fails**
- Check your Gemini API key is correct
- Ensure you have internet connectivity
- Verify your API key has the necessary permissions

**Commands not executing**
- Check if the command requires sudo privileges
- Verify the command syntax is correct for your system
- Some commands may need additional packages installed

## 📞 Support

- 🐛 [Report Issues](https://github.com/your-username/ganpi/issues)
- 💬 [Discussions](https://github.com/your-username/ganpi/discussions)
- 📧 Email: support@ganpi.dev

---

**Made with ❤️ for the hackathon community**
