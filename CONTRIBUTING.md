# Contributing to GANPI

Thank you for your interest in contributing to GANPI! 🧠

## 🚀 Getting Started

1. **Fork the repository** on GitHub
2. **Clone your fork** locally:
   ```bash
   git clone https://github.com/your-username/ganpi.git
   cd ganpi
   ```
3. **Install dependencies** (see README.md)
4. **Build the project**:
   ```bash
   ./build.sh        # Linux/macOS
   build.bat         # Windows
   ```

## 🔧 Development Setup

### Prerequisites
- C++17 compatible compiler
- CMake 3.16+
- libcurl development libraries
- nlohmann/json library
- Git

### Building for Development
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

## 📝 Code Style

We follow these coding standards:

- **C++17** standard
- **4 spaces** for indentation (no tabs)
- **snake_case** for variables and functions
- **PascalCase** for classes
- **UPPER_CASE** for constants
- **Meaningful variable names**
- **Comments** for complex logic

### Example:
```cpp
class ExampleClass {
public:
    void doSomething(const std::string& input_string) {
        // Process the input string
        std::string processed_string = input_string;
        // ... implementation
    }
    
private:
    static const int MAX_RETRIES = 3;
    std::string member_variable_;
};
```

## 🧪 Testing

Before submitting a pull request:

1. **Run the test suite**:
   ```bash
   ./scripts/run_tests.sh
   ```

2. **Test your changes** with various commands:
   ```bash
   ./build/ganpi "test your new feature"
   ```

3. **Check for memory leaks** (if applicable):
   ```bash
   valgrind --leak-check=full ./build/ganpi --help
   ```

## 📋 Pull Request Process

1. **Create a feature branch**:
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **Make your changes** with clear, descriptive commits:
   ```bash
   git commit -m "Add support for batch file operations"
   ```

3. **Push to your fork**:
   ```bash
   git push origin feature/your-feature-name
   ```

4. **Create a Pull Request** with:
   - Clear description of changes
   - Reference to any related issues
   - Screenshots/videos for UI changes
   - Test results

## 🐛 Bug Reports

When reporting bugs, please include:

- **Operating System** and version
- **GANPI version** (from `ganpi --version`)
- **Steps to reproduce** the issue
- **Expected behavior** vs **actual behavior**
- **Error messages** or logs
- **Screenshots** if applicable

## 💡 Feature Requests

We welcome feature ideas! Please:

- **Check existing issues** first
- **Describe the use case** clearly
- **Provide examples** of how it would work
- **Consider implementation complexity**

## 🏗️ Project Structure

```
ganpi/
├── include/           # Header files
│   └── ganpi.h
├── src/              # Source files
│   ├── main.cpp
│   ├── ganpi.cpp
│   ├── gemini_client.cpp
│   ├── command_executor.cpp
│   └── config.cpp
├── examples/         # Demo commands
├── scripts/          # Build and utility scripts
├── CMakeLists.txt    # Build configuration
└── README.md         # Documentation
```

## 🎯 Areas for Contribution

### High Priority
- **Additional AI providers** (OpenAI, Claude, etc.)
- **Command history** and favorites
- **Plugin system** for custom commands
- **Voice input** support
- **Cross-platform improvements**

### Medium Priority
- **Performance optimizations**
- **Better error handling**
- **Configuration GUI**
- **Command templates**
- **Batch operations**

### Low Priority
- **Themes and customization**
- **Statistics and analytics**
- **Command suggestions**
- **Integration with IDEs**

## 🤝 Community Guidelines

- **Be respectful** and inclusive
- **Help others** learn and grow
- **Provide constructive feedback**
- **Follow the code of conduct**

## 📞 Getting Help

- 💬 **Discussions**: GitHub Discussions
- 🐛 **Issues**: GitHub Issues
- 📧 **Email**: maintainers@ganpi.dev
- 💬 **Discord**: [Join our community](https://discord.gg/ganpi)

## 🏆 Recognition

Contributors will be:
- Listed in CONTRIBUTORS.md
- Mentioned in release notes
- Invited to join the core team (for significant contributions)

Thank you for making GANPI better! 🚀
