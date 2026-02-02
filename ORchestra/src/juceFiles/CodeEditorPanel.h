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
#include "ORchestraCodeEditorComponent.h"
#include "ORchestraCodeEditorTokenizer.h"
#include "ErrorReporting.h"

class CodeEditorPanel : public juce::Component
{
public:
    CodeEditorPanel(ORchestra::ORchestraCodeEditorChangeListener* changeListener);
    ~CodeEditorPanel() override;

    void resized() override;

    juce::CodeDocument& getCodeDocument() { return mCodeDocument; }
    
    void updateErrorDisplay(const std::vector<ORchestra::LogEntry>& errors);
    
    void loadContent(const juce::String& content);
    bool hasUnsavedChanges() const;
    void markSaved();
    
    void setEditorLookAndFeel(juce::LookAndFeel* laf);
    void setErrorBoxLookAndFeel(juce::LookAndFeel* laf);
    void applyDefaultStyling();

    int getPreferredHeight() const { return 330; } // 300 for editor + 30 for error box

private:
    ORchestra::ORchestraCodeEditorTokenizer mTokeniser;
    juce::CodeDocument mCodeDocument;
    ORchestra::ORchestraCodeEditorComponent mCodeEditor;
    juce::TextEditor mErrorTextBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CodeEditorPanel)
};
