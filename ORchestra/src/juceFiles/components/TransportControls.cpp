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

using namespace ORchestra;

TransportControls::TransportControls()
{
    mPlayButton.addListener(this);
    addAndMakeVisible(mPlayButton);
}

TransportControls::~TransportControls()
{
    mPlayButton.setLookAndFeel(nullptr);
}

void TransportControls::resized()
{
    mPlayButton.setBounds(getLocalBounds());
}

void TransportControls::buttonClicked(juce::Button* button)
{
    if (button == &mPlayButton && mPlayButtonCallback)
        mPlayButtonCallback();
}

void TransportControls::setPlayButtonCallback(std::function<void()> callback)
{
    mPlayButtonCallback = std::move(callback);
}

void TransportControls::updatePlayButtonState(bool isPlaying)
{
    mPlayButton.setButtonText(isPlaying ? "Stop" : "Play");
}

void TransportControls::setButtonLookAndFeel(juce::LookAndFeel* lookAndFeel)
{
    mPlayButton.setLookAndFeel(lookAndFeel);
}
