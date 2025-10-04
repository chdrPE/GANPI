cmake_minimum_required(VERSION 3.16)
project(GANPI VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

# Find libcurl
find_package(CURL REQUIRED)

# Find nlohmann/json
find_package(nlohmann_json REQUIRED)

# Include directories
include_directories(include)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")

# Create executable
add_executable(ganpi ${SOURCES})

# Link libraries
target_link_libraries(ganpi 
    ${CURL_LIBRARIES}
    nlohmann_json::nlohmann_json
    Threads::Threads
)

# Compiler flags
if(MSVC)
    target_compile_options(ganpi PRIVATE /W4)
else()
    target_compile_options(ganpi PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install target
install(TARGETS ganpi DESTINATION bin)
