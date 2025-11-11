cmake_minimum_required(VERSION 3.22)


juce_add_console_app(ORchestraTests
    PRODUCT_NAME "ORchestraTests")

juce_generate_juce_header(ORchestraTests)
set(ORCHESTRA_SOURCE_DIR ../ORchestra/src/)
set(ORCHESTRA_ENGINE_DIR ${ORCHESTRA_SOURCE_DIR}/ORchestraEngine/)

target_sources(ORchestraTests
    PRIVATE
        src/Main.cpp
        ${ORCHESTRA_ENGINE_DIR}/StepData.cpp
        ${ORCHESTRA_ENGINE_DIR}/DataSequence.cpp
        ${ORCHESTRA_ENGINE_DIR}/VM.cpp
        ${ORCHESTRA_ENGINE_DIR}/ErrorReporting.cpp
        ${ORCHESTRA_ENGINE_DIR}/Scanner.cpp
        ${ORCHESTRA_ENGINE_DIR}/Compiler.cpp
        ${ORCHESTRA_ENGINE_DIR}/Token.cpp
)


target_include_directories(ORchestraTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${ORCHESTRA_SOURCE_DIR}/utils)
target_include_directories(ORchestraTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${ORCHESTRA_SOURCE_DIR}/juceFiles)
target_include_directories(ORchestraTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${ORCHESTRA_ENGINE_DIR})

target_compile_definitions(ORchestraTests
    PRIVATE
        _TEST=1
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0)

target_link_libraries(ORchestraTests
    PRIVATE
        juce::juce_core
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)

# Disable -Wswitch-enum warning for all compilers
# This warning requires exhaustive case handling for all enum values, even when a default case is present
if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    get_target_property(WARNING_FLAGS juce_recommended_warning_flags INTERFACE_COMPILE_OPTIONS)
    list(REMOVE_ITEM WARNING_FLAGS "-Wswitch-enum")
    list(APPEND WARNING_FLAGS "-Wno-switch-enum")
    set_target_properties(juce_recommended_warning_flags PROPERTIES INTERFACE_COMPILE_OPTIONS "${WARNING_FLAGS}")
endif()

add_test(NAME ORchestraTests COMMAND ORchestraTests)
