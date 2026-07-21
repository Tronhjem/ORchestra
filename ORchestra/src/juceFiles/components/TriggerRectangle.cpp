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

void TriggerRectangleComponent::Update()
{
#if defined(_DEBUG)
    assert(mAudioProcessor != nullptr);
#endif

    const float bpmFromScript = static_cast<float>(mAudioProcessor->GetUiBpmFromScript());
    const float bpmDivision = mAudioProcessor->GetUiBpmDivision();

    const float stepDurationInMiliSeconds = 60000.f / (bpmFromScript * bpmDivision);
    
    for (int i = static_cast<int>(mRectangles.size()) - 1; i >= 0; --i)
    {
        TimelineRectangle& rect = mRectangles[static_cast<size_t>(i)];

        const float framesPerStep = stepDurationInMiliSeconds * rect.initialDurationInSteps * miliesecondsPerFrameInverse;
        const float alphaDecrementPerFrame = 1.f / framesPerStep;

        rect.width = std::max(0.f, STEP_WIDTH * rect.durationInSteps - STEP_MARGIN);
        rect.value -= alphaDecrementPerFrame;
    
        if(rect.value <= 0.f)
        {
            mRectangles[static_cast<size_t>(i)] = mRectangles.back();
            mRectangles.pop_back();
        }
    }
    
    repaint(getLocalBounds());
}

void TriggerRectangleComponent::IncrementStep()
{
    for (TimelineRectangle& rect : mRectangles)
        rect.durationInSteps -= 1.f;
}

void TriggerRectangleComponent::paint(juce::Graphics& g)
{
    for (auto& rect : mRectangles)
    {
        const float v = std::max(0.f, rect.value);
        const float alpha = 1.f - (1.f - v) * (1.f - v);

        const auto color = juce::Colour::fromFloatRGBA(1.f, 1.f, 1.f, alpha);
        g.setColour(color);

        constexpr float triggerRectLineThickness = 4.f;
        g.drawRoundedRectangle(rect.x, rect.y, rect.width, DRAWN_STEP_HEIGHT, ROUNDED_CORNER_SIZE, triggerRectLineThickness);
    }
}
