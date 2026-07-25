cmake_minimum_required(VERSION 3.22)

option(STRESS_ASSERTS "Enable _DEBUG asserts and race-window widening in the stress target" ON)

juce_add_console_app(ORchestraStress
    PRODUCT_NAME "ORchestraStress")

juce_generate_juce_header(ORchestraStress)

set(ORCHESTRA_ENGINE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../ORchestra/src/ORchestraEngine)

target_sources(ORchestraStress
    PRIVATE
        main.cpp
        ${ORCHESTRA_ENGINE_DIR}/ORchestraEngine.cpp
        ${ORCHESTRA_ENGINE_DIR}/MidiScheduler.cpp
        ${ORCHESTRA_ENGINE_DIR}/VM.cpp
        ${ORCHESTRA_ENGINE_DIR}/Scanner.cpp
        ${ORCHESTRA_ENGINE_DIR}/Compiler.cpp
        ${ORCHESTRA_ENGINE_DIR}/DataSequence.cpp
        ${ORCHESTRA_ENGINE_DIR}/StepData.cpp
        ${ORCHESTRA_ENGINE_DIR}/ErrorReporting.cpp
        ${ORCHESTRA_ENGINE_DIR}/FileLoader.cpp
        ${ORCHESTRA_ENGINE_DIR}/ORchestraToken.cpp
)

target_include_directories(ORchestraStress PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../ORchestra/src/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/../../ORchestra/src/juceFiles
    ${ORCHESTRA_ENGINE_DIR}
)

target_compile_definitions(ORchestraStress PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

if(STRESS_ASSERTS)
    target_compile_definitions(ORchestraStress PRIVATE
        _DEBUG=1
        ORCHESTRA_RACE_WIDEN=1
    )
endif()

target_link_libraries(ORchestraStress PRIVATE
    juce::juce_audio_basics
    juce::juce_recommended_warning_flags
    juce::juce_recommended_config_flags
)

if(ENABLE_TSAN)
    target_compile_options(ORchestraStress PRIVATE -fsanitize=thread -g)
    target_link_options(ORchestraStress PRIVATE -fsanitize=thread)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(ORchestraStress PRIVATE -Wno-switch-enum)
endif()
