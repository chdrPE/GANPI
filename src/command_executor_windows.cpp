#include "ganpi.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace ganpi {

CommandExecutor::ExecutionResult CommandExecutor::execute(const std::string& command) {
    ExecutionResult result;
    
    if (command.empty()) {
        result.success = false;
        result.error = "Empty command";
        result.exit_code = -1;
        return result;
    }
    
    // Sanitize command for safety
    std::string sanitized_command = sanitizeCommand(command);
    
    if (sanitized_command.empty()) {
        result.success = false;
        result.error = "Command was filtered out for safety reasons";
        result.exit_code = -1;
        return result;
    }
    
    // Execute command using Windows _popen
    std::string full_command = sanitized_command + " 2>&1";
    
    FILE* pipe = _popen(full_command.c_str(), "r");
    if (!pipe) {
        result.success = false;
        result.error = "Failed to execute command";
        result.exit_code = -1;
        return result;
    }
    
    char buffer[128];
    std::string output;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }
    
    int exit_code = _pclose(pipe);
    result.success = (exit_code == 0);
    result.output = output;
    result.exit_code = exit_code;
    
    if (!result.success) {
        result.error = "Command failed with exit code " + std::to_string(exit_code);
    }
    
    return result;
}

CommandExecutor::ExecutionResult CommandExecutor::executeWithConfirmation(const std::string& command) {
    std::cout << "(Y/N): ";
    
    std::string response;
    std::getline(std::cin, response);
    
    // Default to yes for safe commands, no for dangerous commands
    bool should_execute = false;
    if (isDangerousCommand(command)) {
        should_execute = (response == "y" || response == "Y" || response == "yes");
    } else {
        should_execute = (response.empty() || response == "y" || response == "Y" || response == "yes");
    }
    
    if (should_execute) {
        return execute(command);
    } else {
        std::cout << "❌ Command cancelled." << std::endl;
        ExecutionResult result;
        result.success = false;
        result.error = "User cancelled";
        result.exit_code = -1;
        return result;
    }
}

std::string CommandExecutor::sanitizeCommand(const std::string& command) {
    std::string sanitized = command;
    
    // Remove leading/trailing whitespace
    sanitized.erase(0, sanitized.find_first_not_of(" \t\n\r"));
    sanitized.erase(sanitized.find_last_not_of(" \t\n\r") + 1);
    
    // Remove leading $ if present (common in shell examples)
    if (sanitized[0] == '$') {
        sanitized = sanitized.substr(1);
        sanitized.erase(0, sanitized.find_first_not_of(" \t"));
    }
    
    // Basic safety checks
    std::vector<std::string> dangerous_patterns = {
        "format",
        "del /s /q C:\\",
        "rmdir /s /q C:\\",
        "shutdown",
        "reboot"
    };
    
    std::string lower_command = sanitized;
    std::transform(lower_command.begin(), lower_command.end(), lower_command.begin(), ::tolower);
    
    for (const auto& pattern : dangerous_patterns) {
        if (lower_command.find(pattern) != std::string::npos) {
            return ""; // Filter out dangerous commands
        }
    }
    
    return sanitized;
}

bool CommandExecutor::isDangerousCommand(const std::string& command) {
    std::vector<std::string> dangerous_keywords = {
        "format",
        "del /s",
        "rmdir /s",
        "shutdown",
        "reboot"
    };
    
    std::string lower_command = command;
    std::transform(lower_command.begin(), lower_command.end(), lower_command.begin(), ::tolower);
    
    // Check if command has /q flag (quiet mode - non-interactive)
    bool has_quiet_flag = (lower_command.find("/q") != std::string::npos);
    
    for (const auto& keyword : dangerous_keywords) {
        if (lower_command.find(keyword) != std::string::npos) {
            // If it has /q flag, it's non-interactive so it's safer
            if (has_quiet_flag && (keyword == "del /s" || keyword == "rmdir /s")) {
                return false; // Non-interactive deletion is safer
            }
            return true;
        }
    }
    
    return false;
}

} // namespace ganpi
