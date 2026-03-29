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
    
    mErrorTextBox.setFont(MONOSPACE_FONT_OPTIONS);
    mErrorTextBox.setColour(juce::TextEditor::textColourId, TextColor);
    mErrorTextBox.setMultiLine(true);
    mErrorTextBox.setEnabled(false);
    
    mClearLogButton.setButtonText("Clear");
    mClearLogButton.onClick = [this]() {
        if (mClearLogCallback)
            mClearLogCallback();
    };
    
    addAndMakeVisible(mFileOperationsToolbar);
    addAndMakeVisible(mCodeEditor);
    addAndMakeVisible(mErrorTextBox);
    addAndMakeVisible(mClearLogButton);
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

void CodeEditorPanel::setClearLogCallback(std::function<void()> callback)
{
    mClearLogCallback = std::move(callback);
}
    
CodeEditorPanel::~CodeEditorPanel()
{
    mCodeEditor.setLookAndFeel(nullptr);
    mErrorTextBox.setLookAndFeel(nullptr);
}

void CodeEditorPanel::resized()
{
    auto bounds = getLocalBounds();
    
    mCodeEditor.setBounds(bounds.removeFromTop(CODE_EDITOR_HEIGHT));

    const int fileOpsWidth = mFileOperationsToolbar.getPreferredWidth();
    const int fileOpsHeight = mFileOperationsToolbar.getPreferredHeight();

    bounds.removeFromTop(20);
    auto buttonRow = bounds.removeFromTop(fileOpsHeight);

    mFileOperationsToolbar.setBounds(buttonRow.removeFromLeft(fileOpsWidth));

    bounds.removeFromTop(20);

    const auto errorBounds = bounds.removeFromTop(ERROR_BOX_HEIGHT);
    mErrorTextBox.setBounds(errorBounds);
    
    bounds.removeFromTop(20);
    const auto clearButtonBounds = bounds.removeFromTop(20).withWidth(60);
    mClearLogButton.setBounds(clearButtonBounds);
    
    repaint();
}

void CodeEditorPanel::updateErrorDisplay(const std::vector<ORchestra::LogEntry>& errors)
{
    juce::String mess;
    // Iterate in reverse order so newest messages appear first
    for (auto it = errors.rbegin(); it != errors.rend(); ++it)
    {
        const auto& entry = *it;
        // Add step count prefix if available (non-zero)
        if (entry.mStepCount > 0)
        {
            mess += "[Step " + juce::String(entry.mStepCount) + "] ";
        }
        mess.append(entry.mMessage.data(), entry.mMessage.size());
        mess.append("\n", 1);
    }
    mErrorTextBox.setText(mess);
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

void CodeEditorPanel::setErrorBoxLookAndFeel(juce::LookAndFeel* laf)
{
    mErrorTextBox.setLookAndFeel(laf);
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
