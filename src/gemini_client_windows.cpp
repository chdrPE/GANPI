#include "ganpi.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")
#else
#include <curl/curl.h>
#endif

namespace ganpi {

// Callback function for libcurl to write response data (Linux/macOS only)
#ifndef _WIN32
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
#endif

GeminiClient::GeminiClient(const std::string& api_key) 
    : api_key_(api_key), model_("gemini-2.5-flash") {
}

std::string GeminiClient::interpretCommand(const std::string& natural_language, bool verbose_output, bool show_context) {
    std::string prompt = buildPrompt(natural_language);
    
    if (show_context) {
        std::cout << "prompt sent to api: " << prompt << std::endl;
    }
    
    // Build JSON request manually for Windows
    std::string json_request = "{"
        "\"contents\":[{"
            "\"parts\":[{"
                "\"text\":\"" + escapeJsonString(prompt) + "\""
            "}]"
        "}],"
        "\"generationConfig\":{"
            "\"temperature\":0.1,"
            "\"maxOutputTokens\":3000"
        "}"
    "}";
    
    std::string url = "https://generativelanguage.googleapis.com/v1beta/models/" + 
                      model_ + ":generateContent?key=" + api_key_;
    
    std::string response = makeHttpRequest(url, json_request);
    
    if (verbose_output) {
        std::cout << "[INFO] API Response length: " << response.length() << " bytes" << std::endl;
        std::cout << "[DEBUG] API Response preview: " << response.substr(0, 300) << std::endl;
    }
    
    // Parse response manually
    std::string command = parseGeminiResponse(response);
    
    return command;
}

std::string GeminiClient::summarizeContent(const std::string& content) {
    // Build JSON request for summarization with higher token limits
    std::string json_request = "{"
        "\"contents\":[{"
            "\"parts\":[{"
                "\"text\":\"" + escapeJsonString(content) + "\""
            "}]"
        "}],"
        "\"generationConfig\":{"
            "\"temperature\":0.3,"
            "\"maxOutputTokens\":4000"
        "}"
    "}";
    
    std::string url = "https://generativelanguage.googleapis.com/v1beta/models/" + 
                      model_ + ":generateContent?key=" + api_key_;
    
    std::string response = makeHttpRequest(url, json_request);
    
    std::cout << "[INFO] API Response length: " << response.length() << " bytes" << std::endl;
    
    // Parse response for summarization (return raw text, not commands)
    std::string summary = parseSummaryResponse(response);
    
    return summary;
}

bool GeminiClient::validateApiKey(bool verbose_output) {
    std::string url = "https://generativelanguage.googleapis.com/v1beta/models?key=" + api_key_;
    if (verbose_output) {
        std::cout << "[INFO] Validating API key with URL: " << url.substr(0, 50) << "..." << std::endl;
    }
    
    std::string response = makeHttpRequest(url, "");
    if (verbose_output) {
        std::cout << "[INFO] Response received: " << (response.empty() ? "EMPTY" : std::to_string(response.length()) + " bytes") << std::endl;
    }
    
    if (response.empty()) {
        std::cout << "[ERROR] No response from API - check internet connection" << std::endl;
        return false;
    }
    
    bool is_valid = response.find("\"models\"") != std::string::npos;
    if (verbose_output) {
        std::cout << "[RESULT] API key validation: " << (is_valid ? "VALID" : "INVALID") << std::endl;
    }
    
    if (!is_valid && verbose_output) {
        std::cout << "[DEBUG] Response preview: " << response.substr(0, 200) << std::endl;
    }
    
    return is_valid;
}

std::string GeminiClient::makeHttpRequest(const std::string& url, const std::string& data) {
#ifdef _WIN32
    // Windows implementation using WinINet
    HINTERNET hInternet = InternetOpenA("GANPI/1.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return "";
    
    std::string response;
    
    if (data.empty()) {
        // GET request
        HINTERNET hConnect = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0,
                                              INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
        if (!hConnect) {
            InternetCloseHandle(hInternet);
            return "";
        }
        
        char buffer[4096];
        DWORD bytesRead;
        
        while (InternetReadFile(hConnect, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            response += buffer;
        }
        
        InternetCloseHandle(hConnect);
    } else {
        // POST request
        // Parse URL to get host and path
        size_t protocol_end = url.find("://");
        if (protocol_end == std::string::npos) {
            InternetCloseHandle(hInternet);
            return "";
        }
        
        size_t host_start = protocol_end + 3;
        size_t path_start = url.find("/", host_start);
        
        std::string host = url.substr(host_start, path_start - host_start);
        std::string path = path_start != std::string::npos ? url.substr(path_start) : "/";
        
        HINTERNET hConnect = InternetConnectA(hInternet, host.c_str(), INTERNET_DEFAULT_HTTPS_PORT,
                                              NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
        if (!hConnect) {
            InternetCloseHandle(hInternet);
            return "";
        }
        
        const char* accept_types[] = {"application/json", NULL};
        HINTERNET hRequest = HttpOpenRequestA(hConnect, "POST", path.c_str(), "HTTP/1.1",
                                              NULL, accept_types, 
                                              INTERNET_FLAG_SECURE | INTERNET_FLAG_RELOAD, 0);
        if (!hRequest) {
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hInternet);
            return "";
        }
        
        // Set headers
        std::string headers = "Content-Type: application/json\r\n";
        HttpAddRequestHeadersA(hRequest, headers.c_str(), static_cast<DWORD>(headers.length()), HTTP_ADDREQ_FLAG_ADD);
        
        // Send request
        BOOL result = HttpSendRequestA(hRequest, NULL, 0, const_cast<char*>(data.c_str()), static_cast<DWORD>(data.length()));
        if (!result) {
            InternetCloseHandle(hRequest);
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hInternet);
            return "";
        }
        
        // Read response
        char buffer[4096];
        DWORD bytesRead;
        
        while (InternetReadFile(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            response += buffer;
        }
        
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
    }
    
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
           "5. For folder deletion, use 'rmdir /s /q' to avoid confirmation prompts\n" +
           "6. For file deletion, use 'del /q' to avoid confirmation prompts\n" +
           "7. Format your response as: ```cmd\n<command>\n```\n\n" +
           "User request: " + user_input + "\n\n" +
           "Windows shell command:";
}

std::string GeminiClient::escapeJsonString(const std::string& input) {
    std::string result;
    for (char c : input) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string GeminiClient::parseGeminiResponse(const std::string& response) {
    // Parse the new Gemini API response format
    // Look for candidates[0].content.parts[0].text
    
    size_t candidates_start = response.find("\"candidates\":");
    if (candidates_start == std::string::npos) {
        return "echo 'No candidates found in API response'";
    }
    
    // Find the first candidate
    size_t content_start = response.find("\"content\":", candidates_start);
    if (content_start == std::string::npos) {
        return "echo 'No content found in API response'";
    }
    
    // Find parts array
    size_t parts_start = response.find("\"parts\":", content_start);
    if (parts_start == std::string::npos) {
        return "echo 'No parts found in API response'";
    }
    
    // Find the first part with text - look for "text":" (with possible whitespace)
    size_t text_start = response.find("\"text\"", parts_start);
    if (text_start == std::string::npos) {
        return "echo 'No text found in API response'";
    }
    
    // Find the colon and opening quote after "text"
    size_t colon_pos = response.find(":", text_start);
    if (colon_pos == std::string::npos) {
        return "echo 'No colon found after text'";
    }
    
    // Find the opening quote after the colon (skip any whitespace)
    size_t quote_start = colon_pos + 1;
    while (quote_start < response.length() && (response[quote_start] == ' ' || response[quote_start] == '\t')) {
        quote_start++;
    }
    
    if (quote_start >= response.length() || response[quote_start] != '"') {
        return "echo 'No opening quote found after text:'";
    }
    
    text_start = quote_start + 1; // Position after the opening quote
    
    // Find the end of the text field - this is tricky because text can contain quotes
    // We need to find the closing quote that's not escaped
    size_t text_end = text_start;
    bool escaped = false;
    while (text_end < response.length()) {
        if (response[text_end] == '\\' && !escaped) {
            escaped = true;
            text_end++;
            continue;
        }
        if (response[text_end] == '"' && !escaped) {
            break;
        }
        escaped = false;
        text_end++;
    }
    
    if (text_end >= response.length()) {
        return "echo 'Malformed text in API response'";
    }
    
    std::string generated_text = response.substr(text_start, text_end - text_start);
    
    // Unescape JSON string
    std::string unescaped_text;
    for (size_t i = 0; i < generated_text.length(); i++) {
        if (generated_text[i] == '\\' && i + 1 < generated_text.length()) {
            switch (generated_text[i + 1]) {
                case 'n': unescaped_text += '\n'; i++; break;
                case 'r': unescaped_text += '\r'; i++; break;
                case 't': unescaped_text += '\t'; i++; break;
                case '"': unescaped_text += '"'; i++; break;
                case '\\': unescaped_text += '\\'; i++; break;
                default: unescaped_text += generated_text[i]; break;
            }
        } else {
            unescaped_text += generated_text[i];
        }
    }
    
    // Decode Unicode escapes (like \u003e -> >)
    unescaped_text = decodeUnicodeEscapes(unescaped_text);
    
    // Extract shell command from the response
    size_t cmd_start = unescaped_text.find("```cmd");
    if (cmd_start != std::string::npos) {
        cmd_start += 6; // Length of "```cmd"
        size_t cmd_end = unescaped_text.find("```", cmd_start);
        if (cmd_end != std::string::npos) {
            std::string command = unescaped_text.substr(cmd_start, cmd_end - cmd_start);
            // Remove leading/trailing whitespace and newlines
            command.erase(0, command.find_first_not_of(" \t\n\r"));
            command.erase(command.find_last_not_of(" \t\n\r") + 1);
            return command;
        }
    }
    
    // If no code blocks, try to extract first line that looks like a command
    std::istringstream iss(unescaped_text);
    std::string line;
    while (std::getline(iss, line)) {
        line.erase(0, line.find_first_not_of(" \t"));
        if (!line.empty() && (line.find("dir") == 0 || line.find("copy") == 0 || 
            line.find("move") == 0 || line.find("del") == 0 || line.find("echo") == 0)) {
            return line;
        }
    }
    
    return unescaped_text;
}

std::string GeminiClient::parseSummaryResponse(const std::string& response) {
    // Parse the Gemini API response format for summarization
    // Look for candidates[0].content.parts[0].text
    
    size_t candidates_start = response.find("\"candidates\":");
    if (candidates_start == std::string::npos) {
        return "No candidates found in API response";
    }
    
    // Find the first candidate
    size_t content_start = response.find("\"content\":", candidates_start);
    if (content_start == std::string::npos) {
        return "No content found in API response";
    }
    
    // Find parts array
    size_t parts_start = response.find("\"parts\":", content_start);
    if (parts_start == std::string::npos) {
        return "No parts found in API response";
    }
    
    // Find the first part with text
    size_t text_start = response.find("\"text\"", parts_start);
    if (text_start == std::string::npos) {
        return "No text found in API response";
    }
    
    // Find the colon and opening quote after "text"
    size_t colon_pos = response.find(":", text_start);
    if (colon_pos == std::string::npos) {
        return "No colon found after text";
    }
    
    // Find the opening quote after the colon (skip any whitespace)
    size_t quote_start = colon_pos + 1;
    while (quote_start < response.length() && (response[quote_start] == ' ' || response[quote_start] == '\t')) {
        quote_start++;
    }
    
    if (quote_start >= response.length() || response[quote_start] != '"') {
        return "No opening quote found after text:";
    }
    
    text_start = quote_start + 1; // Position after the opening quote
    
    // Find the end of the text field - this is tricky because text can contain quotes
    // We need to find the closing quote that's not escaped
    size_t text_end = text_start;
    bool escaped = false;
    while (text_end < response.length()) {
        if (response[text_end] == '\\' && !escaped) {
            escaped = true;
            text_end++;
            continue;
        }
        if (response[text_end] == '"' && !escaped) {
            break;
        }
        escaped = false;
        text_end++;
    }
    
    if (text_end >= response.length()) {
        return "Malformed text in API response";
    }
    
    std::string generated_text = response.substr(text_start, text_end - text_start);
    
    // Unescape JSON string
    std::string unescaped_text;
    for (size_t i = 0; i < generated_text.length(); i++) {
        if (generated_text[i] == '\\' && i + 1 < generated_text.length()) {
            switch (generated_text[i + 1]) {
                case 'n': unescaped_text += '\n'; i++; break;
                case 'r': unescaped_text += '\r'; i++; break;
                case 't': unescaped_text += '\t'; i++; break;
                case '"': unescaped_text += '"'; i++; break;
                case '\\': unescaped_text += '\\'; i++; break;
                default: unescaped_text += generated_text[i]; break;
            }
        } else {
            unescaped_text += generated_text[i];
        }
    }
    
    // Decode Unicode escapes (like \u003e -> >)
    unescaped_text = decodeUnicodeEscapes(unescaped_text);
    
    return unescaped_text;
}

std::string GeminiClient::decodeUnicodeEscapes(const std::string& text) {
    std::string result = text;
    size_t pos = 0;
    
    while ((pos = result.find("\\u", pos)) != std::string::npos) {
        if (pos + 5 < result.length()) {
            std::string hex_code = result.substr(pos + 2, 4);
            
            try {
                // Convert hex to decimal
                int unicode_value = std::stoi(hex_code, nullptr, 16);
                
                // Replace the Unicode escape with the actual character
                result.replace(pos, 6, 1, static_cast<char>(unicode_value));
                pos += 1;
            } catch (const std::exception&) {
                // If conversion fails, skip this escape sequence
                pos += 6;
            }
        } else {
            break;
        }
    }
    
    return result;
}

} // namespace ganpi