#include "ganpi.h"
#include <iostream>
#include <sstream>
#include <filesystem>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")
#else
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#endif

namespace ganpi {

// Callback function for libcurl to write response data
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
        return newLength;
    }
    catch (std::bad_alloc& e) {
        return 0;
    }
}

GeminiClient::GeminiClient(const std::string& api_key) 
    : api_key_(api_key), model_("gemini-pro") {
}

std::string GeminiClient::interpretCommand(const std::string& natural_language) {
    std::string prompt = buildPrompt(natural_language);
    std::cout << "prompt sent to api: " << prompt << std::endl;
    
    nlohmann::json request_json;
    request_json["contents"] = nlohmann::json::array();
    request_json["contents"][0] = nlohmann::json::object();
    request_json["contents"][0]["parts"] = nlohmann::json::array();
    request_json["contents"][0]["parts"][0] = nlohmann::json::object();
    request_json["contents"][0]["parts"][0]["text"] = prompt;
    
    request_json["generationConfig"] = nlohmann::json::object();
    request_json["generationConfig"]["temperature"] = 0.1;
    request_json["generationConfig"]["maxOutputTokens"] = 1000;
    
    std::string url = "https://generativelanguage.googleapis.com/v1beta/models/" + 
                      model_ + ":generateContent?key=" + api_key_;
    
    std::string response = makeHttpRequest(url, request_json.dump());
    
    try {
        nlohmann::json response_json = nlohmann::json::parse(response);
        
        if (response_json.contains("candidates") && 
            response_json["candidates"].size() > 0 &&
            response_json["candidates"][0].contains("content") &&
            response_json["candidates"][0]["content"].contains("parts") &&
            response_json["candidates"][0]["content"]["parts"].size() > 0) {
            
            std::string generated_text = response_json["candidates"][0]["content"]["parts"][0]["text"];
            
            // Extract shell command from the response
            // Look for commands between ```bash and ``` or just the command itself
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
    } catch (const std::exception& e) {
        std::cerr << "Error parsing Gemini response: " << e.what() << std::endl;
    }
    
    return "";
}

bool GeminiClient::validateApiKey() {
    std::string url = "https://generativelanguage.googleapis.com/v1beta/models?key=" + api_key_;
    std::string response = makeHttpRequest(url, "");
    
    try {
        nlohmann::json response_json = nlohmann::json::parse(response);
        return response_json.contains("models");
    } catch (const std::exception& e) {
        return false;
    }
}

std::string GeminiClient::makeHttpRequest(const std::string& url, const std::string& data) {
#ifdef _WIN32
    // Windows implementation using WinINet
    HINTERNET hInternet = InternetOpenA("GANPI/1.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return "";
    
    HINTERNET hConnect = InternetOpenUrlA(hInternet, url.c_str(), 
                                          data.empty() ? NULL : data.c_str(), 
                                          data.empty() ? 0 : static_cast<DWORD>(data.length()),
                                          INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
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
#else
    // Linux/macOS implementation using libcurl
    CURL* curl;
    CURLcode res;
    std::string response_data;
    
    curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        
        if (!data.empty()) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        }
        
        res = curl_easy_perform(curl);
        
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            return "";
        }
    }
    
    return response_data;
#endif
}

std::string GeminiClient::getCurrentDirectoryStructure() {
    std::string structure;
    try {
        std::string current_path = std::filesystem::current_path().string();
        structure = "Current directory: " + current_path + "\nDirectory structure:\n";
        
        // Get directory contents
        for (const auto& entry : std::filesystem::directory_iterator(current_path)) {
            std::string name = entry.path().filename().string();
            if (entry.is_directory()) {
                structure += "  [DIR]  " + name + "/\n";
            } else {
                structure += "  [FILE] " + name + "\n";
            }
        }
    } catch (const std::exception& e) {
        structure = "Could not read directory structure: " + std::string(e.what());
    }
    return structure;
}

std::string GeminiClient::buildPrompt(const std::string& user_input) {
    std::string directory_info = getCurrentDirectoryStructure();
    
    return "Context:\n" + directory_info + "\n" +
           "You are a command-line assistant that converts natural language requests into precise Windows shell commands.\n\n" +
           "IMPORTANT RULES:\n" +
           "1. ONLY respond with the Windows shell command needed to fulfill the request\n" +
           "2. Do NOT include explanations or additional text\n" +
           "3. Use Windows commands (dir, copy, move, del, etc.)\n" +
           "4. For file operations, use relative paths when possible\n" +
           "5. For dangerous operations, add confirmation prompts\n" +
           "6. Format your response as: ```cmd\n<command>\n```\n\n" +
           "User request: " + user_input + "\n\n" +
           "Windows shell command:";
}

} // namespace ganpi
