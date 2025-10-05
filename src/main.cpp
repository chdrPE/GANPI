#include "ganpi.h"
#include <iostream>
#include <string>

using namespace ganpi;

void printBanner() {
    std::cout << R"(
    ╔══════════════════════════════════════════════════════════════╗
    ║                                                              ║
    ║    G.A.N.P.I. - Gemini-Assisted Natural Processing         ║
    ║                     Interface                                ║
    ║                                                              ║
    ║    "Talk to your terminal like a human!"                     ║
    ║                                                              ║
    ╚══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        GANPI app;
        
        // Parse flags first
        bool verbose_output = false;
        bool show_context = false;
        std::vector<std::string> command_args;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "-o" || arg == "--output") {
                verbose_output = true;
            } else if (arg == "-c" || arg == "--context") {
                show_context = true;
            } else if (arg == "--help" || arg == "-h") {
                app.showHelp();
                return 0;
            } else if (arg == "--interactive" || arg == "-i") {
                if (!app.initialize(verbose_output, show_context)) {
                    std::cerr << "[ERROR] Failed to initialize GANPI. Please check your configuration." << std::endl;
                    return 1;
                }
                app.runInteractive();
                return 0;
            } else if (arg == "summarize" || arg == "--summarize" || arg == "-s") {
                if (i + 1 >= argc) {
                    std::cerr << "[ERROR] Please provide a filename to summarize." << std::endl;
                    std::cerr << "Usage: ganpi summarize \"filename.txt\"" << std::endl;
                    return 1;
                }
                if (!app.initialize(verbose_output, show_context)) {
                    std::cerr << "[ERROR] Failed to initialize GANPI. Please check your configuration." << std::endl;
                    return 1;
                }
                app.summarizeFile(argv[i + 1]);
                return 0;
            } else {
                // Not a flag, add to command
                command_args.push_back(arg);
            }
        }
        
        if (!app.initialize(verbose_output, show_context)) {
            std::cerr << "[ERROR] Failed to initialize GANPI. Please check your configuration." << std::endl;
            return 1;
        }
        
        // Handle command line arguments
        if (!command_args.empty()) {
            // Concatenate all command arguments
            std::string command;
            for (size_t i = 0; i < command_args.size(); ++i) {
                if (i > 0) command += " ";
                command += command_args[i];
            }
            
            app.processCommand(command);
        } else {
            // No arguments provided, show help
            app.showHelp();
            std::cout << "\n[TIP] Try: ganpi \"Find all PDF files in Downloads and zip them\"" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
