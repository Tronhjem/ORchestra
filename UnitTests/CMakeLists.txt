cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ORCHESTRA_SOURCE_DIR ../ORchestra/src/)
set(ORCHESTRA_ENGINE_DIR ${ORCHESTRA_SOURCE_DIR}/ORchestraEngine/)

# Create executable for unit tests
add_executable(ORchestraTests
    src/Main.cpp
    ${ORCHESTRA_ENGINE_DIR}/StepData.cpp
    ${ORCHESTRA_ENGINE_DIR}/DataSequence.cpp
    ${ORCHESTRA_ENGINE_DIR}/VM.cpp
    ${ORCHESTRA_ENGINE_DIR}/ErrorReporting.cpp
    ${ORCHESTRA_ENGINE_DIR}/Scanner.cpp
    ${ORCHESTRA_ENGINE_DIR}/Compiler.cpp
    ${ORCHESTRA_ENGINE_DIR}/ORchestraToken.cpp
    ${ORCHESTRA_ENGINE_DIR}/FileLoader.cpp
)

target_include_directories(ORchestraTests PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/${ORCHESTRA_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/${ORCHESTRA_SOURCE_DIR}/juceFiles
    ${CMAKE_CURRENT_SOURCE_DIR}/${ORCHESTRA_ENGINE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../UnitTests/include
)

target_compile_definitions(ORchestraTests
    PRIVATE
        _TEST=1
        $<$<CONFIG:Debug>:_DEBUG=1>
)

target_compile_features(ORchestraTests PRIVATE cxx_std_17)

# Disable -Wswitch-enum warning for all compilers
if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(ORchestraTests PRIVATE -Wno-switch-enum)
endif()

add_test(NAME ORchestraTests COMMAND ORchestraTests)
