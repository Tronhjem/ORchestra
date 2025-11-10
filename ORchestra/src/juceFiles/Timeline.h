#pragma once

#include <JuceHeader.h>
#include "StepData.h"
#include "ORchestraEngine.h"
#include "PluginProcessor.h"


constexpr int TIMELINE_STEPS_DRAWN = 17;
constexpr int TIMELINE_ROWS_DRAWN = 5;


class Timeline : public juce::Component, public juce::Timer
{
public:
    Timeline() :
        mAudioProcessor(nullptr),
        mLastGlobalStep(-1),
        mPixelsPerBeat(0.f),
        mLastTimeInSamples(0)
    {
        startTimerHz(25);
    }
    
    ~Timeline() override
    {
        stopTimer();
    }
    
    void SetProcessor(ORchestraAudioProcessor* audioProcessor) { mAudioProcessor = audioProcessor; }
    void timerCallback() override;
    void paint(juce::Graphics& g) override;
    
private:
    ORchestraAudioProcessor* mAudioProcessor;
    int mLastGlobalStep;
    float mPixelsPerBeat;
    int64_t mLastTimeInSamples;
    std::array<std::array<int, TIMELINE_ROWS_DRAWN>, TIMELINE_STEPS_DRAWN> mStepXPositions;
};
