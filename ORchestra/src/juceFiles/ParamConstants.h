#pragma once
#include <JuceHeader.h>

const StringRef bpmString{ "bpm" };
const StringRef tempoDivisionString{ "tempo-division" };
const StringRef noteLengthString{ "note-length" };
const StringRef syncToggleString{ "sync-toggle" };

const juce::ParameterID bpmParamId{ bpmString, 1 };
const juce::ParameterID tempoDivisionId{ tempoDivisionString, 1 };
const juce::ParameterID noteLengthId{ noteLengthString, 1 };
const juce::ParameterID syncToggleId{ syncToggleString, 1 };
