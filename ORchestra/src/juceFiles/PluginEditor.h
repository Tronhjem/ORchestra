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
#include "PluginProcessor.h"
#include "Timeline.h"
#include "TriggerRectangle.h"
#include "CodeEditorPanel.h"
#include "FileOperationsToolbar.h"

#include "GeneralLookAndFeel.h"
#include "ButtonsLookAndFeel.h"
#include "TextEditorLookAndFeel.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_gui_extra/juce_gui_extra.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
//==============================================================================
/**
*/
class ORchestraAudioProcessorEditor : public juce::AudioProcessorEditor,
    public juce::Button::Listener,
    public juce::ChangeListener,
    public ORchestraCodeEditorChangeListener
{
public:
    ORchestraAudioProcessorEditor(ORchestraAudioProcessor&);
    ~ORchestraAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    void extracted();
    void CodeEditorHasChanged() override;

private:
    void buttonClicked(juce::Button* button) override;
    void changeListenerCallback(juce::ChangeBroadcaster* broadCaster) override;

    void handleCompile();
    void handleImportFile();
    void handleExportFile();

    ORchestraAudioProcessor& audioProcessor;
    inline void UpdateErrors();

    std::unique_ptr<GeneralLookAndFeel> mGeneralLookAndFeel;
    std::unique_ptr<ButtonLookAndFeel> mButtonLookAndFeel;
    std::unique_ptr<TextEditorLookAndFeel> mTextEditorLookAndFeel;

    juce::TextButton mTogglePlayButton{ "Play" };
    FileOperationsToolbar mFileOperationsToolbar;

    juce::Label mSyncToggleLabel{ "sync", "Sync" };
    juce::Label mTempoDivLabel{ "tempo", "Tempo Division" };
    juce::Label mBpmLabel{ "bpm","BPM" };
    juce::Label mNoteLengthLabel{ "noteLength", "Note Length" };

    juce::StringArray mNoteDivisions{ "1n", "2n","4n", "8n", "16n", "32n", "64n" };
    juce::ComboBox mTempoDivisionSelectorBox;
    juce::ComboBox mNoteLengtSelectorBox;
    juce::ToggleButton mSyncToggleBox;
    juce::Slider mBpmBox;

    std::unique_ptr<SliderAttachment> mBpmSliderAttachment;
    std::unique_ptr<ComboBoxAttachment> mTempoDivisionAttachment;
    std::unique_ptr<ComboBoxAttachment> mNoteLengthAttachment;
    std::unique_ptr<ButtonAttachment> mToggleButtonAttachment;

    juce::FileChooser mFileChooser{ "Select a file to load...", juce::File{}, "*.txt" };
    int mFileChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    TriggerRectangleComponent mTriggerRectangle;
    Timeline mTimeline;
    CodeEditorPanel mCodeEditorPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ORchestraAudioProcessorEditor)
};
