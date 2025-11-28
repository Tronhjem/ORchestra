#include "Timeline.h"
#include "Utility.h"
#include "Colours.h"

constexpr float trackHeight = 32.f;
constexpr float stepMargin = 2.5f;

constexpr float stepHeight = trackHeight;
constexpr float quaterStepHeight = trackHeight * 0.25f;
constexpr float stepWidth = (trackHeight * 1.5f);
constexpr float drawnStepHeight = stepHeight - stepMargin;
constexpr float drawnStepWidth = stepWidth - stepMargin;

constexpr float stepY = trackHeight + (trackHeight - stepHeight) / 2.0f;
constexpr float stepX = stepWidth + stepWidth / 2.0f - stepWidth / 2.0f;

constexpr float indexStartFade = 3.f;

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

    for (int step = 0; step < TIMELINE_STEPS_DRAWN; ++step)
    {
        const int stepWrapped = (globalStepOffset + step) % STEP_BUFFER_SIZE;
        std::vector<SequenceStep>& stepDatas = mAudioProcessor->GetStepData()[static_cast<unsigned long>(stepWrapped)];

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

        const int size = static_cast<int>(stepDatas.size());
        for (int i = 0; i < size; ++i)
        {
            const SequenceStep& stepData = stepDatas[static_cast<unsigned long>(i)];

            juce::Colour colorToSet;
            if (stepData.mShouldTrigger.GetValue(0) > 0)
            {
                float t = static_cast<float>(stepData.mSecond.GetValue(0)) / 127.f;
                colorToSet = smoothstepColour(ORchestraColours::MinVelocity,
                    ORchestraColours::MaxVelocity,
                    t);
            }
            else
                colorToSet = ORchestraColours::InactiveStep;

            g.setColour(colorToSet.withAlpha(alpha));

            const float y = static_cast<float>(i) * stepY;
            const float x = static_cast<float>(step) * stepX;

            g.fillRect(x, y, drawnStepWidth, drawnStepHeight);
            if (step == 0 && stepData.mShouldTrigger.GetValue(0))
            {
                g.setColour(juce::Colours::black);
                g.drawRect(x, y, drawnStepWidth, drawnStepHeight, 2.f);
            }

//            =================================================================================
//            DISABLE TEXT FOR NOW
//            =================================================================================
//            g.setColour(juce::Colours::black);
//            std::string first{ std::to_string((int)stepData.mFirst.GetValue(0)) };
//            
//            g.drawText(first, static_cast<int>(x),
//                static_cast<int>(y + quaterStepHeight),
//                static_cast<int>(stepWidth), 15, juce::Justification::centred);
//            =================================================================================
        }
    }
}
