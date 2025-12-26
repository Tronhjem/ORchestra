#pragma once

#include <JuceHeader.h>
#include <vector>

#include "PluginProcessor.h"

constexpr int updateFrequency = 30;
constexpr float miliesecondsPerFrameInverse = 1.f / (1000.f / static_cast<float>(updateFrequency));

namespace ORchestra
{
    struct TriggerRectangle
    {
        float x;
        float y;
        float width;
        float value;
    };

    class TriggerRectangleComponent : public juce::Component, public juce::Timer
    {
    public:
        TriggerRectangleComponent()
        {
            startTimerHz(updateFrequency);
        }
        
        ~TriggerRectangleComponent() override
        {
            stopTimer();
        }
        

        void AddRectangle(TriggerRectangle rect) { triggerRectangles.emplace_back(rect); }
        void ClearRectangles() { triggerRectangles.clear(); }
        void SetProcessor(ORchestraAudioProcessor* audioProcessor) { mAudioProcessor = audioProcessor; }
        
    private:
        ORchestraAudioProcessor* mAudioProcessor;
        void timerCallback() override;
        void paint(juce::Graphics& g) override;
        
        std::vector<TriggerRectangle> triggerRectangles;
    };
} // namespace ORchestra
