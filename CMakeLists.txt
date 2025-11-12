cmake_minimum_required(VERSION 3.22)

project(ORCHESTRA VERSION 0.0.1)

# Build options to allow building plugin and tests separately
option(BUILD_PLUGIN "Build the ORchestra plugin (requires JUCE)" ON)
option(BUILD_TESTS "Build the unit tests (no JUCE dependency)" ON)

# Only add JUCE if building the plugin
if(BUILD_PLUGIN)
    add_subdirectory(JUCE)
    add_subdirectory(ORchestra)
endif()

# Tests can be built independently without JUCE
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(UnitTests)
endif()
