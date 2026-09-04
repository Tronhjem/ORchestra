cmake_minimum_required(VERSION 3.22)

file(GLOB_RECURSE SRC_FILES "src/*/*.cpp")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(ORCHESTRA_PLUGIN_NAME ORchestra)
set(ORCHESTRA_COMPANY_NAME Spektakel)
set(ORCHESTRA_MANUFACTURER_CODE SPTKL)
set(ORCHESTRA_PLUGIN_CODE ORCS)
set(ORCHESTRA_PRODUCT_NAME "ORchestra")

set(ORCHESTRA_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/juceFiles
    ${CMAKE_CURRENT_SOURCE_DIR}/src/juceFiles/components
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ORchestraEngine
)

set(ORCHESTRA_COMPILE_DEFINITIONS
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

# Beta/pre-release builds: file logging + crash handler without the debug
# features (asserts, ScopedTimer, AssertMutex stay _DEBUG-only).
if(ORCHESTRA_ENABLE_LOGGING)
    list(APPEND ORCHESTRA_COMPILE_DEFINITIONS ORCHESTRA_ENABLE_LOGGING=1)
endif()

set(ORCHESTRA_LINK_LIBRARIES
    PRIVATE
        juce::juce_audio_utils
        juce::juce_recommended_warning_flags
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
)

function(add_orchestra_format target format is_midi_effect)
    juce_add_plugin(${target}
        VERSION ${CMAKE_PROJECT_VERSION}
        COMPANY_NAME ${ORCHESTRA_COMPANY_NAME}
        NEEDS_MIDI_OUTPUT TRUE
        IS_MIDI_EFFECT ${is_midi_effect}
        EDITOR_WANTS_KEYBOARD_FOCUS TRUE
        COPY_PLUGIN_AFTER_BUILD TRUE
        PLUGIN_MANUFACTURER_CODE ${ORCHESTRA_MANUFACTURER_CODE}
        PLUGIN_CODE ${ORCHESTRA_PLUGIN_CODE}
        FORMATS ${format}
        PRODUCT_NAME ${ORCHESTRA_PRODUCT_NAME})

    juce_generate_juce_header(${target})

    target_sources(${target} PRIVATE ${SRC_FILES})

    target_include_directories(${target} PRIVATE ${ORCHESTRA_INCLUDE_DIRS})

    target_compile_definitions(${target} PUBLIC ${ORCHESTRA_COMPILE_DEFINITIONS})

    target_link_libraries(${target} ${ORCHESTRA_LINK_LIBRARIES})
endfunction()

# ===================================================================================================
# VST
# ===================================================================================================
add_orchestra_format(ORchestra_VST "VST3" FALSE)

# ===================================================================================================
# AU (macOS only)
# ===================================================================================================
if(APPLE)
    add_orchestra_format(ORchestra_AU "AU" TRUE)
endif()

# ===================================================================================================
# STANDALONE
# ===================================================================================================
add_orchestra_format(ORchestra_Standalone "Standalone" FALSE)

# Disable -Wswitch-enum: this warning requires exhaustive case handling for all
# enum values even when a default case is present, which is unnecessary for us.
# We also modify the JUCE interface target to remove -Wswitch-enum so it doesn't
# conflict with our override (last flag wins in clang).
if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    get_target_property(_warn_flags juce_recommended_warning_flags INTERFACE_COMPILE_OPTIONS)
    list(REMOVE_ITEM _warn_flags "-Wswitch-enum")
    list(APPEND _warn_flags "-Wno-switch-enum")
    set_target_properties(juce_recommended_warning_flags PROPERTIES INTERFACE_COMPILE_OPTIONS "${_warn_flags}")
endif()
