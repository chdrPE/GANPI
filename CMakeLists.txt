cmake_minimum_required(VERSION 3.16)
project(GANPI VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Source files - use Windows-specific versions
if(WIN32)
    # Windows-specific source files
    set(SOURCES
        src/main.cpp
        src/ganpi.cpp
        src/config.cpp
        src/gemini_client_windows.cpp
        src/command_executor_windows.cpp
    )
else()
    # Linux/macOS source files with curl
    find_package(CURL REQUIRED)
    find_package(nlohmann_json REQUIRED)
    file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
endif()

# Create executable
add_executable(ganpi ${SOURCES})

# Link libraries
if(WIN32)
    target_link_libraries(ganpi wininet)
    # Add Windows-specific libraries for _popen and other functions
    if(MSVC)
        target_link_libraries(ganpi)
    endif()
else()
    target_link_libraries(ganpi ${CURL_LIBRARIES} nlohmann_json::nlohmann_json)
endif()

# Compiler flags
if(MSVC)
    target_compile_options(ganpi PRIVATE /W4)
else()
    target_compile_options(ganpi PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install target
install(TARGETS ganpi DESTINATION bin)