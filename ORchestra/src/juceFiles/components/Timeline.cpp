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
#include "NoteDivision.h"
#include "Colors.h"
#include "LookAndFeelConstants.h"

// Used to offset the trigger step so the trigger rect around
// is shown properly
constexpr float TRIGGER_STEP_MARGIN = 2.f;

void Timeline::timerCallback()
{

#if defined(_DEBUG)
    assert(mAudioProcessor != nullptr);
#endif

    if (!mAudioProcessor->IsORchestraVMInit())
        return;

    mTriggerRectangle.Update(); 

    const int currentStep = mAudioProcessor->GetGlobalStepCount();

    if (currentStep == mLastGlobalStep && !mTimelineDirty.load())
        return;

    mLastGlobalStep = currentStep;
    SetTimelineDirty(false);

    // We start behind the global step, as it's always one ahead and we
    // want to paint the current step being triggered.
    const int globalStepOffset = mLastGlobalStep - 1 + STEP_BUFFER_SIZE;
 
    mTimelineRectangles.clear();
    mBarLines.clear();

    // Draw all steps using fixed chromatic rows (C=bottom/row11, B=top/row0)
    constexpr float totalGridHeight = static_cast<float>(TIMELINE_ROWS_DRAWN) * STEP_HEIGHT;

    const float bpmDivision = mAudioProcessor->GetTransportData().bpmDivision;
    const int barStepCount = GetBpmDivisionWrapIndex(bpmDivision);

    mTriggerRectangle.IncrementStep();

    for(int i = static_cast<int>(mPlayingTimelineRectangles.size()) - 1; i >= 0; --i)
    {
        TimelineRectangle& rect = mPlayingTimelineRectangles[static_cast<size_t>(i)];
        rect.durationInSteps -= 1.f;
        rect.width = std::max(0.f, STEP_WIDTH * rect.durationInSteps - STEP_MARGIN);

        if (rect.durationInSteps <= 0.f)
        {
            mPlayingTimelineRectangles[static_cast<size_t>(i)] = mPlayingTimelineRectangles.back();
            mPlayingTimelineRectangles.pop_back();
        }
    }
    
    int drawTranspose = 0;
    for (int index = 0; index < TIMELINE_STEPS_DRAWN; ++index)
    {
        const unsigned long stepWrapped =
            static_cast<unsigned long>((globalStepOffset + index) & STEP_BUFFER_SIZE_MASK);

        // Copy under the slot's lock: the worker thread clears/refills slots.
        const std::vector<SequenceStep> sequenceSteps = mAudioProcessor->GetStepDataSlotCopy(stepWrapped);
        const float xOffset = LABEL_COLUMN_WIDTH + static_cast<float>(index) * STEP_WIDTH + TRIGGER_STEP_MARGIN;

        if ((globalStepOffset + index) % barStepCount == 0)
        {
            const int musicalStep = mLastGlobalStep - 1 + index;
            const int barNumber = (musicalStep >= 0 ? musicalStep : 0) / barStepCount + 1;
            const float barWidth = static_cast<float>(barStepCount) * STEP_WIDTH;
            mBarLines.emplace_back(BarLine {xOffset - QAURTER_BAR_LINE_THICKNESS,
                                           0.f, BAR_HEADER_HEIGHT + totalGridHeight,
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
            const float subDividedStepWidth = STEP_WIDTH / static_cast<float>(substepLength);

            for (int substepIndex = 0; substepIndex < substepLength; ++substepIndex)
            {
                if (!step.mShouldTrigger.GetValue(substepIndex))
                    continue;

                const float triggerReactX = static_cast<float>(substepIndex) * subDividedStepWidth + xOffset;
                const int rawNote = static_cast<int>(step.mFirst.GetEquivalentValueAtIndex(substepIndex, substepLength));
                const int transposedNote = std::clamp(rawNote + drawTranspose, 0, 127);

                const int row = 11 - (transposedNote % 12); // B=row0 (top), C=row11 (bottom)
                const float triggerRectY = BAR_HEADER_HEIGHT + static_cast<float>(row) * STEP_HEIGHT + TRIGGER_STEP_MARGIN;

                const float noteDiv = DurationToBpmDivision(step.mDuration);
                const float durationInSteps = bpmDivision / noteDiv;
                float noteWidth = std::max(2.f, STEP_WIDTH * durationInSteps - STEP_MARGIN);
                const float maxRightEdge = LABEL_COLUMN_WIDTH + static_cast<float>(TIMELINE_STEPS_DRAWN) * STEP_WIDTH;
                const float rightEdge = triggerReactX + noteWidth;

                if (rightEdge > maxRightEdge)
                    noteWidth = maxRightEdge - triggerReactX;

                const float velocityFloat = static_cast<float>(step.mSecond.GetEquivalentValueAtIndex(substepIndex, substepLength));

                if (index == 0)
                {
                    mTriggerRectangle.AddRectangle(TimelineRectangle{triggerReactX, triggerRectY,
                                                                     noteWidth, 1.f, durationInSteps, durationInSteps, step.mType});

                    mPlayingTimelineRectangles.emplace_back(TimelineRectangle{triggerReactX, triggerRectY,
                                                                       noteWidth, velocityFloat, durationInSteps, durationInSteps, step.mType});
                }
                else 
                    mTimelineRectangles.emplace_back(TimelineRectangle{triggerReactX, triggerRectY,
                                                                       noteWidth, velocityFloat, durationInSteps, durationInSteps, step.mType});
            }
        }
    }

    mTriggerRectangle.Update(); 
    repaint(getLocalBounds());
}


void Timeline::paint(juce::Graphics& g)
{
    static const char* pitchNames[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    constexpr float totalGridWidth = static_cast<float>(TIMELINE_STEPS_DRAWN) * STEP_WIDTH;
    constexpr float totalGridHeight = static_cast<float>(TIMELINE_ROWS_DRAWN) * STEP_HEIGHT;
    constexpr float gridTop = BAR_HEADER_HEIGHT;
    constexpr float gridBottom = BAR_HEADER_HEIGHT + totalGridHeight;

    // Bar header background
    g.setColour(Colour(ColorPalette::Mantle));
    g.fillRect(0.f, 0.f, LABEL_COLUMN_WIDTH + totalGridWidth, BAR_HEADER_HEIGHT);

    // Label column background (grid area only)
    g.fillRect(0.f, gridTop, LABEL_COLUMN_WIDTH, totalGridHeight);

    // Darker background for sharp note rows (black keys)
    g.setColour(SharpRowColor);
    for (int row = 0; row < TIMELINE_ROWS_DRAWN; ++row)
    {
        const int pitchClass = 11 - row;
        const bool isSharp = (pitchClass == 1 || pitchClass == 3 ||
                              pitchClass == 6 || pitchClass == 8 || pitchClass == 10);
        if (isSharp)
        {
            const float y = gridTop + static_cast<float>(row) * STEP_HEIGHT;
            g.fillRect(LABEL_COLUMN_WIDTH, y, totalGridWidth, STEP_HEIGHT);
        }
    }

    // Horizontal grid lines and pitch labels
    g.setColour(GridLineColor);
    for (int row = 0; row <= TIMELINE_ROWS_DRAWN; ++row)
    {
        const float y = gridTop + static_cast<float>(row) * STEP_HEIGHT;
        g.drawLine(0.f, y, LABEL_COLUMN_WIDTH + totalGridWidth, y, 1.f);
    }

    g.setFont(juce::Font(BUTTON_FONT_OPTIONS));
    for (int row = 0; row < TIMELINE_ROWS_DRAWN; ++row)
    {
        const float y = gridTop + static_cast<float>(row) * STEP_HEIGHT;
        const int pitchClass = 11 - row;
        g.setColour(TextColor);
        g.drawText(pitchNames[pitchClass],
                   8, static_cast<int>(y),
                   static_cast<int>(LABEL_COLUMN_WIDTH) - 4, static_cast<int>(STEP_HEIGHT),
                   juce::Justification::centredLeft, false);
    }

    // Vertical column grid lines (grid area only, no header ticks)
    g.setColour(GridLineColor);
    for (int col = 0; col <= TIMELINE_STEPS_DRAWN; ++col)
    {
        const float x = LABEL_COLUMN_WIDTH + static_cast<float>(col) * STEP_WIDTH;
        g.drawLine(x, gridTop, x, gridBottom, 1.f);
    }

    // Bar boxes in header + bar lines in grid + bar numbers
    g.setFont(juce::Font(juce::FontOptions{12.f}));
    for (const auto& barLine : mBarLines)
    {
        // Bar number text inside the box
        g.setColour(juce::Colour(ColorPalette::Subtext1));
        g.drawText(juce::String(barLine.barNumber),
                   static_cast<int>(barLine.x) + 8, 0,
                   static_cast<int>(barLine.barWidth) - 4, static_cast<int>(BAR_HEADER_HEIGHT),
                   juce::Justification::centredLeft, false);

        // Bar line in the grid
        g.setColour(BarLineColor);
        g.drawLine(barLine.x, barLine.startY, barLine.x, barLine.endY, BAR_LINE_THICKNESS);
    }

    // Solid separator between header and grid (closes the bottom of all bar boxes)
    g.setColour(GridLineColor);
    g.drawLine(LABEL_COLUMN_WIDTH, gridTop, LABEL_COLUMN_WIDTH + totalGridWidth, gridTop, OUTLINE_THICKNESS);

    // Note rectangles
    for (const auto& rect : mTimelineRectangles)
    {
        const juce::Colour colorToSet = GetStepColorFromVelocity(rect.value, rect.midiType);
        g.setColour(colorToSet);
        g.fillRoundedRectangle(rect.x, rect.y, rect.width, DRAWN_STEP_HEIGHT, ROUNDED_CORNER_SIZE);

        g.setColour(GridLineColor);
        g.drawRoundedRectangle(rect.x, rect.y, rect.width, DRAWN_STEP_HEIGHT, ROUNDED_CORNER_SIZE, 2.f);
    }

    // Playing rectacngles for keeping them alive while still playing
    for (const auto& rect : mPlayingTimelineRectangles)
    {
        const juce::Colour colorToSet = GetStepColorFromVelocity(rect.value, rect.midiType);
        g.setColour(colorToSet);
        g.fillRoundedRectangle(rect.x, rect.y, rect.width, DRAWN_STEP_HEIGHT, ROUNDED_CORNER_SIZE);

        g.setColour(GridLineColor);
        g.drawRoundedRectangle(rect.x, rect.y, rect.width, DRAWN_STEP_HEIGHT, ROUNDED_CORNER_SIZE, 2.f);
    }

    // closing bar header at top.
    g.setColour(ComponentOutlineColor);
    g.drawLine(LABEL_COLUMN_WIDTH, 0.f, LABEL_COLUMN_WIDTH, BAR_HEADER_HEIGHT, OUTLINE_THICKNESS);

    // Close space to the code editor with line
    g.drawLine(0.f, 0.f, 0.f, (float)getHeight(), VERTICAL_SEPARATOR_THICKNESS);
}

juce::Colour Timeline::GetStepColorFromVelocity(const float value, const SequenceStepType midiType)
{
    const Colour& minColor = midiType == SequenceStepType::CC ? MinCCValueColor : MinVelocityColor;
    const Colour& maxColor = midiType == SequenceStepType::CC ? MaxCCValueColor : MaxVelocityColor;

    return smoothstepColour(minColor, maxColor, value / 127.f);
}

int Timeline::GetBpmDivisionWrapIndex(const float bpmDivision)
{
    return static_cast<int>(bpmDivision * 4.f);
}

