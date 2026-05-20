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

#include "Defines.h"
#include "PluginProcessor.h"
#include "TriggerRectangle.h"

using namespace ORchestra;

constexpr int TIMELINE_STEPS_DRAWN = 20;
constexpr int TIMELINE_ROWS_DRAWN = 12;

constexpr float stepHeight = 25.f;
constexpr float stepWidth = 48.f;
constexpr float stepMargin = 2.5f;
constexpr float labelColumnWidth = 32.f;

constexpr float drawnStepHeight = stepHeight - stepMargin;
constexpr float drawnStepWidth = stepWidth - stepMargin;
constexpr float barHeaderHeight = 16.f;

struct BarLine
{
    float x;
    float startY;
    float endY;
    int barNumber;
    float barWidth;
};


class Timeline : public juce::Component, public juce::Timer
{
public:
    Timeline(TriggerRectangleComponent& triggerRectangle) :
                mAudioProcessor(nullptr),
                mLastGlobalStep(-1),
                mTriggerRectangle(triggerRectangle),
                mTimelineDirty(false)
    {
        startTimerHz(40);
        mTimelineTriggerRectangles.reserve(TIMELINE_STEPS_DRAWN * TIMELINE_ROWS_DRAWN);
        mBarLines.reserve(8);
    }

    ~Timeline() override
    {
        stopTimer();
    }

    void SetProcessor(ORchestraAudioProcessor* audioProcessor) { mAudioProcessor = audioProcessor; }

    inline void SetTimelineDirty(const bool isDirty) 
    { 
        mTimelineDirty.store(isDirty, std::memory_order_release); 
    }

    void timerCallback() override;
    void paint(juce::Graphics& g) override;

private:
    inline int GetBpmDivisionWrapIndex(const float bpmDivision);
    inline juce::Colour GetStepColorFromVelocity(const float value, const SequenceStepType MidiType);
    ORchestraAudioProcessor* mAudioProcessor;
    int mLastGlobalStep;
    TriggerRectangleComponent& mTriggerRectangle;
    std::vector<TriggerRectangle> mTimelineTriggerRectangles;
    std::vector<BarLine> mBarLines;
    std::atomic<bool> mTimelineDirty;
};
