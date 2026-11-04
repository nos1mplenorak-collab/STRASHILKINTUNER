cmake_minimum_required(VERSION 3.22)
project(ShtrashilkinTuner VERSION 1.0.0)

include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.8
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(ShtrashilkinTuner
    COMPANY_NAME "Shtrashilkin"
    PLUGIN_MANUFACTURER_CODE Shtr
    PLUGIN_CODE Tun1
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    FORMATS VST3
    PRODUCT_NAME "Shtrashilkin Tuner"
    COPY_PLUGIN_AFTER_BUILD FALSE)

target_sources(ShtrashilkinTuner PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp)

target_compile_definitions(ShtrashilkinTuner PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

if (MSVC)
    target_compile_options(ShtrashilkinTuner PRIVATE /utf-8)
endif()

target_link_libraries(ShtrashilkinTuner PRIVATE
    juce::juce_audio_utils
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)
