#pragma once

const StringRef bpmString{ "bpm" };
const StringRef tempoDivisionString{ "tempo-division" };
const StringRef noteLengthString{ "note-length" };

const juce::ParameterID bpmParamId{ bpmString, 1 };
const juce::ParameterID tempoDivisionId{ tempoDivisionString, 1 };
const juce::ParameterID noteLengthId{ noteLengthString, 1 };
