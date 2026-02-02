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

using namespace ORchestra;

CodeEditorPanel::CodeEditorPanel(ORchestra::ORchestraCodeEditorChangeListener* changeListener)
    : mCodeEditor(mCodeDocument, &mTokeniser)
{
    // Configure code editor
    mCodeEditor.setTabSize(4, true);
    mCodeEditor.setLineNumbersShown(true);
    mCodeEditor.AddChangeListener(changeListener);
    
    // Configure error text box
    mErrorTextBox.setFont(MONOSPACE_FONT_OPTIONS);
    mErrorTextBox.setColour(juce::TextEditor::textColourId, TextColor);
    mErrorTextBox.setMultiLine(true);
    mErrorTextBox.setEnabled(false);
    
    addAndMakeVisible(mCodeEditor);
    addAndMakeVisible(mErrorTextBox);
}

CodeEditorPanel::~CodeEditorPanel()
{
    mCodeEditor.setLookAndFeel(nullptr);
}

void CodeEditorPanel::resized()
{
    auto bounds = getLocalBounds();
    
    // Error box at bottom
    auto errorBounds = bounds.removeFromBottom(ERROR_BOX_HEIGHT);
    mErrorTextBox.setBounds(errorBounds);
    
    // Code editor takes remaining space
    mCodeEditor.setBounds(bounds);
}

void CodeEditorPanel::updateErrorDisplay(const std::vector<ORchestra::LogEntry>& errors)
{
    if (errors.size() > 0)
        mErrorTextBox.setText(errors[0].mMessage);
    else
        mErrorTextBox.setText("Compiled successfully!");
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
