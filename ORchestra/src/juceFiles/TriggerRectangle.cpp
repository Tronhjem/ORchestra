/*
 * Copyright (C) 2026 Christian Tronhjem
 *
 * This file is part of ORchestra.
 *
 * ORchestra is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ORchestra is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with ORchestra. If not, see <https://www.gnu.org/licenses/>.
 */

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
        rect.value -= alphaDecrementPerFrame;
        
        if(rect.value < 0.f)
            rect.value = 0.f;
    }
    
    repaint(getLocalBounds());
}

void TriggerRectangleComponent::paint(juce::Graphics &g)
{
    for (auto& rect : triggerRectangles)
    {
        const auto color = juce::Colour::fromFloatRGBA(1.f, 1.f, 1.f, rect.value);
        g.setColour(color);
        constexpr float triggerRectLineThickness = 4.f;
        g.drawRoundedRectangle(rect.x, rect.y, rect.width, drawnStepHeight, ROUNDED_CORNER_SIZE, triggerRectLineThickness);
    }
}
