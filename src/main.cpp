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
        
        if (!app.initialize()) {
            std::cerr << "[ERROR] Failed to initialize GANPI. Please check your configuration." << std::endl;
            return 1;
        }
        
        // Handle command line arguments
        if (argc > 1) {
            std::string command;
            
            // Check for help flag
            if (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h") {
                app.showHelp();
                return 0;
            }
            
            // Check for interactive mode
            if (std::string(argv[1]) == "--interactive" || std::string(argv[1]) == "-i") {
                app.runInteractive();
                return 0;
            }
            
            // Check for summarize mode
            if (std::string(argv[1]) == "summarize" || std::string(argv[1]) == "--summarize" || std::string(argv[1]) == "-s") {
                if (argc < 3) {
                    std::cerr << "[ERROR] Please provide a filename to summarize." << std::endl;
                    std::cerr << "Usage: ganpi summarize \"filename.txt\"" << std::endl;
                    return 1;
                }
                app.summarizeFile(argv[2]);
                return 0;
            }
            
            // Concatenate all arguments as the natural language command
            for (int i = 1; i < argc; ++i) {
                if (i > 1) command += " ";
                command += argv[i];
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
