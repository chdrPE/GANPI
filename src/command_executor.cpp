#include "ganpi.h"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <regex>
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
    
    // Execute command and capture output
    std::string full_command = sanitized_command + " 2>&1";
    
    FILE* pipe = popen(full_command.c_str(), "r");
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
    
    int exit_code = pclose(pipe);
    result.success = (exit_code == 0);
    result.output = output;
    result.exit_code = exit_code;
    
    if (!result.success) {
        result.error = "Command failed with exit code " + std::to_string(exit_code);
    }
    
    return result;
}

CommandExecutor::ExecutionResult CommandExecutor::executeWithConfirmation(const std::string& command) {
    std::cout << "\n🔍 Command to execute:" << std::endl;
    std::cout << "   " << command << std::endl;
    
    if (isDangerousCommand(command)) {
        std::cout << "\n⚠️  WARNING: This command may be potentially dangerous!" << std::endl;
        std::cout << "   Proceed? (y/N): ";
    } else {
        std::cout << "\n   Execute? (Y/n): ";
    }
    
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
        std::cout << "\n🚀 Executing..." << std::endl;
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
        "rm -rf /",
        "sudo rm -rf",
        "format",
        "mkfs",
        ":(){ :|:& };:",  // Fork bomb
        "dd if=/dev/urandom",
        "shutdown",
        "reboot",
        "halt",
        "poweroff"
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
        "rm -rf",
        "sudo",
        "format",
        "mkfs",
        "dd",
        "shutdown",
        "reboot",
        "halt",
        "poweroff",
        "chmod 777",
        "chown root",
        "passwd"
    };
    
    std::string lower_command = command;
    std::transform(lower_command.begin(), lower_command.end(), lower_command.begin(), ::tolower);
    
    for (const auto& keyword : dangerous_keywords) {
        if (lower_command.find(keyword) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

} // namespace ganpi
