#include "ganpi.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>

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
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key, value;
        
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
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
    
    file.close();
    return true;
}

void Config::saveToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not save config to " << filename << std::endl;
        return;
    }
    
    file << "GEMINI_API_KEY=" << gemini_api_key_ << std::endl;
    file << "MODEL=" << model_ << std::endl;
    
    file.close();
}

} // namespace ganpi
