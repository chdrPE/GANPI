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
    std::cout << "[INFO] Config loaded: " << (config_loaded ? "YES" : "NO") << std::endl;
    
    std::string api_key = config_.getGeminiApiKey();
    std::cout << "[INFO] API key loaded: " << (api_key.empty() ? "NO" : "YES (length: " + std::to_string(api_key.length()) + ")") << std::endl;
    if (!api_key.empty()) {
        std::cout << "[INFO] API key preview: " << api_key.substr(0, 8) << "..." << std::endl;
    }
    
    if (!config_loaded || api_key.empty()) {
        std::cout << "[WARNING] No API key found. Please set up your Gemini API key:" << std::endl;
        std::cout << "   1. Go to https://makersuite.google.com/app/apikey" << std::endl;
        std::cout << "   2. Create a new API key" << std::endl;
        std::cout << "   3. Enter it below:" << std::endl;
        std::cout << "\n   API Key: ";
        
        std::string api_key;
        std::getline(std::cin, api_key);
        
        if (api_key.empty()) {
            std::cout << "[ERROR] No API key provided. Cannot initialize GANPI." << std::endl;
            return false;
        }
        
        config_.setGeminiApiKey(api_key);
        config_.saveToFile();
        std::cout << "[SUCCESS] API key saved!" << std::endl;
    }
    
    // Initialize Gemini client
    gemini_client_ = std::make_unique<GeminiClient>(config_.getGeminiApiKey());
    
    // Validate API key
    if (!gemini_client_->validateApiKey()) {
        std::cout << "[ERROR] Invalid API key. Please check your Gemini API key." << std::endl;
        return false;
    }
    
    // Initialize command executor
    executor_ = std::make_unique<CommandExecutor>();
    
    return true;
}

void GANPI::processCommand(const std::string& natural_language) {
    if (!gemini_client_ || !executor_) {
        std::cout << "[ERROR] GANPI not properly initialized." << std::endl;
        return;
    }
    
    std::cout << "\n[INFO] Sending to API..." << std::endl;
    
    // Get command from Gemini
    std::string shell_command = gemini_client_->interpretCommand(natural_language);
    
    if (shell_command.empty()) {
        std::cout << "[ERROR] Could not interpret the command. Please try rephrasing." << std::endl;
        return;
    }
    
    // Show what command will be executed
    std::cout << "prompt to execute: " << shell_command << std::endl;
    
    // Execute with confirmation
    auto result = executor_->executeWithConfirmation(shell_command);
    
    if (result.success) {
        std::cout << "\n[INFO] Executing..." << std::endl;
        std::cout << result.output;
        std::cout << "\n[SUCCESS] Done" << std::endl;
    } else {
        std::cout << "\n[ERROR] Command failed: " << result.error << std::endl;
        if (!result.output.empty()) {
            std::cout << result.output << std::endl;
        }
    }
}

void GANPI::runInteractive() {
    if (!gemini_client_ || !executor_) {
        std::cout << "[ERROR] GANPI not properly initialized." << std::endl;
        return;
    }
    
    printWelcomeMessage();
    
    std::string input;
    while (true) {
        std::cout << "\n[INPUT] What would you like me to do? (or 'quit' to exit): ";
        std::getline(std::cin, input);
        
        if (input == "quit" || input == "exit" || input == "q") {
            std::cout << "[GOODBYE] Goodbye!" << std::endl;
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
GANPI - Gemini-Assisted Natural Processing Interface

USAGE:
    ganpi "natural language command"    # Execute a single command
    ganpi summarize "filename.txt"      # Summarize file contents
    ganpi --interactive                 # Start interactive mode
    ganpi --help                        # Show this help

EXAMPLES:
    ganpi "Find all PDF files in Downloads and zip them"
    ganpi "Move all .txt files from Downloads to Documents/notes"
    ganpi "Delete all .DS_Store files in this folder"
    ganpi "Resize all images in Pictures to 1080p"
    ganpi "Start a simple HTTP server on port 8080"
    ganpi "Show me the top 10 processes using the most RAM"
    ganpi summarize "document.txt"
    ganpi summarize "report.pdf"

FEATURES:
    * Natural language to shell command conversion
    * File content summarization using AI
    * Safety checks for dangerous operations
    * Confirmation prompts for potentially risky commands
    * Command preview before execution
    * Interactive mode for multiple commands

SETUP:
    1. Get a Gemini API key from https://makersuite.google.com/app/apikey
    2. Run ganpi once to configure your API key
    3. Start using natural language commands!

For more examples and documentation, visit: https://github.com/your-repo/ganpi
)" << std::endl;
}

void GANPI::printWelcomeMessage() {
    std::cout << R"(
    ----------------------------------------------------------------
    |                                                              |
    |    Welcome to GANPI Interactive Mode!                        |
    |                                                              |
    |    Just tell me what you want to do in plain English         |
    |    and I'll figure out the shell commands for you!           |
    |                                                              |
    |    Type 'quit' to exit                                       |    
    |                                                              |
    ----------------------------------------------------------------
    )" << std::endl;
}

void GANPI::printCommandPreview(const std::string& command) {
    std::cout << "\n[SUGGEST] GANPI suggests this command:" << std::endl;
    std::cout << "   $ " << command << std::endl;
}

void GANPI::summarizeFile(const std::string& filename) {
    if (!gemini_client_) {
        std::cout << "[ERROR] GANPI not properly initialized." << std::endl;
        return;
    }
    
    // Read file contents
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Could not open file '" << filename << "'" << std::endl;
        return;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    if (content.empty()) {
        std::cout << "[INFO] File '" << filename << "' is empty." << std::endl;
        return;
    }
    
    std::cout << "[INFO] Reading file: " << filename << std::endl;
    std::cout << "[INFO] File size: " << content.length() << " characters" << std::endl;
    
    // Create a summarization prompt
    std::string prompt = "Please provide a clear and concise summary of the following file content. "
                        "Include the main topics, key points, and purpose of the document:\n\n" +
                        content;
    
    std::cout << "\n[INFO] Generating summary..." << std::endl;
    
    // Use Gemini to summarize the content directly
    std::string summary = gemini_client_->summarizeContent(prompt);
    
    if (summary.empty()) {
        std::cout << "[ERROR] Could not generate summary. Please try again." << std::endl;
        return;
    }
    
    std::cout << "\n[SUMMARY] Summary of " << filename << ":" << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;
    std::cout << summary << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;
}

} // namespace ganpi
