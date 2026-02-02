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

#include "FileOperationsToolbar.h"

FileOperationsToolbar::FileOperationsToolbar()
{
    mImportButton.addListener(this);
    mExportButton.addListener(this);
    mCompileButton.addListener(this);

    addAndMakeVisible(mExportButton);
    addAndMakeVisible(mImportButton);
    addAndMakeVisible(mCompileButton);
}

FileOperationsToolbar::~FileOperationsToolbar()
{
    mImportButton.setLookAndFeel(nullptr);
    mExportButton.setLookAndFeel(nullptr);
    mCompileButton.setLookAndFeel(nullptr);
}

void FileOperationsToolbar::resized()
{
    auto bounds = getLocalBounds();
    int buttonWidth = (bounds.getWidth() - 2 * BUTTON_SPACING) / 3;

    mExportButton.setBounds(bounds.removeFromLeft(buttonWidth));
    bounds.removeFromLeft(BUTTON_SPACING);

    mImportButton.setBounds(bounds.removeFromLeft(buttonWidth));
    bounds.removeFromLeft(BUTTON_SPACING);

    mCompileButton.setBounds(bounds);
}

void FileOperationsToolbar::buttonClicked(juce::Button* button)
{
    if (button == &mImportButton && mImportCallback)
        mImportCallback();
    else if (button == &mExportButton && mExportCallback)
        mExportCallback();
    else if (button == &mCompileButton && mCompileCallback)
        mCompileCallback();
}

void FileOperationsToolbar::setImportCallback(std::function<void()> callback)
{
    mImportCallback = std::move(callback);
}

void FileOperationsToolbar::setExportCallback(std::function<void()> callback)
{
    mExportCallback = std::move(callback);
}

void FileOperationsToolbar::setCompileCallback(std::function<void()> callback)
{
    mCompileCallback = std::move(callback);
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
}
