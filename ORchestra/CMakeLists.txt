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
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

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
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# Disable -Wswitch-enum warning for all compilers
# This warning requires exhaustive case handling for all enum values, even when a default case is present
# We need to modify the interface compile options of juce_recommended_warning_flags to remove -Wswitch-enum
# and then add -Wno-switch-enum
if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    get_target_property(WARNING_FLAGS juce_recommended_warning_flags INTERFACE_COMPILE_OPTIONS)
    list(REMOVE_ITEM WARNING_FLAGS "-Wswitch-enum")
    list(APPEND WARNING_FLAGS "-Wno-switch-enum")
    set_target_properties(juce_recommended_warning_flags PROPERTIES INTERFACE_COMPILE_OPTIONS "${WARNING_FLAGS}")
endif()
