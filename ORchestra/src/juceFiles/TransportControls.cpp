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

constexpr int BUTTON_HEIGHT = 20;
constexpr int BUTTON_WIDTH = 50;
constexpr int LABEL_HEIGHT = 20;
constexpr int SPACING = 15;

TransportControls::TransportControls(juce::AudioProcessorValueTreeState& valueTree)
{
    // Setup sync toggle with ValueTree attachment
    mSyncToggleAttachment.reset(new ButtonAttachment(valueTree, syncToggleString, mSyncToggleBox));
    
    // Configure label
    mSyncToggleLabel.setColour(juce::Label::textColourId, TextColor);
    
    // Setup listeners
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
    
    // First row: Sync label
    auto labelBounds = bounds.removeFromTop(LABEL_HEIGHT);
    mSyncToggleLabel.setBounds(labelBounds.removeFromLeft(BUTTON_WIDTH * 2));
    
    // Second row: Sync checkbox + Play button
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

void TransportControls::setBpmControlsEnabled(bool enabled)
{
    // This is a placeholder for enabling/disabling BPM controls
    // The actual BPM slider will be in TempoControlsPanel
}

void TransportControls::setButtonLookAndFeel(juce::LookAndFeel* laf)
{
    mPlayButton.setLookAndFeel(laf);
    mSyncToggleBox.setLookAndFeel(laf);
}
