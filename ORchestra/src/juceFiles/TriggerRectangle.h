#pragma once

#include <JuceHeader.h>
#include <vector>

struct TriggerRectangle
{
    float x;
    float y;
    float alpha;
};
    
namespace ORchestra
{
    class TriggerRectangleComponent : public juce::Component, public juce::Timer
    {
    public:
        TriggerRectangleComponent()
        {
            startTimer(60);
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
} // end ORchestra
