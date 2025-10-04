#include "ganpi.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace ganpi {

GeminiClient::GeminiClient(const std::string& api_key) 
    : api_key_(api_key), model_("gemini-pro") {
}

std::string GeminiClient::interpretCommand(const std::string& natural_language) {
    // For demo purposes, return a simple command based on keywords
    // In a real implementation, this would call the Gemini API
    
    std::string lower_input = natural_language;
    std::transform(lower_input.begin(), lower_input.end(), lower_input.begin(), ::tolower);
    
    // Simple keyword-based command generation for demo
    if (lower_input.find("list") != std::string::npos && lower_input.find("file") != std::string::npos) {
        return "dir";
    } else if (lower_input.find("find") != std::string::npos && lower_input.find("pdf") != std::string::npos) {
        return "dir *.pdf /s";
    } else if (lower_input.find("zip") != std::string::npos && lower_input.find("pdf") != std::string::npos) {
        return "powershell Compress-Archive -Path *.pdf -DestinationPath documents.zip";
    } else if (lower_input.find("move") != std::string::npos && lower_input.find("txt") != std::string::npos) {
        return "move *.txt Documents\\notes\\";
    } else if (lower_input.find("delete") != std::string::npos && lower_input.find("temp") != std::string::npos) {
        return "del /s /q *.tmp";
    } else if (lower_input.find("server") != std::string::npos && lower_input.find("http") != std::string::npos) {
        return "python -m http.server 8080";
    } else {
        return "echo 'Command not recognized. Please try: list files, find PDFs, zip files, move files, delete temp files, or start server'";
    }
}

bool GeminiClient::validateApiKey() {
    // For demo purposes, always return true
    return !api_key_.empty();
}

std::string GeminiClient::makeHttpRequest(const std::string& url, const std::string& data) {
    // Placeholder - would make actual HTTP request in real implementation
    return "";
}

std::string GeminiClient::buildPrompt(const std::string& user_input) {
    return "Convert to shell command: " + user_input;
}

} // namespace ganpi
