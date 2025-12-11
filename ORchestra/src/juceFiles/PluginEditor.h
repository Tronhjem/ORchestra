/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"
#include "Timeline.h"
#include "TriggerRectangle.h"

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
    public juce::TextEditor::Listener,
    public juce::Button::Listener,
    public juce::ChangeListener
{
public:
    ORchestraAudioProcessorEditor(ORchestraAudioProcessor&);
    ~ORchestraAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    void textEditorTextChanged(juce::TextEditor& editor) override;
    void extracted();
    void buttonClicked(juce::Button* button) override;

private:
    ORchestraAudioProcessor& audioProcessor;
    void changeListenerCallback(juce::ChangeBroadcaster* broadCaster) override;
    inline void UpdateErrors();

    bool mEditorIsDirty = false;
    std::unique_ptr<GeneralLookAndFeel> mGeneralLookAndFeel;
    std::unique_ptr<ButtonLookAndFeel> mButtonLookAndFeel;
    std::unique_ptr<TextEditorLookAndFeel> mTextEditorLookAndFeel;

    juce::TextButton mTogglePlayButton{ "Play" };
    juce::TextButton mImportFileButton{ "Import" };
    juce::TextButton mExportToFileButton{ "Export" };
    juce::TextButton mCompileButton{ "Compile" };

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
    // juce::TextEditor mCodeEditorTextBox;
    juce::TextEditor mErrorTextBox;

    juce::LuaTokeniser mTokeniser;
    juce::CodeDocument mCodeDocument;
    juce::CodeEditorComponent mCodeEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ORchestraAudioProcessorEditor)
};
