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

#include "Colors.h"
#include "LookAndFeelConstants.h"
#include "FileOperationsToolbar.h"

FileOperationsToolbar::FileOperationsToolbar()
{
    mCompileButton.addListener(this);
    mPlayButton.addListener(this);

    addAndMakeVisible(mCompileButton);
    addAndMakeVisible(mPlayButton);
}

FileOperationsToolbar::~FileOperationsToolbar()
{
    mCompileButton.setLookAndFeel(nullptr);
    mPlayButton.setLookAndFeel(nullptr);
}

void FileOperationsToolbar::paint(juce::Graphics& g) 
{
    g.fillAll(ORchestra::TransportButtonPanelBackground);
    g.setColour(ORchestra::TextColor);
    g.drawRect(getLocalBounds().toFloat(), OUTLINE_THICKNESS);
}

void FileOperationsToolbar::resized()
{
    const int padding = (getHeight() - BUTTON_HEIGHT) / 2;
    auto row = getLocalBounds().reduced(0, padding);

    mCompileButton.setBounds(row.removeFromLeft(BUTTON_WIDTH));
    row.removeFromLeft(BUTTON_SPACING);
    mPlayButton.setBounds(row.removeFromLeft(BUTTON_WIDTH));
}

void FileOperationsToolbar::buttonClicked(juce::Button* button)
{
    if (button == &mCompileButton && mCompileCallback)
        mCompileCallback();
    else if (button == &mPlayButton && mPlayCallback)
        mPlayCallback();
}

void FileOperationsToolbar::updatePlayButtonState(bool isPlaying)
{
    mPlayButton.setButtonText(isPlaying ? "> Stop" : "> Play");
}

void FileOperationsToolbar::setCompileCallback(std::function<void()> callback)
{
    mCompileCallback = std::move(callback);
}

void FileOperationsToolbar::setPlayCallback(std::function<void()> callback)
{
    mPlayCallback = std::move(callback);
}

void FileOperationsToolbar::setCompileButtonEnabled(bool enabled)
{
    mCompileButton.setEnabled(enabled);
}

void FileOperationsToolbar::setButtonLookAndFeel(juce::LookAndFeel* laf)
{
    mImportButton.setLookAndFeel(laf);
    mExportButton.setLookAndFeel(laf);
    mCompileButton.setLookAndFeel(laf);
    mPlayButton.setLookAndFeel(laf);
}
