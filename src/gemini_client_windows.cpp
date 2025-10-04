#include "ganpi.h"
#include <windows.h>
#include <wininet.h>
#include <iostream>
#include <sstream>

#pragma comment(lib, "wininet.lib")

namespace ganpi {

GeminiClient::GeminiClient(const std::string& api_key) 
    : api_key_(api_key), model_("gemini-pro") {
}

std::string GeminiClient::interpretCommand(const std::string& natural_language) {
    std::string prompt = buildPrompt(natural_language);
    
    // Simple JSON request
    std::string json_data = "{\"contents\":[{\"parts\":[{\"text\":\"" + prompt + "\"}]}],\"generationConfig\":{\"temperature\":0.1,\"maxOutputTokens\":1000}}";
    
    std::string url = "https://generativelanguage.googleapis.com/v1beta/models/" + 
                      model_ + ":generateContent?key=" + api_key_;
    
    std::string response = makeHttpRequest(url, json_data);
    
    // Simple response parsing - look for the command
    size_t text_start = response.find("\"text\":\"");
    if (text_start != std::string::npos) {
        text_start += 8; // Length of "\"text\":\""
        size_t text_end = response.find("\"", text_start);
        if (text_end != std::string::npos) {
            std::string generated_text = response.substr(text_start, text_end - text_start);
            
            // Extract shell command from the response
            size_t bash_start = generated_text.find("```bash");
            if (bash_start != std::string::npos) {
                bash_start += 7; // Length of "```bash"
                size_t bash_end = generated_text.find("```", bash_start);
                if (bash_end != std::string::npos) {
                    std::string command = generated_text.substr(bash_start, bash_end - bash_start);
                    // Remove leading/trailing whitespace
                    command.erase(0, command.find_first_not_of(" \t\n\r"));
                    command.erase(command.find_last_not_of(" \t\n\r") + 1);
                    return command;
                }
            }
            
            // If no code blocks, try to extract first line that looks like a command
            std::istringstream iss(generated_text);
            std::string line;
            while (std::getline(iss, line)) {
                line.erase(0, line.find_first_not_of(" \t"));
                if (!line.empty() && (line[0] == '$' || line.find_first_of("abcdefghijklmnopqrstuvwxyz") == 0)) {
                    if (line[0] == '$') {
                        line = line.substr(1);
                        line.erase(0, line.find_first_not_of(" \t"));
                    }
                    return line;
                }
            }
            
            return generated_text;
        }
    }
    
    return "";
}

bool GeminiClient::validateApiKey() {
    std::string url = "https://generativelanguage.googleapis.com/v1beta/models?key=" + api_key_;
    std::string response = makeHttpRequest(url, "");
    return response.find("\"models\"") != std::string::npos;
}

std::string GeminiClient::makeHttpRequest(const std::string& url, const std::string& data) {
    HINTERNET hInternet = InternetOpenA("GANPI/1.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return "";
    
    HINTERNET hConnect = InternetOpenUrlA(hInternet, url.c_str(), data.empty() ? NULL : data.c_str(), 
                                          data.empty() ? 0 : static_cast<DWORD>(data.length()),
                                          INTERNET_FLAG_RELOAD, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return "";
    }
    
    std::string response;
    char buffer[4096];
    DWORD bytesRead;
    
    while (InternetReadFile(hConnect, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        response += buffer;
    }
    
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    
    return response;
}

std::string GeminiClient::buildPrompt(const std::string& user_input) {
    return R"(You are a command-line assistant that converts natural language requests into precise shell commands.

IMPORTANT RULES:
1. ONLY respond with the shell command needed to fulfill the request
2. Do NOT include explanations or additional text
3. Use safe, standard commands that work on Unix-like systems (Linux/macOS)
4. For file operations, use relative paths when possible
5. For dangerous operations (rm -rf, sudo, etc.), add confirmation prompts
6. Format your response as: ```bash
<command>
```

User request: )" + user_input + R"( 

Shell command:)";
}

} // namespace ganpi
