#include "TriggerRectangle.h"
#include "Timeline.h"

using namespace ORchestra;

void TriggerRectangleComponent::timerCallback()
{
    for (auto& rect : triggerRectangles)
    {
        rect.alpha -= alphaDecrementPerFrame;
        rect.alpha *= rect.alpha; // squaring for visual fade of alpha
        
        if(rect.alpha < 0.f)
            rect.alpha = 0.f;
    }
    
    repaint(getLocalBounds());
}

void TriggerRectangleComponent::paint(juce::Graphics &g)
{
    for (auto& rect : triggerRectangles)
    {
        const auto color = juce::Colour::fromFloatRGBA(1.f, 1.f, 1.f, rect.alpha);
        g.setColour(color);
        g.drawRoundedRectangle(rect.x, rect.y, drawnStepWidth, drawnStepHeight, roundedCornerSize, 4.f);
    }
}
