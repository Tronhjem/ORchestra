cmake_minimum_required(VERSION 3.22)

enable_testing()

project(ORCHESTRA VERSION 0.0.1)

# Export compile commands for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(JUCE)
add_subdirectory(UnitTests)
add_subdirectory(ORchestra)
