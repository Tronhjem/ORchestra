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

#include "CodeEditorPanel.h"
#include "Colors.h"
#include "LookAndFeelConstants.h"
#include "juce_graphics/juce_graphics.h"
#include "juce_gui_basics/juce_gui_basics.h"

using namespace ORchestra;

CodeEditorPanel::CodeEditorPanel(ORchestra::ORchestraCodeEditorChangeListener* changeListener)
    : mCodeEditor(mCodeDocument, &mTokeniser)
{
    mCodeEditor.setTabSize(4, true);
    mCodeEditor.setLineNumbersShown(true);
    mCodeEditor.AddChangeListener(changeListener);

    addAndMakeVisible(mFileOperationsToolbar);
    addAndMakeVisible(mCodeEditor);
}

CodeEditorPanel::~CodeEditorPanel()
{
    mCodeEditor.setLookAndFeel(nullptr);
}

void CodeEditorPanel::resized()
{
    auto bounds = getLocalBounds();

    const int fileOpsHeight = mFileOperationsToolbar.getPreferredHeight();
    const int fileOpsWidth = mFileOperationsToolbar.getPreferredWidth();

    auto toolbarRow = bounds.removeFromBottom(fileOpsHeight);
    mFileOperationsToolbar.setBounds(toolbarRow.removeFromLeft(fileOpsWidth));

    mCodeEditor.setBounds(bounds);
}

void CodeEditorPanel::loadContent(const juce::String& content)
{
    mCodeDocument.insertText(0, content);
}

bool CodeEditorPanel::hasUnsavedChanges() const
{
    return mCodeDocument.hasChangedSinceSavePoint();
}

void CodeEditorPanel::markSaved()
{
    mCodeDocument.setSavePoint();
}

void CodeEditorPanel::setEditorLookAndFeel(juce::LookAndFeel* laf)
{
    mCodeEditor.setLookAndFeel(laf);
}

void CodeEditorPanel::setFileOperationButtonsLookAndFeel(juce::LookAndFeel* laf)
{
    mFileOperationsToolbar.setButtonLookAndFeel(laf);
}

void CodeEditorPanel::setCompileButtonEnabled(bool enabled)
{
    mFileOperationsToolbar.setCompileButtonEnabled(enabled);
}

void CodeEditorPanel::applyDefaultStyling()
{
    mCodeEditor.setFont(MONOSPACE_FONT_OPTIONS);
    mCodeEditor.setColour(juce::CodeEditorComponent::ColourIds::backgroundColourId, TextEditorBackgroundColor);
    mCodeEditor.setColour(juce::CodeEditorComponent::ColourIds::lineNumberBackgroundId, TextEditorBackgroundColor);
    mCodeEditor.setColour(juce::CodeEditorComponent::ColourIds::highlightColourId, HighlightColor.withAlpha(0.3f));
    mCodeEditor.setColour(juce::CodeEditorComponent::ColourIds::defaultTextColourId, TextColor);
    mCodeEditor.setColour(juce::CodeEditorComponent::ColourIds::lineNumberTextId, TextColor);
    mCodeEditor.setScrollbarThickness(4);
}

void CodeEditorPanel::setImportCallback(std::function<void()> callback)
{
    mFileOperationsToolbar.setImportCallback(std::move(callback));
}

void CodeEditorPanel::setExportCallback(std::function<void()> callback)
{
    mFileOperationsToolbar.setExportCallback(std::move(callback));
}

void CodeEditorPanel::setCompileCallback(std::function<void()> callback)
{
    mFileOperationsToolbar.setCompileCallback(std::move(callback));
}

void CodeEditorPanel::setPlayCallback(std::function<void()> callback)
{
    mFileOperationsToolbar.setPlayCallback(std::move(callback));
}

void CodeEditorPanel::updatePlayButtonState(bool isPlaying)
{
    mFileOperationsToolbar.updatePlayButtonState(isPlaying);
}
