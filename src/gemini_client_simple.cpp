#include "ganpi.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <cstdio>
#include <vector>
#include <set>
#include <string>

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
    
    context += "\n===========================\n\n";
    return context;
}

GeminiClient::GeminiClient(const std::string& api_key) 
    : api_key_(api_key), model_("gemini-pro") {
}

std::string extractDirectoryName(const std::string& text, const std::string& keyword) {
    // Try to find "the <name> directory" or "into <name>" or "from <name>"
    std::regex dir_pattern(keyword + R"(\s+(?:the\s+)?(\w+)\s+(?:directory|dir|folder))");
    std::smatch match;
    if (std::regex_search(text, match, dir_pattern)) {
        return match[1].str();
    }
    return "";
}

std::string extractNestedPath(const std::string& text, const std::string& dir_name) {
    // Check if directory is nested: "dir1 in test", "dir1 directory in test", etc.
    std::regex nested_pattern(dir_name + R"(\s+(?:directory|dir|folder)?\s+in\s+(?:the\s+)?(\w+))");
    std::smatch match;
    if (std::regex_search(text, match, nested_pattern)) {
        // Found "dir1 in test" -> return "test/dir1" (Unix-style path)
        return match[1].str() + "/" + dir_name;
    }
    return dir_name;
}

std::string GeminiClient::interpretCommand(const std::string& natural_language) {
    // Gather and display file system context
    std::string context = getFileSystemContext(natural_language);
    std::cout << "\n📂 Analyzing file system context..." << std::endl;
    std::cout << context << std::endl;
    
    // For demo purposes, return a simple command based on keywords
    // In a real implementation, this would call the Gemini API with this context
    
    std::string lower_input = natural_language;
    std::transform(lower_input.begin(), lower_input.end(), lower_input.begin(), ::tolower);
    
    // Enhanced pattern matching
    
    // Move files command - extract source and destination directories
    if (lower_input.find("move") != std::string::npos && 
        (lower_input.find("directory") != std::string::npos || lower_input.find("dir") != std::string::npos)) {
        
        std::string source_dir = extractDirectoryName(lower_input, "(?:in|from)");
        std::string dest_dir = extractDirectoryName(lower_input, "(?:into|to)");
        
        // Fallback: try to extract directory names without keywords
        if (source_dir.empty() || dest_dir.empty()) {
            std::regex simple_dir(R"(\b(test|dir1|dir2|documents?|downloads?|backup|temp)\b)");
            std::smatch match;
            std::string::const_iterator searchStart(lower_input.cbegin());
            std::vector<std::string> dirs;
            
            while (std::regex_search(searchStart, lower_input.cend(), match, simple_dir)) {
                dirs.push_back(match[0].str());
                searchStart = match.suffix().first;
            }
            
            if (dirs.size() >= 2) {
                source_dir = dirs[0];
                dest_dir = dirs[1];
            }
        }
        
        // Check if destination is nested inside another directory
        if (!dest_dir.empty()) {
            dest_dir = extractNestedPath(lower_input, dest_dir);
        }
        
        // Check if we also need to display contents
        bool display_after = (lower_input.find("display") != std::string::npos || 
                             lower_input.find("show") != std::string::npos ||
                             lower_input.find("list") != std::string::npos) &&
                            (lower_input.find("then") != std::string::npos || 
                             lower_input.find("and") != std::string::npos);
        
        if (!source_dir.empty() && !dest_dir.empty()) {
            // Check if destination is a subdirectory of source (nested structure)
            bool is_nested = (dest_dir.find(source_dir + "/") == 0);
            
            std::string command;
            if (is_nested) {
                // For nested moves, only move files (not subdirectories) to avoid recursion
                // Use find to get only files, then mv them
                command = "find " + source_dir + "/ -maxdepth 1 -type f -exec mv {} " + dest_dir + "/ \\;";
            } else {
                // For non-nested moves, move everything
                command = "mv " + source_dir + "/* " + dest_dir + "/";
            }
            
            if (display_after) {
                command += " && ls -la " + dest_dir;
            }
            return command;
        }
    }
    
    // List/display directory contents
    if ((lower_input.find("list") != std::string::npos || 
         lower_input.find("display") != std::string::npos || 
         lower_input.find("show") != std::string::npos) && 
        (lower_input.find("file") != std::string::npos || 
         lower_input.find("content") != std::string::npos ||
         lower_input.find("directory") != std::string::npos)) {
        
        // Check if user means current directory
        if (lower_input.find("current") != std::string::npos || 
            lower_input.find("this") != std::string::npos ||
            lower_input.find("the directory") != std::string::npos ||
            lower_input.find("here") != std::string::npos) {
            return "ls -la";
        }
        
        std::string dir_name = extractDirectoryName(lower_input, "(?:of|in)");
        if (dir_name.empty()) {
            // Try to find any directory name (excluding common words)
            std::regex simple_dir(R"(\b(test|dir1|dir2|documents?|downloads?|backup|temp|home|desktop)\b)");
            std::smatch match;
            if (std::regex_search(lower_input, match, simple_dir)) {
                dir_name = match[0].str();
            }
        }
        
        // Check if it's a nested path
        if (!dir_name.empty()) {
            dir_name = extractNestedPath(lower_input, dir_name);
            return "ls -la " + dir_name;
        }
        return "ls -la";
    }
    
    // Find PDFs
    if (lower_input.find("find") != std::string::npos && lower_input.find("pdf") != std::string::npos) {
        return "find . -name '*.pdf' -type f";
    }
    
    // Zip files
    if (lower_input.find("zip") != std::string::npos && lower_input.find("pdf") != std::string::npos) {
        return "zip documents.zip *.pdf";
    }
    
    // Move txt files (legacy)
    if (lower_input.find("move") != std::string::npos && lower_input.find("txt") != std::string::npos) {
        return "mv *.txt Documents/notes/";
    }
    
    // Delete temp files
    if (lower_input.find("delete") != std::string::npos && 
        (lower_input.find("temp") != std::string::npos || lower_input.find("temporary") != std::string::npos)) {
        return "rm -rf *.tmp";
    }
    
    // Start server
    if ((lower_input.find("server") != std::string::npos || lower_input.find("serve") != std::string::npos) && 
        lower_input.find("http") != std::string::npos) {
        return "python -m http.server 8080";
    }
    
    // Default
    return "echo 'Command not recognized. Try: move files from dir1 to dir2, list files in dir1, find PDFs, etc.'";
}

bool GeminiClient::validateApiKey() {
    // For demo purposes, always return true
    return !api_key_.empty();
}

std::string GeminiClient::makeHttpRequest(const std::string& url, const std::string& data) {
    // Placeholder - would make actual HTTP request in real implementation
    return "";
}

std::string GeminiClient::buildPrompt(const std::string& user_input, const std::string& fs_context) {
    // In the simple implementation, we don't actually use the prompt for API calls
    // but we keep the signature consistent
    return "Convert to shell command: " + user_input + "\n" + fs_context;
}

} // namespace ganpi
