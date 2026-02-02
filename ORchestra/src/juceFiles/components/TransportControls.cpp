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

#include "TransportControls.h"
#include "Colors.h"
#include "ParamConstants.h"

using namespace ORchestra;

TransportControls::TransportControls(juce::AudioProcessorValueTreeState& valueTree)
{
    mSyncToggleAttachment.reset(new ButtonAttachment(valueTree, syncToggleString, mSyncToggleBox));
    
    mSyncToggleLabel.setColour(juce::Label::textColourId, TextColor);
    
    mPlayButton.addListener(this);
    mSyncToggleBox.addListener(this);
    
    addAndMakeVisible(mSyncToggleLabel);
    addAndMakeVisible(mSyncToggleBox);
    addAndMakeVisible(mPlayButton);
}

TransportControls::~TransportControls()
{
    mPlayButton.setLookAndFeel(nullptr);
    mSyncToggleBox.setLookAndFeel(nullptr);
}

void TransportControls::resized()
{
    auto bounds = getLocalBounds();
    
    auto labelBounds = bounds.removeFromTop(LABEL_HEIGHT);
    mSyncToggleLabel.setBounds(labelBounds.removeFromLeft(BUTTON_WIDTH * 2));
    
    mSyncToggleBox.setBounds(bounds.removeFromLeft(BUTTON_HEIGHT));
    bounds.removeFromLeft(SPACING);
    mPlayButton.setBounds(bounds.removeFromLeft(BUTTON_WIDTH));
}

void TransportControls::buttonClicked(juce::Button* button)
{
    if (button == &mPlayButton && mPlayButtonCallback)
        mPlayButtonCallback();
    else if (button == &mSyncToggleBox && mSyncToggleCallback)
        mSyncToggleCallback(mSyncToggleBox.getToggleState());
}

void TransportControls::setPlayButtonCallback(std::function<void()> callback)
{
    mPlayButtonCallback = std::move(callback);
}

void TransportControls::setSyncToggleCallback(std::function<void(bool)> callback)
{
    mSyncToggleCallback = std::move(callback);
}

void TransportControls::updatePlayButtonState(bool isPlaying)
{
    mPlayButton.setButtonText(isPlaying ? "Stop" : "Play");
}

void TransportControls::setPlayButtonEnabled(bool enabled)
{
    mPlayButton.setEnabled(enabled);
}

void TransportControls::setButtonLookAndFeel(juce::LookAndFeel* lookAndFeel)
{
    mPlayButton.setLookAndFeel(lookAndFeel);
    mSyncToggleBox.setLookAndFeel(lookAndFeel);
}
