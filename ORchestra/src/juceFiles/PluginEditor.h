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

#include "PluginProcessor.h"

#include "Timeline.h"
#include "TriggerRectangle.h"
#include "CodeEditorPanel.h"
#include "FileOperationsToolbar.h"
#include "TransportControls.h"
#include "TempoControlsPanel.h"

#include "GeneralLookAndFeel.h"
#include "ButtonsLookAndFeel.h"
#include "TextEditorLookAndFeel.h"
#include "juce_gui_basics/juce_gui_basics.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
//==============================================================================
/**
*/
class ORchestraAudioProcessorEditor : public juce::AudioProcessorEditor,
    public juce::ChangeListener,
    public ORchestraCodeEditorChangeListener
{
public:
    ORchestraAudioProcessorEditor(ORchestraAudioProcessor&);
    ~ORchestraAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    void CodeEditorHasChanged() override;

private:
    void changeListenerCallback(juce::ChangeBroadcaster* broadCaster) override;

    void handleCompile();
    void handleImportFile();
    void handleExportFile();
    void handlePlayButton();
    void handleSyncToggle(bool shouldSync);

    ORchestraAudioProcessor& audioProcessor;
    inline void UpdateErrors();

    std::unique_ptr<GeneralLookAndFeel> mGeneralLookAndFeel;
    std::unique_ptr<ButtonLookAndFeel> mButtonLookAndFeel;
    std::unique_ptr<TextEditorLookAndFeel> mTextEditorLookAndFeel;

    juce::FileChooser mFileChooser{ "Select a file to load...", juce::File{}, "*.txt" };
    int mFileChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    TransportControls mTransportControls;
    TempoControlsPanel mTempoControlsPanel;
    FileOperationsToolbar mFileOperationsToolbar;
    CodeEditorPanel mCodeEditorPanel;
    TriggerRectangleComponent mTriggerRectangle;
    Timeline mTimeline;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ORchestraAudioProcessorEditor)
};
