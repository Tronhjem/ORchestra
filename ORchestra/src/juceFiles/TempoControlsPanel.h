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

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

class TempoControlsPanel : public juce::Component
{
public:
    TempoControlsPanel(juce::AudioProcessorValueTreeState& valueTree);
    ~TempoControlsPanel() override = default;

    void resized() override;

    // State management
    void setBpmEnabled(bool enabled);

    // Styling
    void setGeneralLookAndFeel(juce::LookAndFeel* laf);

private:
    juce::Label mTempoDivLabel{ "tempo", "Tempo Division" };
    juce::Label mBpmLabel{ "bpm","BPM" };
    juce::Label mNoteLengthLabel{ "noteLength", "Note Length" };

    juce::StringArray mNoteDivisions{ "1n", "2n","4n", "8n", "16n", "32n", "64n" };
    juce::ComboBox mTempoDivisionSelectorBox;
    juce::ComboBox mNoteLengthSelectorBox;
    juce::Slider mBpmSlider;

    std::unique_ptr<SliderAttachment> mBpmSliderAttachment;
    std::unique_ptr<ComboBoxAttachment> mTempoDivisionAttachment;
    std::unique_ptr<ComboBoxAttachment> mNoteLengthAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TempoControlsPanel)
};
