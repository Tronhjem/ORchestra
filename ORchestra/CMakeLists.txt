cmake_minimum_required(VERSION 3.22)

file(GLOB_RECURSE SRC_FILES "src/*/*.cpp")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ===================================================================================================
# PLGUINS 
# ===================================================================================================
juce_add_plugin(ORchestra
    VERSION 0.1
    # ICON_BIG ...
    # ICON_SMALL ...
    COMPANY_NAME SonoTron
    # IS_SYNTH TRUE/FALSE
    # NEEDS_MIDI_INPUT TRUE/FALSE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE SNTN
    PLUGIN_CODE ORCS
    FORMATS AU VST3
    PRODUCT_NAME "ORchestra")

juce_generate_juce_header(ORchestra)

target_sources(ORchestra
    PRIVATE
        ${SRC_FILES}
    )

target_include_directories(ORchestra PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/juceFiles
    ${CMAKE_CURRENT_SOURCE_DIR}/src/juceFiles/components
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ORchestraEngine
)

target_compile_definitions(ORchestra
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(ORchestra
    PRIVATE
        juce::juce_audio_utils
        juce::juce_recommended_warning_flags
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags)

# ===================================================================================================
# STANDLONE 
# ===================================================================================================
juce_add_plugin(ORchestra_Standalone
    VERSION 0.1
    # ICON_BIG 
    # ICON_SMALL 
    COMPANY_NAME SonoTron
    # IS_SYNTH TRUE/FALSE
    # NEEDS_MIDI_INPUT TRUE/FALSE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE SNTN
    PLUGIN_CODE ORCS
    FORMATS Standalone
    PRODUCT_NAME "ORchestra")

juce_generate_juce_header(ORchestra_Standalone)

target_sources(ORchestra_Standalone
    PRIVATE
        ${SRC_FILES}
    )

target_include_directories(ORchestra_Standalone PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/juceFiles
    ${CMAKE_CURRENT_SOURCE_DIR}/src/juceFiles/components
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ORchestraEngine
)

target_compile_definitions(ORchestra_Standalone
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(ORchestra_Standalone
    PRIVATE
        juce::juce_audio_utils
        juce::juce_recommended_warning_flags
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags)

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
