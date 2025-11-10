cmake_minimum_required(VERSION 3.22)

# project(ORCHESTRA_TESTS VERSION 0.0.1)

# find_package(JUCE CONFIG REQUIRED)        # If you've installed JUCE to your system
# or
# add_subdirectory(../JUCE ${CMAKE_CURRENT_BINARY_DIR}/build)                    # If you've put JUCE in a subdirectory called JUCE

juce_add_console_app(ORchestraTests
    PRODUCT_NAME "ORchestraTests"
    )     # The name of the final executable, which can differ from the target name

# file(GLOB_RECURSE TEST_SRC_FILES "../src/**/*.cpp")

juce_generate_juce_header(ORchestraTests)

target_sources(ORchestraTests
    PRIVATE
        src/Main.cpp
        ../src/ORchestraEngine/DataSequenceStep.cpp
        ../src/ORchestraEngine/DataSequence.cpp
        ../src/ORchestraEngine/VM.cpp
        ../src/ORchestraEngine/ErrorReporting.cpp
        ../src/ORchestraEngine/Scanner.cpp
        ../src/ORchestraEngine/Compiler.cpp
        ../src/ORchestraEngine/Token.cpp
)

target_include_directories(ORchestraTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src/utils)
target_include_directories(ORchestraTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src/juceFiles)
target_include_directories(ORchestraTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src/ORchestraEngine)

target_compile_definitions(ORchestraTests
    PRIVATE
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_console_app` call
        JUCE_USE_CURL=0)    # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_console_app` call

target_link_libraries(ORchestraTests
    PRIVATE
        juce::juce_core
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)
