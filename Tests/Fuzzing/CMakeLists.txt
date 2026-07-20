cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ORCHESTRA_SOURCE_DIR ../../ORchestra/src/)
set(ORCHESTRA_ENGINE_DIR ${ORCHESTRA_SOURCE_DIR}/ORchestraEngine/)

option(FUZZING_USE_LIBFUZZER "Build with libFuzzer (requires Clang)" OFF)

set(FUZZ_COMMON_SOURCES
    ${ORCHESTRA_ENGINE_DIR}/Scanner.cpp
    ${ORCHESTRA_ENGINE_DIR}/Compiler.cpp
    ${ORCHESTRA_ENGINE_DIR}/ORchestraToken.cpp
    ${ORCHESTRA_ENGINE_DIR}/ErrorReporting.cpp
)

set(FUZZ_VM_EXTRA_SOURCES
    ${ORCHESTRA_ENGINE_DIR}/VM.cpp
    ${ORCHESTRA_ENGINE_DIR}/DataSequence.cpp
    ${ORCHESTRA_ENGINE_DIR}/StepData.cpp
)

# --- Scanner/Compiler fuzzer ---
add_executable(ORchestraFuzz
    src/Fuzz_ScannerCompiler.cpp
    ${FUZZ_COMMON_SOURCES}
)

target_include_directories(ORchestraFuzz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/${ORCHESTRA_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/${ORCHESTRA_ENGINE_DIR}
)

target_compile_features(ORchestraFuzz PRIVATE cxx_std_17)

# --- VM end-to-end fuzzer ---
add_executable(ORchestraFuzzVM
    src/Fuzz_VM.cpp
    ${FUZZ_COMMON_SOURCES}
    ${FUZZ_VM_EXTRA_SOURCES}
)

target_include_directories(ORchestraFuzzVM PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/${ORCHESTRA_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/${ORCHESTRA_ENGINE_DIR}
)

target_compile_features(ORchestraFuzzVM PRIVATE cxx_std_17)

# --- Sanitizer / compiler options ---
if(FUZZING_USE_LIBFUZZER)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR "libFuzzer requires Clang. Current compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
    if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
        message(FATAL_ERROR "AppleClang does not ship libFuzzer. Install upstream Clang/LLVM (e.g. 'brew install llvm') and set CMAKE_CXX_COMPILER to the upstream clang++.")
    endif()

    target_compile_definitions(ORchestraFuzz PRIVATE FUZZING_BUILD_LIBFUZZER)
    target_compile_options(ORchestraFuzz PRIVATE -fsanitize=fuzzer,address,undefined -g)
    target_link_options(ORchestraFuzz PRIVATE -fsanitize=fuzzer,address,undefined)

    target_compile_definitions(ORchestraFuzzVM PRIVATE FUZZING_BUILD_LIBFUZZER)
    target_compile_options(ORchestraFuzzVM PRIVATE -fsanitize=fuzzer,address,undefined -g)
    target_link_options(ORchestraFuzzVM PRIVATE -fsanitize=fuzzer,address,undefined)
else()
    if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_compile_options(ORchestraFuzz PRIVATE -fsanitize=address,undefined -g -Wno-switch-enum)
        target_link_options(ORchestraFuzz PRIVATE -fsanitize=address,undefined)
        target_compile_options(ORchestraFuzzVM PRIVATE -fsanitize=address,undefined -g -Wno-switch-enum)
        target_link_options(ORchestraFuzzVM PRIVATE -fsanitize=address,undefined)
    endif()
endif()
