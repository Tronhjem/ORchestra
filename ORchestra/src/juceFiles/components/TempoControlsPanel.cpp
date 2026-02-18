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

#include "TempoControlsPanel.h"
#include "Colors.h"
#include "ParamConstants.h"
#include "juce_graphics/juce_graphics.h"

using namespace ORchestra;

TempoControlsPanel::TempoControlsPanel(juce::AudioProcessorValueTreeState& valueTree)
{
    // Setup labels
    mTempoDivLabel.setColour(juce::Label::textColourId, TextColor);
    mBpmLabel.setColour(juce::Label::textColourId, TextColor);
    mNoteLengthLabel.setColour(juce::Label::textColourId, TextColor);

    // Setup combo boxes
    mTempoDivisionSelectorBox.addItemList(mNoteDivisions, 3);
    mNoteLengthSelectorBox.addItemList(mNoteDivisions, 3);

    // Setup BPM slider
    mBpmSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    mBpmSlider.setSliderSnapsToMousePosition(false);
    mBpmSlider.setColour(juce::Slider::textBoxOutlineColourId, BackgroundColor);

    // Create ValueTree attachments
    mBpmSliderAttachment.reset(new SliderAttachment(valueTree, bpmString, mBpmSlider));
    mTempoDivisionAttachment.reset(new ComboBoxAttachment(valueTree, tempoDivisionString, mTempoDivisionSelectorBox));
    mNoteLengthAttachment.reset(new ComboBoxAttachment(valueTree, noteLengthString, mNoteLengthSelectorBox));

    addAndMakeVisible(mTempoDivLabel);
    addAndMakeVisible(mBpmLabel);
    addAndMakeVisible(mNoteLengthLabel);
    addAndMakeVisible(mTempoDivisionSelectorBox);
    addAndMakeVisible(mNoteLengthSelectorBox);
    addAndMakeVisible(mBpmSlider);
}

void TempoControlsPanel::resized()
{
    auto bounds = getLocalBounds();
    
    auto labelBounds = bounds.removeFromTop(LABEL_HEIGHT);
    mTempoDivLabel.setBounds(labelBounds.removeFromLeft(COMBO_WIDTH));
    mTempoDivLabel.setJustificationType(juce::Justification::centred);

    labelBounds.removeFromLeft(SPACING);
    mNoteLengthLabel.setBounds(labelBounds.removeFromLeft(COMBO_WIDTH));
    mNoteLengthLabel.setJustificationType(juce::Justification::centred);

    labelBounds.removeFromLeft(SPACING);
    mBpmLabel.setBounds(labelBounds.removeFromLeft(SLIDER_WIDTH));
    mBpmLabel.setJustificationType(juce::Justification::centred);
    
    mTempoDivisionSelectorBox.setBounds(bounds.removeFromLeft(COMBO_WIDTH));
    bounds.removeFromLeft(SPACING);
    mNoteLengthSelectorBox.setBounds(bounds.removeFromLeft(COMBO_WIDTH));
    bounds.removeFromLeft(SPACING);
    mBpmSlider.setBounds(bounds.removeFromLeft(SLIDER_WIDTH));
}

void TempoControlsPanel::setBpmEnabled(bool enabled)
{
    mBpmSlider.setEnabled(enabled);
}

void TempoControlsPanel::setGeneralLookAndFeel(juce::LookAndFeel* laf)
{
    mTempoDivisionSelectorBox.setLookAndFeel(laf);
    mNoteLengthSelectorBox.setLookAndFeel(laf);
    mBpmSlider.setLookAndFeel(laf);
}
