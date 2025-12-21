#include <algorithm>

#include "Timeline.h"
#include "Defines.h"
#include "SequenceStep.h"
#include "StepData.h"
#include "Utility.h"
#include "Colors.h"
#include "LookAndFeelConstants.h"

void Timeline::timerCallback()
{
#if _DEBUG
    assert(mAudioProcessor != nullptr);
#endif
    const TransportData& transportData = mAudioProcessor->GetTransportData();
    if (!transportData.isPlaying || !mAudioProcessor->IsORchestraVMInit())
        return;
    
    const int64_t timeInSamples = transportData.timeInSamples;
    const double samplesPerStep = static_cast<double>(transportData.sampleRate) * (60.0 / (transportData.bpm * transportData.bpmDivision));
    const int currentStep = static_cast<int>(ceil(static_cast<double>(timeInSamples) / samplesPerStep));
    
    if (currentStep == mLastGlobalStep)
    {
        return;
    }

    mUniqueNoteValues.clear();
    mLastGlobalStep = currentStep;
    mLastTimeInSamples = timeInSamples;
     // We start behind the global step, as it's always one ahead and we
    // want to paint the current step being triggered.
    const int globalStepOffset = mLastGlobalStep - 1 + STEP_BUFFER_SIZE;
    
    mTriggerRectangle.ClearRectangles();
    mTimelineTriggerRectangles.clear();
    
    //=================================================================================================
    // Here we gather the unique pitches
    for (int index = 0; index < TIMELINE_STEPS_DRAWN; ++index)
    {
        const int stepWrapped = (globalStepOffset + index) % STEP_BUFFER_SIZE;
        const std::vector<SequenceStep>& sequenceSteps = mAudioProcessor->GetStepData()[static_cast<unsigned long>(stepWrapped)];

        for (const auto& step : sequenceSteps)
        {
            //TODO: How should we draw CC?
            // if (step.mType != MidiType::NoteOn)
            //     continue;

            const int substepLength = step.mFirst.GetLength();
            for (int j = 0; j < substepLength; ++j)
            {
                const DataUnit noteValue = step.mFirst.GetValue(j);
                bool isUnique = true;
                for(const DataUnit uniqueNote : mUniqueNoteValues)
                {
                    if (uniqueNote == noteValue)
                    {
                        isUnique = false;
                        break;
                    }
                }
                
                if (isUnique)
                    mUniqueNoteValues.emplace_back(noteValue);
            }
            
        }
    }

    std::sort(mUniqueNoteValues.begin(), mUniqueNoteValues.end(), [](DataUnit a, DataUnit b) { return a > b; } );
    
    for (int index = 0; index < TIMELINE_STEPS_DRAWN; ++index)
    {
        const int stepWrapped = (globalStepOffset + index) % STEP_BUFFER_SIZE;
        const std::vector<SequenceStep>& sequenceSteps = mAudioProcessor->GetStepData()[static_cast<unsigned long>(stepWrapped)];

        for (const auto& step : sequenceSteps)
        {
            const DataUnit noteValue = step.mFirst.GetValue(0);
            int yIndex = 0;
            for (const auto uniqueNoteValue : mUniqueNoteValues)
            {
                if (uniqueNoteValue == noteValue)
                    break;

                ++yIndex;
            }

            const float x = static_cast<float>(index) * stepWidth + 1.f;
            const float y = static_cast<float>(yIndex) * stepHeight + 1.f;
            const float velocityFloat = static_cast<float>(step.mSecond.GetValue(0));

            if(step.mShouldTrigger.GetValue(0))
            {
                TriggerRectangle timelineRect {x, y, velocityFloat};
                mTimelineTriggerRectangles.emplace_back(timelineRect);

                if (index == 0)
                {
                    TriggerRectangle triggerRect {x, y, 1.f};
                    mTriggerRectangle.AddRectangle(triggerRect);
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
        const juce::Colour colorToSet = GetStepColorFromVelocity(rect.alpha);
        g.setColour(colorToSet.withAlpha(1.f));
        g.fillRoundedRectangle(rect.x, rect.y, drawnStepWidth,  drawnStepHeight, ROUNDED_CORNER_SIZE);
    }
}

juce::Colour Timeline::GetStepColorFromVelocity(const float velocity)
{
        return smoothstepColour(MinVelocityColor,
                                MaxVelocityColor, velocity / 127.f);
}
