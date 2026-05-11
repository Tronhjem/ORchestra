cmake_minimum_required(VERSION 3.22)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(ORCHESTRA VERSION 0.0.1)

option(BUILD_PLUGIN "Build the ORchestra plugin (requires JUCE)" ON)
option(BUILD_TESTS "Build the unit tests (no JUCE dependency)" ON)
option(BUILD_FUZZING "Build the fuzzing targets (no JUCE dependency)" OFF)

if(BUILD_PLUGIN)
    add_subdirectory(JUCE)
    add_subdirectory(ORchestra)
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(UnitTests)
endif()

if(BUILD_FUZZING)
    add_subdirectory(Fuzzing)
endif()
