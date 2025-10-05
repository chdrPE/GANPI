#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace ganpi {

// Configuration class for API keys and settings
class Config {
public:
    static Config& getInstance();
    
    void setGeminiApiKey(const std::string& key);
    std::string getGeminiApiKey() const;
    
    void setModel(const std::string& model);
    std::string getModel() const;
    
    bool loadFromFile(const std::string& filename = ".ganpi.config");
    void saveToFile(const std::string& filename = ".ganpi.config");
    
private:
    Config() = default;
    std::string gemini_api_key_;
    std::string model_ = "gemini-pro";
    static std::unique_ptr<Config> instance_;
};

// Gemini API client for natural language processing
class GeminiClient {
public:
    GeminiClient(const std::string& api_key);
    
    // Send natural language query to Gemini and get shell command
    std::string interpretCommand(const std::string& natural_language, bool verbose_output = false, bool show_context = false);
    
    // Summarize content using Gemini
    std::string summarizeContent(const std::string& content);
    
    // Check if API key is valid
    bool validateApiKey(bool verbose_output = false);
    
private:
    std::string api_key_;
    std::string model_;
    
    std::string makeHttpRequest(const std::string& url, const std::string& data);
    std::string buildPrompt(const std::string& user_input);
    std::string getCurrentDirectoryStructure();
    std::string escapeJsonString(const std::string& input);
    std::string parseGeminiResponse(const std::string& response);
    std::string parseSummaryResponse(const std::string& response);
    std::string decodeUnicodeEscapes(const std::string& text);
};

// Command executor for running shell commands
class CommandExecutor {
public:
    struct ExecutionResult {
        bool success;
        std::string output;
        std::string error;
        int exit_code;
    };
    
    // Execute a shell command and return results
    ExecutionResult execute(const std::string& command);
    
    // Execute command with confirmation prompt
    ExecutionResult executeWithConfirmation(const std::string& command);
    
private:
    std::string sanitizeCommand(const std::string& command);
    bool isDangerousCommand(const std::string& command);
};

// Main GANPI application
class GANPI {
public:
    GANPI();
    ~GANPI() = default;
    
    // Initialize the application
    bool initialize(bool verbose_output = false, bool show_context = false);
    
    // Process a natural language command
    void processCommand(const std::string& natural_language);
    
    // Run in interactive mode
    void runInteractive();
    
    // Show help information
    void showHelp();
    
    // Summarize file contents
    void summarizeFile(const std::string& filename);
    
    // Set verbose flags
    void setVerboseOutput(bool verbose) { verbose_output_ = verbose; }
    void setShowContext(bool context) { show_context_ = context; }
    
private:
    std::unique_ptr<GeminiClient> gemini_client_;
    std::unique_ptr<CommandExecutor> executor_;
    Config& config_;
    bool verbose_output_;
    bool show_context_;
    
    void printWelcomeMessage();
    void printCommandPreview(const std::string& command);
};

} // namespace ganpi
