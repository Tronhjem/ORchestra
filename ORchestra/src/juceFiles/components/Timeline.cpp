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
#if defined(_DEBUG)
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
    // Draw all steps using fixed chromatic rows (C=bottom/row11, B=top/row0)
    //
    constexpr float totalGridHeight = static_cast<float>(TIMELINE_ROWS_DRAWN) * stepHeight;

    const int barStepCount = GetBpmDivisionWrapIndex(transportData.bpmDivision);

    int drawTranspose = 0;
    for (int index = 0; index < TIMELINE_STEPS_DRAWN; ++index)
    {
        const unsigned long stepWrapped =
            static_cast<unsigned long>((globalStepOffset + index) & STEP_BUFFER_SIZE_MASK);

        const std::vector<SequenceStep>& sequenceSteps = mAudioProcessor->GetStepData()[stepWrapped];
        const float xOffset = labelColumnWidth + static_cast<float>(index) * stepWidth + triggerStepMargin;

        if ((globalStepOffset + index) % barStepCount == 0)
        {
            const int musicalStep = mLastGlobalStep - 1 + index;
            const int barNumber = (musicalStep >= 0 ? musicalStep : 0) / barStepCount + 1;
            const float barWidth = static_cast<float>(barStepCount) * stepWidth;
            mBarLines.emplace_back(BarLine{xOffset - QAURTER_BAR_LINE_THICKNESS,
                                           0.f, barHeaderHeight + totalGridHeight,
                                           barNumber, barWidth});
        }

        for (const auto& step : sequenceSteps)
        {
            if (step.mType == SequenceStepType::TRANSPOSE)
            {
                drawTranspose = static_cast<int>(step.mFirst.GetValue(0));
                continue;
            }

            if (step.mType != SequenceStepType::NoteOn && step.mType != SequenceStepType::CC)
                continue;

            const int substepLength = step.mShouldTrigger.GetLength();
            const float subDividedStepWidth = stepWidth / static_cast<float>(substepLength);
            const float subStepDrawnWidth = drawnStepWidth / static_cast<float>(substepLength);

            for (int substepIndex = 0; substepIndex < substepLength; ++substepIndex)
            {
                if (step.mShouldTrigger.GetValue(substepIndex))
                {
                    const float triggerReactX = static_cast<float>(substepIndex) * subDividedStepWidth + xOffset;
                    const int rawNote = static_cast<int>(step.mFirst.GetEquivalentValueAtIndex(substepIndex, substepLength));
                    const int transposedNote = std::clamp(rawNote + drawTranspose, 0, 127);
                    const int pitchClass = transposedNote % 12;
                    const int row = 11 - pitchClass; // B=row0 (top), C=row11 (bottom)
                    const float triggerRectY = barHeaderHeight + static_cast<float>(row) * stepHeight + triggerStepMargin;
                    const float velocityFloat = static_cast<float>(step.mSecond.GetEquivalentValueAtIndex(substepIndex, substepLength));

                    mTimelineTriggerRectangles.emplace_back(TriggerRectangle{triggerReactX, triggerRectY,
                                                                             subStepDrawnWidth, velocityFloat, step.mType});

                    if (index == 0)
                    {
                        mTriggerRectangle.AddRectangle(TriggerRectangle{triggerReactX, triggerRectY,
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
    static const char* pitchNames[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    constexpr float totalGridWidth = static_cast<float>(TIMELINE_STEPS_DRAWN) * stepWidth;
    constexpr float totalGridHeight = static_cast<float>(TIMELINE_ROWS_DRAWN) * stepHeight;
    constexpr float gridTop = barHeaderHeight;
    constexpr float gridBottom = barHeaderHeight + totalGridHeight;

    // Bar header background
    g.setColour(Colour(ColorPalette::Mantle));
    g.fillRect(0.f, 0.f, labelColumnWidth + totalGridWidth, barHeaderHeight);

    // Label column background (grid area only)
    g.fillRect(0.f, gridTop, labelColumnWidth, totalGridHeight);

    // Darker background for sharp note rows (black keys)
    g.setColour(SharpRowColor);
    for (int row = 0; row < TIMELINE_ROWS_DRAWN; ++row)
    {
        const int pitchClass = 11 - row;
        const bool isSharp = (pitchClass == 1 || pitchClass == 3 ||
                              pitchClass == 6 || pitchClass == 8 || pitchClass == 10);
        if (isSharp)
        {
            const float y = gridTop + static_cast<float>(row) * stepHeight;
            g.fillRect(labelColumnWidth, y, totalGridWidth, stepHeight);
        }
    }

    // Horizontal grid lines and pitch labels
    g.setColour(GridLineColor);
    for (int row = 0; row <= TIMELINE_ROWS_DRAWN; ++row)
    {
        const float y = gridTop + static_cast<float>(row) * stepHeight;
        g.drawLine(0.f, y, labelColumnWidth + totalGridWidth, y, 1.f);
    }

    g.setFont(juce::Font(juce::FontOptions{13.f}));
    for (int row = 0; row < TIMELINE_ROWS_DRAWN; ++row)
    {
        const float y = gridTop + static_cast<float>(row) * stepHeight;
        const int pitchClass = 11 - row;
        g.setColour(TextColor);
        g.drawText(pitchNames[pitchClass],
                   8, static_cast<int>(y),
                   static_cast<int>(labelColumnWidth) - 4, static_cast<int>(stepHeight),
                   juce::Justification::centredLeft, false);
    }

    // Vertical column grid lines (grid area only, no header ticks)
    g.setColour(GridLineColor);
    for (int col = 0; col <= TIMELINE_STEPS_DRAWN; ++col)
    {
        const float x = labelColumnWidth + static_cast<float>(col) * stepWidth;
        g.drawLine(x, gridTop, x, gridBottom, 1.f);
    }

    // Bar boxes in header + bar lines in grid + bar numbers
    g.setFont(juce::Font(juce::FontOptions{12.f}));
    for (const auto& barLine : mBarLines)
    {
        // Outlined box for this bar in the header
        // g.setColour(GridLineColor.brighter(0.4f));
        // const float xPos = barLine.x < 0.f ? 0 : barLine.x;
        // g.drawRect(xPos, 0.f, barLine.barWidth, barHeaderHeight, 1.f);

        // Bar number text inside the box
        g.setColour(juce::Colour(ColorPalette::Subtext1));
        g.drawText(juce::String(barLine.barNumber),
                   static_cast<int>(barLine.x) + 8, 0,
                   static_cast<int>(barLine.barWidth) - 4, static_cast<int>(barHeaderHeight),
                   juce::Justification::centredLeft, false);

        // Bar line in the grid
        g.setColour(BarLineColor);
        g.drawLine(barLine.x, barLine.startY, barLine.x, barLine.endY, BAR_LINE_THICKNESS);
    }

    // Solid separator between header and grid (closes the bottom of all bar boxes)
    g.setColour(GridLineColor);
    g.drawLine(labelColumnWidth, gridTop, labelColumnWidth + totalGridWidth, gridTop, OUTLINE_THICKNESS);

    // Note rectangles
    for (const auto& rect : mTimelineTriggerRectangles)
    {
        const juce::Colour colorToSet = GetStepColorFromVelocity(rect.value, rect.midiType);
        g.setColour(colorToSet);
        g.fillRoundedRectangle(rect.x, rect.y, rect.width, drawnStepHeight, ROUNDED_CORNER_SIZE);
    }

    // closing bar header at top.
    g.setColour(ComponentOutlineColor);
    g.drawLine(labelColumnWidth, 0.f, labelColumnWidth, barHeaderHeight, OUTLINE_THICKNESS);

    // Close space to the code editor with line
    g.drawLine(0.f, 0.f, 0.f, (float)getHeight(), VERTICAL_SEPARATOR_THICKNESS);
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

