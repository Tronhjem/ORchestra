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

class CodeEditorPanel : public juce::Component,
                         private juce::CodeDocument::Listener
{
public:
    CodeEditorPanel(ORchestra::ORchestraCodeEditorChangeListener* changeListener);
    ~CodeEditorPanel() override;

    void resized() override;

    juce::CodeDocument& getCodeDocument() { return mCodeDocument; }

    void loadContent(const juce::String& content);
    bool hasUnsavedChanges() const;
    void markSaved();

    void setEditorLookAndFeel(juce::LookAndFeel* laf);
    void applyDefaultStyling();

private:
    void codeDocumentTextInserted (const juce::String&, int) override { mTokeniser.syncWithDocument (mCodeDocument); }
    void codeDocumentTextDeleted (int, int) override                  { mTokeniser.syncWithDocument (mCodeDocument); }

    ORchestra::ORchestraCodeEditorTokenizer mTokeniser;
    juce::CodeDocument mCodeDocument;
    ORchestra::ORchestraCodeEditorComponent mCodeEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CodeEditorPanel)
};
