#include "ganpi.h"
#include <iostream>
#include <string>
#include <fstream>

namespace ganpi {

GANPI::GANPI() : config_(Config::getInstance()) {
}

bool GANPI::initialize() {
    // Try to load configuration
    bool config_loaded = config_.loadFromFile();
    std::cout << "📁 Config loaded: " << (config_loaded ? "YES" : "NO") << std::endl;
    
    std::string api_key = config_.getGeminiApiKey();
    std::cout << "🔑 API key loaded: " << (api_key.empty() ? "NO" : "YES (length: " + std::to_string(api_key.length()) + ")") << std::endl;
    if (!api_key.empty()) {
        std::cout << "🔑 API key preview: " << api_key.substr(0, 8) << "..." << std::endl;
    }
    
    if (!config_loaded || api_key.empty()) {
        std::cout << "⚠️  No API key found. Please set up your Gemini API key:" << std::endl;
        std::cout << "   1. Go to https://makersuite.google.com/app/apikey" << std::endl;
        std::cout << "   2. Create a new API key" << std::endl;
        std::cout << "   3. Enter it below:" << std::endl;
        std::cout << "\n   API Key: ";
        
        std::string api_key;
        std::getline(std::cin, api_key);
        
        if (api_key.empty()) {
            std::cout << "❌ No API key provided. Cannot initialize GANPI." << std::endl;
            return false;
        }
        
        config_.setGeminiApiKey(api_key);
        config_.saveToFile();
        std::cout << "✅ API key saved!" << std::endl;
    }
    
    // Initialize Gemini client
    gemini_client_ = std::make_unique<GeminiClient>(config_.getGeminiApiKey());
    
    // Validate API key
    if (!gemini_client_->validateApiKey()) {
        std::cout << "❌ Invalid API key. Please check your Gemini API key." << std::endl;
        return false;
    }
    
    // Initialize command executor
    executor_ = std::make_unique<CommandExecutor>();
    
    return true;
}

void GANPI::processCommand(const std::string& natural_language) {
    if (!gemini_client_ || !executor_) {
        std::cout << "❌ GANPI not properly initialized." << std::endl;
        return;
    }
    
    std::cout << "\nsending to api...." << std::endl;
    
    // Get command from Gemini
    std::string shell_command = gemini_client_->interpretCommand(natural_language);
    
    if (shell_command.empty()) {
        std::cout << "❌ Could not interpret the command. Please try rephrasing." << std::endl;
        return;
    }
    
    // Show what command will be executed
    std::cout << "prompt to execute: " << shell_command << std::endl;
    
    // Execute with confirmation
    auto result = executor_->executeWithConfirmation(shell_command);
    
    if (result.success) {
        std::cout << "\nexecuting..." << std::endl;
        std::cout << result.output;
        std::cout << "\ndone" << std::endl;
    } else {
        std::cout << "\n❌ Command failed: " << result.error << std::endl;
        if (!result.output.empty()) {
            std::cout << result.output << std::endl;
        }
    }
}

void GANPI::runInteractive() {
    if (!gemini_client_ || !executor_) {
        std::cout << "❌ GANPI not properly initialized." << std::endl;
        return;
    }
    
    printWelcomeMessage();
    
    std::string input;
    while (true) {
        std::cout << "\n💬 What would you like me to do? (or 'quit' to exit): ";
        std::getline(std::cin, input);
        
        if (input == "quit" || input == "exit" || input == "q") {
            std::cout << "👋 Goodbye!" << std::endl;
            break;
        }
        
        if (input.empty()) {
            continue;
        }
        
        processCommand(input);
    }
}

void GANPI::showHelp() {
    std::cout << R"(
🧠 GANPI - Gemini-Assisted Natural Processing Interface

USAGE:
    ganpi "natural language command"    # Execute a single command
    ganpi --interactive                 # Start interactive mode
    ganpi --help                        # Show this help

EXAMPLES:
    ganpi "Find all PDF files in Downloads and zip them"
    ganpi "Move all .txt files from Downloads to Documents/notes"
    ganpi "Delete all .DS_Store files in this folder"
    ganpi "Resize all images in Pictures to 1080p"
    ganpi "Start a simple HTTP server on port 8080"
    ganpi "Show me the top 10 processes using the most RAM"

FEATURES:
    🧠 Natural language to shell command conversion
    🔒 Safety checks for dangerous operations
    ✅ Confirmation prompts for potentially risky commands
    📝 Command preview before execution
    🔄 Interactive mode for multiple commands

SETUP:
    1. Get a Gemini API key from https://makersuite.google.com/app/apikey
    2. Run ganpi once to configure your API key
    3. Start using natural language commands!

For more examples and documentation, visit: https://github.com/your-repo/ganpi
)" << std::endl;
}

void GANPI::printWelcomeMessage() {
    std::cout << R"(
    ╔══════════════════════════════════════════════════════════════╗
    ║                                                              ║
    ║    🧠 Welcome to GANPI Interactive Mode!                     ║
    ║                                                              ║
    ║    Just tell me what you want to do in plain English        ║
    ║    and I'll figure out the shell commands for you!          ║
    ║                                                              ║
    ║    Type 'quit' to exit                                       ║
    ║                                                              ║
    ╚══════════════════════════════════════════════════════════════╝
)" << std::endl;
}

void GANPI::printCommandPreview(const std::string& command) {
    std::cout << "\n🔧 GANPI suggests this command:" << std::endl;
    std::cout << "   $ " << command << std::endl;
}

} // namespace ganpi
