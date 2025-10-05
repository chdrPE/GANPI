#include "ganpi.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>

namespace ganpi {

std::unique_ptr<Config> Config::instance_ = nullptr;

Config& Config::getInstance() {
    if (!instance_) {
        instance_ = std::unique_ptr<Config>(new Config());
    }
    return *instance_;
}

void Config::setGeminiApiKey(const std::string& key) {
    gemini_api_key_ = key;
}

std::string Config::getGeminiApiKey() const {
    return gemini_api_key_;
}

void Config::setModel(const std::string& model) {
    model_ = model;
}

std::string Config::getModel() const {
    return model_;
}

bool Config::loadFromFile(const std::string& filename) {
    try {
        // Try to get home directory
        std::string config_path;
        const char* home = getenv("USERPROFILE"); // Windows
        if (!home) {
            home = getenv("HOME"); // Unix/Linux
        }
        
        if (home && filename[0] == '.') {
            // If filename starts with . and we have home dir, use home directory
            config_path = std::string(home) + "/" + filename;
        } else {
            config_path = filename;
        }
        
        // Use C-style file operations for better error handling
        FILE* f = fopen(config_path.c_str(), "r");
        if (!f) {
            return false;
        }
        
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), f)) {
            std::string line(buffer);
            
            // Remove newline
            if (!line.empty() && line[line.length()-1] == '\n') {
                line.erase(line.length()-1);
            }
            if (!line.empty() && line[line.length()-1] == '\r') {
                line.erase(line.length()-1);
            }
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Find the = sign
            size_t eq_pos = line.find('=');
            if (eq_pos != std::string::npos) {
                std::string key = line.substr(0, eq_pos);
                std::string value = line.substr(eq_pos + 1);
                
                // Remove whitespace
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                if (key == "GEMINI_API_KEY") {
                    gemini_api_key_ = value;
                } else if (key == "MODEL") {
                    model_ = value;
                }
            }
        }
        
        fclose(f);
        return true;
    } catch (...) {
        return false;
    }
}

void Config::saveToFile(const std::string& filename) {
    // Try to get home directory
    std::string config_path;
    const char* home = getenv("USERPROFILE"); // Windows
    if (!home) {
        home = getenv("HOME"); // Unix/Linux
    }
    
    if (home && filename[0] == '.') {
        // If filename starts with . and we have home dir, use home directory
        config_path = std::string(home) + "/" + filename;
    } else {
        config_path = filename;
    }
    
    std::ofstream file(config_path);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not save config to " << config_path << std::endl;
        return;
    }
    
    file << "GEMINI_API_KEY=" << gemini_api_key_ << std::endl;
    file << "MODEL=" << model_ << std::endl;
    
    file.close();
    std::cout << "   Config saved to: " << config_path << std::endl;
}

} // namespace ganpi
