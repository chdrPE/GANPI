#include "ganpi.h"
#include <iostream>
#include <sstream>
#include <cstdio>
#include <algorithm>
#include <regex>
#include <set>

#ifdef _WIN32
#include <windows.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")
#else
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#endif

namespace ganpi {

// Helper function to execute a command and capture output
std::string executeAndCapture(const std::string& cmd) {
    std::string result;
    // On Windows with Git Bash, need to explicitly use bash
    std::string full_cmd = "bash -c \"" + cmd + "\"";
    FILE* pipe = _popen(full_cmd.c_str(), "r");
    if (!pipe) return "";
    
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    _pclose(pipe);
    return result;
}

// Get file system context for directories mentioned in the query
std::string getFileSystemContext(const std::string& query) {
    std::string context = "\n=== FILE SYSTEM CONTEXT ===\n";
    
    // Get current directory
    std::string pwd = executeAndCapture("pwd");
    if (!pwd.empty()) {
        pwd.erase(pwd.find_last_not_of("\n\r") + 1);
        context += "Current directory: " + pwd + "\n";
    }
    
    // ALWAYS show current directory structure first
    context += "\n--- Current Directory Structure ---\n";
    std::string current_structure = executeAndCapture("ls -lAh 2>/dev/null");
    if (!current_structure.empty()) {
        context += current_structure;
    }
    
    // Show directory tree (limited depth for performance)
    context += "\n--- Directory Tree (2 levels) ---\n";
    std::string tree = executeAndCapture("find . -maxdepth 2 -type d 2>/dev/null | head -30");
    if (!tree.empty()) {
        context += tree;
    }
    
    // List all files in current directory (non-hidden)
    context += "\n--- All Files in Current Directory ---\n";
    std::string files_list = executeAndCapture("find . -maxdepth 1 -type f 2>/dev/null");
    if (!files_list.empty()) {
        context += files_list;
    }
    
    // Extract potential directory names from query and show their contents
    std::regex dir_pattern(R"(\b(test|dir1|dir2|downloads?|documents?|backup|temp|home|desktop)\b)");
    std::smatch match;
    std::string lower_query = query;
    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);
    
    std::set<std::string> found_dirs;
    std::string::const_iterator searchStart(lower_query.cbegin());
    while (std::regex_search(searchStart, lower_query.cend(), match, dir_pattern)) {
        found_dirs.insert(match[0].str());
        searchStart = match.suffix().first;
    }
    
    // List files in specifically mentioned directories
    if (!found_dirs.empty()) {
        context += "\n--- Mentioned Directories ---\n";
        for (const auto& dir : found_dirs) {
            std::string ls_output = executeAndCapture("ls -lAh " + dir + " 2>/dev/null | head -30");
            if (!ls_output.empty()) {
                context += "\nContents of " + dir + "/:\n" + ls_output;
            } else {
                context += "\n" + dir + "/ does not exist or is empty\n";
            }
        }
    }
    
    context += "\n===========================\n";
    return context;
}

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
    // Gather file system context
    std::string fs_context = getFileSystemContext(natural_language);
    std::cout << "\n📂 Analyzing file system context..." << std::endl;
    std::cout << fs_context << std::endl;
    
    std::string prompt = buildPrompt(natural_language, fs_context);
    
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
    
    // Print API request data
    std::cout << "\n🌐 Calling Gemini API..." << std::endl;
    std::cout << "📤 Request Data:\n" << request_json.dump(2) << std::endl;
    std::cout << "\n⏳ Waiting for response...\n" << std::endl;
    
    std::string response = makeHttpRequest(url, request_json.dump());
    
    // Print API response
    std::cout << "📥 Response received from Gemini API" << std::endl;
    std::cout << "📄 Raw Response:\n" << response << std::endl;
    std::cout << "\n🔍 Parsing response...\n" << std::endl;
    
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
}

std::string GeminiClient::buildPrompt(const std::string& user_input, const std::string& fs_context) {
    return R"(You are a command-line assistant that converts natural language requests into precise shell commands.

IMPORTANT RULES:
1. ONLY respond with the shell command needed to fulfill the request
2. Do NOT include explanations or additional text
3. Use safe, standard commands that work on Unix-like systems (Linux/macOS/Git Bash)
4. For file operations, use relative paths when possible
5. For dangerous operations (rm -rf, sudo, etc.), add confirmation prompts
6. Format your response as: ```bash
<command>
```

)" + fs_context + R"(

User request: )" + user_input + R"(

Based on the file system context above, generate the appropriate shell command:)";
}

} // namespace ganpi
