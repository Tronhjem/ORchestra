cmake_minimum_required(VERSION 3.22)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(ORCHESTRA VERSION 0.2.4)

option(BUILD_PLUGIN "Build the ORchestra plugin (requires JUCE)" ON)
option(BUILD_TESTS "Build the unit tests (no JUCE dependency)" ON)
option(BUILD_FUZZING "Build the fuzzing targets (no JUCE dependency)" OFF)
option(BUILD_STRESS "Build the engine stress harness (requires JUCE)" OFF)
option(ENABLE_TSAN "Instrument the stress harness with ThreadSanitizer" OFF)

if(BUILD_PLUGIN)
    add_subdirectory(JUCE)
    add_subdirectory(ORchestra)
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Tests/UnitTests)
endif()

if(BUILD_FUZZING)
    add_subdirectory(Tests/Fuzzing)
endif()

if(BUILD_STRESS)
    if(NOT BUILD_PLUGIN)
        add_subdirectory(JUCE)
    endif()
    add_subdirectory(Tests/StressTest)
endif()
