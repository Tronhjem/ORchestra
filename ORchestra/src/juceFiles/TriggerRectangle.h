#pragma once

#include <JuceHeader.h>
#include <vector>

constexpr float alphaDecrementPerFrame = 1.f/100.f;
    
namespace ORchestra
{
    struct TriggerRectangle
    {
        float x;
        float y;
        float alpha;
    };

    class TriggerRectangleComponent : public juce::Component, public juce::Timer
    {
    public:
        TriggerRectangleComponent()
        {
            startTimerHz(30);
        }
        
        ~TriggerRectangleComponent() override
        {
            stopTimer();
        }
        

        void AddRectangle(TriggerRectangle rect) { triggerRectangles.emplace_back(rect); }
        void ClearRectangles() { triggerRectangles.clear(); }
        
    private:
        void timerCallback() override;
        void paint(juce::Graphics& g) override;
        
        std::vector<TriggerRectangle> triggerRectangles;
    };
} // namespace ORchestra
