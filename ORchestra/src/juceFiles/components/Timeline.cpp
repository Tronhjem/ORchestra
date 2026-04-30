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

#include <unordered_map>

#include "Timeline.h"
#include "Defines.h"
#include "SequenceStep.h"
#include "StepData.h"
#include "TriggerRectangle.h"
#include "Utility.h"
#include "Colors.h"
#include "LookAndFeelConstants.h"

// Used to offset the trigger step so the trigger rect around 
// is shown properly
constexpr float triggerStepMargin = 2.f;

void Timeline::timerCallback()
{
#if _DEBUG
    assert(mAudioProcessor != nullptr);
#endif

    const TransportData& transportData = mAudioProcessor->GetTransportData();
    if (!transportData.isPlaying || !mAudioProcessor->IsORchestraVMInit())
        return;

    const int currentStep = mAudioProcessor->GetGlobalStepCount();

    if (currentStep == mLastGlobalStep)
        return;

    mLastGlobalStep = currentStep;
    // We start behind the global step, as it's always one ahead and we
    // want to paint the current step being triggered.
    const int globalStepOffset = mLastGlobalStep - 1 + STEP_BUFFER_SIZE;
 
    mTriggerRectangle.ClearRectangles();
    mTimelineTriggerRectangles.clear();
    mBarLines.clear();

    //=================================================================================================
    //
    // Here we gather the unique pitches, accounting for transpose offset per step
    //
    std::set<DataUnit> uniqueNoteValues;

    int currentTranspose = 0;
    for (int index = 0; index < TIMELINE_STEPS_DRAWN; ++index)
    {
        const unsigned long stepWrapped =
                static_cast<unsigned long>((globalStepOffset + index) & STEP_BUFFER_SIZE_MASK);

        const std::vector<SequenceStep>& sequenceSteps = mAudioProcessor->GetStepData()[stepWrapped];

        for (const auto& step : sequenceSteps)
        {
            if (step.mType == SequenceStepType::TRANSPOSE)
            {
                currentTranspose = static_cast<int>(step.mFirst.GetValue(0));
                continue;
            }

            if (step.mType != SequenceStepType::NoteOn && step.mType != SequenceStepType::CC)
                continue;

            const int substepLength = step.mFirst.GetLength();
            for (int j = 0; j < substepLength; ++j)
            {
                const int transposed = std::clamp(static_cast<int>(step.mFirst.GetValue(j)) + currentTranspose, 0, 127);
                uniqueNoteValues.insert(static_cast<DataUnit>(transposed));
            }
        }
    }

    //=================================================================================================
    //
    // Create index map for y values
    //
    std::unordered_map<DataUnit, int> indexMap;

    int yIndex = static_cast<int>(uniqueNoteValues.size());
    for (const auto uniqueNoteValue : uniqueNoteValues)
    {
        indexMap[uniqueNoteValue] = yIndex--;
    }

    //=================================================================================================
    //
    //Draw only unique pitches, relative to each other.
    //
    int drawTranspose = 0;
    for (int index = 0; index < TIMELINE_STEPS_DRAWN; ++index)
    {
        const unsigned long stepWrapped = 
            static_cast<unsigned long>((globalStepOffset + index) & STEP_BUFFER_SIZE_MASK);

        const std::vector<SequenceStep>& sequenceSteps = mAudioProcessor->GetStepData()[stepWrapped];
        const float xOffset = static_cast<float>(index) * stepWidth + triggerStepMargin;

        if ((globalStepOffset + index) % GetBpmDivisionWrapIndex(transportData.bpmDivision) == 0)
        {
            mBarLines.emplace_back(BarLine{xOffset - QAURTER_BAR_LINE_THICKNESS, 0, 500.f});
        }

        for (const auto& step : sequenceSteps)
        {
            if (step.mType == SequenceStepType::TRANSPOSE)
            {
                drawTranspose = static_cast<int>(step.mFirst.GetValue(0));
                continue;
            }

            if (step.mType != SequenceStepType::NoteOn && step.mType != SequenceStepType::CC)
            {
                continue;
            }

            const int substepLength = step.mShouldTrigger.GetLength();
            const float subDividedStepWidth = stepWidth / static_cast<float>(substepLength);
            const float subStepDrawnWidth = drawnStepWidth / static_cast<float>(substepLength);

            for (int substepIndex = 0; substepIndex < substepLength; ++substepIndex)
            {
                if(step.mShouldTrigger.GetValue(substepIndex))
                {
                    const float triggerReactX = static_cast<float>((substepIndex)) * subDividedStepWidth + xOffset;
                    const int rawNote = static_cast<int>(step.mFirst.GetEquivalentValueAtIndex(substepIndex, substepLength));
                    const DataUnit transposedNote = static_cast<DataUnit>(std::clamp(rawNote + drawTranspose, 0, 127));
                    const float triggerRectY = static_cast<float>(indexMap[transposedNote]) * stepHeight + triggerStepMargin;
                    const float velocityFloat = static_cast<float>(step.mSecond.GetEquivalentValueAtIndex(substepIndex, substepLength));

                    mTimelineTriggerRectangles.emplace_back(TriggerRectangle {triggerReactX, triggerRectY,
                                                                              subStepDrawnWidth, velocityFloat, step.mType});

                    if (index == 0)
                    {
                        mTriggerRectangle.AddRectangle(TriggerRectangle {triggerReactX, triggerRectY,
                                                                         subStepDrawnWidth, 1.f, step.mType});
                    }
                }
            }
        }
    }
    
    repaint(getLocalBounds());
}


void Timeline::paint(juce::Graphics& g)
{
    g.setColour(BarLineColor);
    for (const auto& barLine : mBarLines)
    {
        g.drawLine(barLine.x, barLine.startY, barLine.x, barLine.endY, BAR_LINE_THICKNESS);
    }

    for (const auto& rect : mTimelineTriggerRectangles)
    {
        const juce::Colour colorToSet = GetStepColorFromVelocity(rect.value, rect.midiType);
        g.setColour(colorToSet);
        g.fillRoundedRectangle(rect.x, rect.y, rect.width, drawnStepHeight, ROUNDED_CORNER_SIZE);
    }

}

juce::Colour Timeline::GetStepColorFromVelocity(const float value, const SequenceStepType midiType)
{
    const Colour& minColor = midiType == SequenceStepType::CC ? MinCCValueColor : MinVelocityColor;
    const Colour& maxColor = midiType == SequenceStepType::CC ? MaxCCValueColor : MaxVelocityColor;

    return smoothstepColour(minColor,
                            maxColor, value / 127.f);
}

int Timeline::GetBpmDivisionWrapIndex(const float bpmDivision)
{
    return static_cast<int>(bpmDivision * 4.f);
}

