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

#include <algorithm>
#include <unordered_map>

#include "Timeline.h"
#include "Defines.h"
#include "SequenceStep.h"
#include "StepData.h"
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

    const int64_t timeInSamples = transportData.timeInSamples;
    const double samplesPerStep = static_cast<double>(transportData.sampleRate) 
                                  * (60.0 / (transportData.bpm * transportData.bpmDivision));

    const int currentStep = static_cast<int>(ceil(static_cast<double>(timeInSamples) / samplesPerStep));
    
    if (currentStep == mLastGlobalStep)
    {
        return;
    }

    mLastGlobalStep = currentStep;
    mLastTimeInSamples = timeInSamples;
    // We start behind the global step, as it's always one ahead and we
    // want to paint the current step being triggered.
    const int globalStepOffset = mLastGlobalStep - 1 + STEP_BUFFER_SIZE;
 
    mTriggerRectangle.ClearRectangles();
    mTimelineTriggerRectangles.clear();
    
    //=================================================================================================
    //
    // Here we gather the unique pitches
    //
    std::set<DataUnit> uniqueNoteValues;

    for (int index = 0; index < TIMELINE_STEPS_DRAWN; ++index)
    {
        const unsigned long stepWrapped = 
                static_cast<unsigned long>((globalStepOffset + index) & STEP_BUFFER_SIZE_MASK);

        const std::vector<SequenceStep>& sequenceSteps = mAudioProcessor->GetStepData()[stepWrapped];

        for (const auto& step : sequenceSteps)
        {
            //TODO: How should we draw CC?
            // if (step.mType != MidiType::NoteOn)
            //     continue;

            const int substepLength = step.mFirst.GetLength();
            for (int j = 0; j < substepLength; ++j)
            {
                uniqueNoteValues.insert(step.mFirst.GetValue(j));
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
    for (int index = 0; index < TIMELINE_STEPS_DRAWN; ++index)
    {
        const unsigned long stepWrapped = 
            static_cast<unsigned long>((globalStepOffset + index) & STEP_BUFFER_SIZE_MASK);

        const std::vector<SequenceStep>& sequenceSteps = mAudioProcessor->GetStepData()[stepWrapped];

        for (const auto& step : sequenceSteps)
        {
            const int substepLength = step.mShouldTrigger.GetLength();
            const float subDividedStepWidth = stepWidth / static_cast<float>(substepLength);
            const float subStepDrawnWidth = drawnStepWidth / static_cast<float>(substepLength);
            
            for (int substepIndex = 0; substepIndex < substepLength; ++substepIndex)
            {
                if(step.mShouldTrigger.GetValue(substepIndex))
                {

                    const float x = (static_cast<float>(index) * stepWidth)
                                    + (static_cast<float>((substepIndex)) * subDividedStepWidth)
                                    + triggerStepMargin;

                    const DataUnit noteValue = 
                            step.mFirst.GetEquivalentValueAtIndex(substepIndex, substepLength);
                    
                    const float y = static_cast<float>(indexMap[noteValue]) * stepHeight + triggerStepMargin;

                    const float velocityFloat =
                            static_cast<float>(step.mSecond.GetEquivalentValueAtIndex(substepIndex, substepLength));

                    TriggerRectangle timelineRect {x, y, subStepDrawnWidth, velocityFloat};
                    mTimelineTriggerRectangles.emplace_back(timelineRect);

                    if (index == 0)
                    {
                        TriggerRectangle triggerRect {x, y, subStepDrawnWidth, 1.f};
                        mTriggerRectangle.AddRectangle(triggerRect);
                    }
                }
            }
        }
    }
    
    repaint(getLocalBounds());
}

void Timeline::paint(juce::Graphics& g)
{
    for (const auto& rect : mTimelineTriggerRectangles)
    {
        const juce::Colour colorToSet = GetStepColorFromVelocity(rect.value);
        g.setColour(colorToSet.withAlpha(1.f));
        g.fillRoundedRectangle(rect.x, rect.y, rect.width,  drawnStepHeight, ROUNDED_CORNER_SIZE);
    }
}

juce::Colour Timeline::GetStepColorFromVelocity(const float velocity)
{
        return smoothstepColour(MinVelocityColor,
                                MaxVelocityColor, velocity / 127.f);
}
