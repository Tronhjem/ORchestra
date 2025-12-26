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
