#include "TriggerRectangle.h"
#include "Timeline.h"
#include "LookAndFeelConstants.h" 

using namespace ORchestra;

void TriggerRectangleComponent::timerCallback()
{
#if _DEBUG
    assert(mAudioProcessor != nullptr);
#endif

    const TransportData& transportData = mAudioProcessor->GetTransportData();
    if (!transportData.isPlaying || !mAudioProcessor->IsORchestraVMInit())
        return;

    
    const float stepDurationInMiliSeconds = 60000.f / static_cast<float>(transportData.bpm * transportData.bpmDivision);
    const float framesPerStep = stepDurationInMiliSeconds * miliesecondsPerFrameInverse;
    const float alphaDecrementPerFrame = 1.f / framesPerStep;
    
    for (auto& rect : triggerRectangles)
    {
        rect.alpha -= alphaDecrementPerFrame;
        
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
        constexpr float triggerRectLineThickness = 4.f;
        g.drawRoundedRectangle(rect.x, rect.y, drawnStepWidth, drawnStepHeight, ROUNDED_CORNER_SIZE, triggerRectLineThickness);
    }
}
