cmake_minimum_required(VERSION 3.22)

enable_testing()

project(ORCHESTRA VERSION 0.0.1)

# Export compile commands for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add FreeType include path for Linux builds
if(UNIX AND NOT APPLE)
    include_directories(/usr/include/freetype2)
endif()

add_subdirectory(JUCE)                  # If you've put JUCE in a subdirectory called JUCE
add_subdirectory(UnitTests)
add_subdirectory(ORchestra)
