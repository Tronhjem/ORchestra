#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "StepData.h"
#include "TriggerRectangle.h"


using namespace ORchestra;
constexpr int TIMELINE_STEPS_DRAWN = 20;
constexpr int TIMELINE_ROWS_DRAWN = 5;

constexpr float trackHeight = 32.f;
constexpr float stepMargin = 2.5f;

constexpr float quaterStepHeight = trackHeight * 0.25f;
constexpr float stepHeight = trackHeight;
constexpr float stepWidth = (trackHeight * 1.5f);
constexpr float drawnStepHeight = stepHeight - stepMargin;
constexpr float drawnStepWidth = stepWidth - stepMargin;

constexpr float stepYIncrement = trackHeight + (trackHeight - stepHeight) / 2.0f;
constexpr float stepXIncrement = stepWidth + stepWidth / 2.0f - stepWidth / 2.0f;

constexpr float roundedCornerSize = 5.f;

class Timeline : public juce::Component, public juce::Timer
{
public:
    Timeline(TriggerRectangleComponent& triggerRectangle) :
                mAudioProcessor(nullptr),
                mLastGlobalStep(-1),
                mLastTimeInSamples(0),
                mTriggerRectangle(triggerRectangle)
    {
        startTimerHz(40);
        mUniqueNoteValues.reserve(8);
        mTimelineTriggerRectangles.reserve(TIMELINE_STEPS_DRAWN * 5);
    }

    ~Timeline() override
    {
        stopTimer();
    }

    void SetProcessor(ORchestraAudioProcessor* audioProcessor) { mAudioProcessor = audioProcessor; }
    void timerCallback() override;
    void paint(juce::Graphics& g) override;

private:
    void UpdateTriggerRectangles(juce::Graphics& g);
    inline juce::Colour GetStepColorFromVelocity(const float velocity);
    ORchestraAudioProcessor* mAudioProcessor;
    int mLastGlobalStep;
    int64_t mLastTimeInSamples;
    TriggerRectangleComponent& mTriggerRectangle;
    std::vector<DataUnit> mUniqueNoteValues;
    std::vector<TriggerRectangle> mTimelineTriggerRectangles;
};
