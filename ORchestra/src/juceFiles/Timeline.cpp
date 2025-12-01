#include "Timeline.h"
#include "Defines.h"
#include "SequenceStep.h"
#include "StepData.h"
#include "Utility.h"
#include "Colours.h"
#include <algorithm>

constexpr float trackHeight = 32.f;
constexpr float stepMargin = 2.5f;

// constexpr float indexStartFade = 3.f;

constexpr float quaterStepHeight = trackHeight * 0.25f;
constexpr float stepHeight = trackHeight;
constexpr float stepWidth = (trackHeight * 1.5f);
constexpr float drawnStepHeight = stepHeight - stepMargin;
constexpr float drawnStepWidth = stepWidth - stepMargin;

constexpr float stepY = trackHeight + (trackHeight - stepHeight) / 2.0f;
constexpr float stepX = stepWidth + stepWidth / 2.0f - stepWidth / 2.0f;


void Timeline::timerCallback()
{
#if _DEBUG
    assert(mAudioProcessor != nullptr);
#endif

    const TransportData& transportData = mAudioProcessor->GetTransportData();
    const int64_t timeInSamples = transportData.timeInSamples;
    const double samplesPerStep = static_cast<double>(transportData.sampleRate) * (60.0 / (transportData.bpm * transportData.bpmDivision));
    const int currentStep = static_cast<int>(ceil(static_cast<double>(timeInSamples) / samplesPerStep));

    //  const double pixelPerSample = dotSize / samplesPerStep;
    //  const int64_t samplesSinceLast = timeInSamples - mLastTimeInSamples;
    //  const double deltaPixels = samplesSinceLast * pixelPerSample;
    //  mStepXPositions[step][i] = (mStepXPositions[step][i] - x) + deltaPixels;
    
    if (currentStep == mLastGlobalStep)
    {
        return;
    }

    mLastGlobalStep = currentStep;
    mLastTimeInSamples = timeInSamples;

    repaint();
}

void Timeline::paint(juce::Graphics& g)
{
    // We start behind the global step, as it's always one ahead and we
    // want to paint the current step being triggered.
    const int globalStepOffset = mLastGlobalStep - 1 + STEP_BUFFER_SIZE;
    const float alpha = 1.f;


    //===========================================================================================================
    // Here we gather the unique pitches
    std::vector<DataUnit> uniqueNoteValues;
    uniqueNoteValues.reserve(8);

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
                for(const DataUnit uniqueNote : uniqueNoteValues)
                {
                    if (uniqueNote == noteValue)
                    {
                        isUnique = false;
                        break;
                    }
                }
                
                if (isUnique)
                    uniqueNoteValues.emplace_back(noteValue);
            }
        }
    }

    std::sort(uniqueNoteValues.begin(), uniqueNoteValues.end(), [](DataUnit a, DataUnit b) { return a > b; } );

    //===========================================================================================================


    for (int index = 0; index < TIMELINE_STEPS_DRAWN; ++index)
    {
//          ============================================================================================================
//          DISABLING FADE FOR NOW
//          ============================================================================================================
//          Calculate alpha values for fadeing steps.
//          if (static_cast<float>(step) >= TIMELINE_STEPS_DRAWN - indexStartFade)
//          {
//              float alphaValue = 1.f - (static_cast<float>(step - TIMELINE_STEPS_DRAWN) + indexStartFade) / indexStartFade;
//              alpha = alphaValue * alphaValue;
//          }
//          ============================================================================================================
//
        const int stepWrapped = (globalStepOffset + index) % STEP_BUFFER_SIZE;
        const std::vector<SequenceStep>& sequenceSteps = mAudioProcessor->GetStepData()[static_cast<unsigned long>(stepWrapped)];

        for (const auto& step : sequenceSteps)
        {
            const juce::Colour colorToSet = GetStepColor(step);
            g.setColour(colorToSet.withAlpha(alpha));

            const DataUnit noteValue = step.mFirst.GetValue(0);
            int yIndex = 0;
            for (const auto uniqueNoteValue : uniqueNoteValues)
            {
                if (uniqueNoteValue == noteValue)
                    break;

                ++yIndex;
            }

            const float y = static_cast<float>(yIndex) * stepY;
            const float x = static_cast<float>(index) * stepX;

            g.fillRect(x, y, drawnStepWidth, drawnStepHeight);

            if (index == 0 && step.mShouldTrigger.GetValue(0))
            {
                g.setColour(juce::Colours::black);
                g.drawRect(x, y, drawnStepWidth, drawnStepHeight, 2.f);
            }

//            =================================================================================
//            DISABLE TEXT FOR NOW
//            =================================================================================
           g.setColour(juce::Colours::black);
           const std::string noteValueString { std::to_string(static_cast<int>(noteValue))};
           
           g.drawText(noteValueString, static_cast<int>(x),
                      static_cast<int>(y + quaterStepHeight),
                      static_cast<int>(stepWidth), 15, juce::Justification::centred);

           // =================================================================================
//
//      scan all sequence steps
//      Get all unique note values
//      Sort 
//      decide y position based on values
//      loop through tracks and draw accoringly
//
//        67    []    []    []    [] []
//        64    [] [] [] [] [] [] [] []
//        56       []    []    []    []
//
//        67    []    []    []    [] []
//        64    [] [] [] [] [] [] [] []
//        62    []          []      
//        56       []    []    []    []
//
        }
    }
}

juce::Colour Timeline::GetStepColor(const SequenceStep& sequenceStep)
{
    if (sequenceStep.mShouldTrigger.GetValue(0) > 0)
    {
        const float velocityFloat = static_cast<float>(sequenceStep.mSecond.GetValue(0)) / 127.f;
        return smoothstepColour(ORchestraColours::MinVelocity,
                                ORchestraColours::MaxVelocity, velocityFloat);
    }
    else
    {
        return ORchestraColours::InactiveStep;
    }
}
