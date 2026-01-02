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

#include <ctime>

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "LookAndFeelConstants.h"
#include "Colors.h"
#include "ParamConstants.h"
#include "Utility.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_gui_extra/juce_gui_extra.h"

constexpr int WINDOW_WIDTH = 1000;
constexpr int WINDOW_HEIGHT = 800;
constexpr int COMPONENT_MARGIN = 15;
constexpr int OUTER_MARGIN = 20;
const int buttonWidth = 50;
constexpr int buttonHeight = 20;
constexpr int codeEditorWidth = WINDOW_WIDTH - 2 * OUTER_MARGIN;
constexpr int codeEditorHeight = 300;

//==============================================================================
ORchestraAudioProcessorEditor::ORchestraAudioProcessorEditor(ORchestraAudioProcessor& p)
        : AudioProcessorEditor(&p),
          audioProcessor(p),
          mTimeline(mTriggerRectangle),
          mCodeEditor(mCodeDocument, &mTokeniser)
{
    setSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    audioProcessor.addChangeListener(this);
    juce::AudioProcessorValueTreeState& valueTree = audioProcessor.GetValueTree();

    mGeneralLookAndFeel = std::make_unique<GeneralLookAndFeel>();
    mButtonLookAndFeel = std::make_unique<ButtonLookAndFeel>();
    mTextEditorLookAndFeel = std::make_unique<TextEditorLookAndFeel>();

    mCodeEditor.setTabSize(4, true);
    mCodeEditor.setLineNumbersShown(true);

    int buttonXStart = static_cast<int>(OUTER_MARGIN - 10);
    int nextLineY = 20;

    mSyncToggleLabel.setBounds(buttonXStart, nextLineY, static_cast<int>(buttonWidth), buttonHeight);

    buttonXStart += static_cast<int>(buttonWidth * 2.f + COMPONENT_MARGIN);
    mBpmLabel.setBounds(buttonXStart, nextLineY, static_cast<int>(buttonWidth * 1.5f), buttonHeight);

    buttonXStart += static_cast<int>(buttonWidth - 20.f + COMPONENT_MARGIN);
    mTempoDivLabel.setBounds(buttonXStart, nextLineY, static_cast<int>(buttonWidth * 2.f), buttonHeight);

    buttonXStart += static_cast<int>(buttonWidth * 1.8f + COMPONENT_MARGIN);
    mNoteLengthLabel.setBounds(buttonXStart, nextLineY, static_cast<int>(buttonWidth * 1.5f), buttonHeight);

    // ======== NEW LINE ============
    nextLineY += buttonHeight;
    buttonXStart = OUTER_MARGIN;
    mSyncToggleBox.setBounds(buttonXStart, nextLineY, buttonHeight, buttonHeight);

    buttonXStart += static_cast<int>(buttonHeight + COMPONENT_MARGIN);
    mTogglePlayButton.setBounds(buttonXStart, nextLineY, buttonWidth, buttonHeight);

    buttonXStart += static_cast<int>(buttonWidth + COMPONENT_MARGIN);
    mBpmBox.setBounds(buttonXStart, nextLineY, buttonWidth, buttonHeight);

    buttonXStart += static_cast<int>(buttonWidth + COMPONENT_MARGIN);
    mTempoDivisionSelectorBox.setBounds(buttonXStart, nextLineY, static_cast<int>(buttonWidth * 1.5f), buttonHeight);

    buttonXStart += static_cast<int>(buttonWidth * 1.5f + COMPONENT_MARGIN);
    mNoteLengtSelectorBox.setBounds(buttonXStart, nextLineY, static_cast<int>(buttonWidth * 1.5f), buttonHeight);

    buttonXStart += static_cast<int>(buttonWidth * 1.5f + COMPONENT_MARGIN);
    mExportToFileButton.setBounds(buttonXStart, nextLineY, static_cast<int>(buttonWidth * 1.2f), buttonHeight);

    buttonXStart += static_cast<int>(buttonWidth * 1.2f + COMPONENT_MARGIN);
    mImportFileButton.setBounds(buttonXStart, nextLineY, static_cast<int>(buttonWidth * 1.2f), buttonHeight);

    buttonXStart += static_cast<int>(buttonWidth * 1.2f + COMPONENT_MARGIN);
    mCompileButton.setBounds(buttonXStart, nextLineY, static_cast<int>(buttonWidth * 1.5f), buttonHeight);

    // ======== NEW LINE ============
    nextLineY += buttonHeight + COMPONENT_MARGIN;
    mCodeEditor.setBounds(OUTER_MARGIN, nextLineY, codeEditorWidth, codeEditorHeight);

    nextLineY += codeEditorHeight - 2;
    mErrorTextBox.setBounds(OUTER_MARGIN, nextLineY, codeEditorWidth, 30);

    // ======== NEW LINE ============
    nextLineY += 30 + COMPONENT_MARGIN;
    mTimeline.setBounds(OUTER_MARGIN, nextLineY, WINDOW_WIDTH - OUTER_MARGIN * 2, WINDOW_HEIGHT - nextLineY - OUTER_MARGIN);
    mTriggerRectangle.setBounds(OUTER_MARGIN, nextLineY, 100, WINDOW_HEIGHT - nextLineY - OUTER_MARGIN);

    mBpmBox.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
    mBpmBox.setSliderSnapsToMousePosition(false);

    mBpmBox.setColour(Slider::textBoxOutlineColourId, BackgroundColor);

    mSyncToggleLabel.setColour(juce::Label::textColourId, TextColor);
    mTempoDivLabel.setColour(juce::Label::textColourId, TextColor);
    mBpmLabel.setColour(juce::Label::textColourId, TextColor);
    mNoteLengthLabel.setColour(juce::Label::textColourId, TextColor);

    mTempoDivisionSelectorBox.addItemList(mNoteDivisions, 3);
    mNoteLengtSelectorBox.addItemList(mNoteDivisions, 3);

    mExportToFileButton.addListener(this);
    mImportFileButton.addListener(this);
    mCompileButton.addListener(this);

    mTogglePlayButton.addListener(this);
    mSyncToggleBox.addListener(this);
    mCodeEditor.AddChangeListener(this);

    juce::LookAndFeel::setDefaultLookAndFeel(mGeneralLookAndFeel.get());

    mTogglePlayButton.setLookAndFeel(mButtonLookAndFeel.get());
    mExportToFileButton.setLookAndFeel(mButtonLookAndFeel.get());
    mImportFileButton.setLookAndFeel(mButtonLookAndFeel.get());
    mSyncToggleBox.setLookAndFeel(mButtonLookAndFeel.get());
    mCompileButton.setLookAndFeel(mButtonLookAndFeel.get());
    mErrorTextBox.setLookAndFeel(mTextEditorLookAndFeel.get());
    mTempoDivisionSelectorBox.setLookAndFeel(mGeneralLookAndFeel.get());
    mNoteLengtSelectorBox.setLookAndFeel(mGeneralLookAndFeel.get());
    mBpmBox.setLookAndFeel(mGeneralLookAndFeel.get());

    mCodeEditor.setLookAndFeel(mTextEditorLookAndFeel.get());
    
    mCodeEditor.setFont(MONOSPACE_FONT_OPTIONS);
    mCodeEditor.setColour(juce::CodeEditorComponent::ColourIds::backgroundColourId, TextEditorBackgroundColor);
    mCodeEditor.setColour(juce::CodeEditorComponent::ColourIds::lineNumberBackgroundId, TextEditorBackgroundColor);
    mCodeEditor.setColour(juce::CodeEditorComponent::ColourIds::highlightColourId, HighlightColor.withAlpha(0.3f));
    mCodeEditor.setColour(juce::CodeEditorComponent::ColourIds::defaultTextColourId, TextColor);
    mCodeEditor.setColour(juce::CodeEditorComponent::ColourIds::lineNumberTextId, TextColor);
    mCodeEditor.setScrollbarThickness(4);

    mErrorTextBox.setFont(MONOSPACE_FONT_OPTIONS);
    mErrorTextBox.setColour(juce::TextEditor::textColourId, TextColor);
    mErrorTextBox.setMultiLine(true);
    mErrorTextBox.setEnabled(false);

    mTimeline.SetProcessor(&audioProcessor);
    mTriggerRectangle.SetProcessor(&audioProcessor);

    addAndMakeVisible(mSyncToggleLabel);
    addAndMakeVisible(mTempoDivLabel);
    addAndMakeVisible(mBpmLabel);
    addAndMakeVisible(mNoteLengthLabel);
    addAndMakeVisible(mTogglePlayButton);
    addAndMakeVisible(mSyncToggleBox);
    addAndMakeVisible(mExportToFileButton);
    addAndMakeVisible(mImportFileButton);
    addAndMakeVisible(mCompileButton);
    addAndMakeVisible(mTempoDivisionSelectorBox);
    addAndMakeVisible(mNoteLengtSelectorBox);

    addAndMakeVisible(mCodeEditor);

    addAndMakeVisible(mErrorTextBox);
    addAndMakeVisible(mTimeline);
    addAndMakeVisible(mTriggerRectangle);
    addAndMakeVisible(mBpmBox);

    const std::string& data = audioProcessor.GetInstructionData();
    juce::String dataAsString{ data };
    mCodeDocument.insertText(0, dataAsString);

    mBpmSliderAttachment.reset(new SliderAttachment(valueTree, bpmString, mBpmBox));
    mTempoDivisionAttachment.reset(new ComboBoxAttachment(valueTree, tempoDivisionString, mTempoDivisionSelectorBox));
    mNoteLengthAttachment.reset(new ComboBoxAttachment(valueTree, noteLengthString, mNoteLengtSelectorBox));
    mToggleButtonAttachment.reset(new ButtonAttachment(valueTree, syncToggleString, mSyncToggleBox));

    setWantsKeyboardFocus(true);
}

ORchestraAudioProcessorEditor::~ORchestraAudioProcessorEditor()
{
    mTogglePlayButton.setLookAndFeel(nullptr);
    mExportToFileButton.setLookAndFeel(nullptr);
    mImportFileButton.setLookAndFeel(nullptr);
    mSyncToggleBox.setLookAndFeel(nullptr);
    mCodeEditor.setLookAndFeel(nullptr);

    audioProcessor.removeChangeListener(this);
}

void ORchestraAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster* broadCaster)
{
    UNUSED(broadCaster);

    const std::string& data = audioProcessor.GetInstructionData();
    const juce::String dataAsString{ data };
    mCodeDocument.insertText(0, dataAsString);
}

void ORchestraAudioProcessorEditor::CodeEditorHasChanged()
{
    if(mCodeDocument.hasChangedSinceSavePoint())
    {
        mCompileButton.setEnabled(true);
    }
}

void ORchestraAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    auto processorReadAndCompile = [&]()
    {
        juce::String text = mCodeDocument.getAllContent();
        std::string utf8Text = text.toRawUTF8();
        audioProcessor.Compile(utf8Text);
        if (audioProcessor.IsORchestraVMInit())
        {
            mCodeDocument.setSavePoint();
            mCompileButton.setEnabled(false);
        }
        
        UpdateErrors();
    };

    if (button == &mTogglePlayButton)
    {
        if (mCodeDocument.hasChangedSinceSavePoint())
            processorReadAndCompile();
        
        if (audioProcessor.IsORchestraVMInit())
        {
            audioProcessor.IsRunning = !audioProcessor.IsRunning;
            mTogglePlayButton.setButtonText(audioProcessor.IsRunning ? "Stop" : "Play");
        }
    }
    else if (button == &mExportToFileButton)
    {
        // TODO: Should we compile before saving?
        // processorReadAndCompile();
        
        mFileChooser.launchAsync(mFileChooserFlags, [this](const juce::FileChooser& fc)
            {
                UNUSED(fc);
                juce::File file = mFileChooser.getResult();
                std::string filePath{ file.getFullPathName().toRawUTF8() };
                audioProcessor.ExportToFile(filePath);
            });

        UpdateErrors();
    }
    else if (button == &mImportFileButton)
    {
        mFileChooser.launchAsync(mFileChooserFlags, [this](const juce::FileChooser& fc)
             {
                UNUSED(fc);
                juce::File file = mFileChooser.getResult();
                std::string filePath{ file.getFullPathName().toRawUTF8() };
                const std::string& data = audioProcessor.ImportFromFile(filePath);
                juce::String dataAsString{ data };
                mCodeDocument.insertText(0, dataAsString);

            });

        processorReadAndCompile();
        UpdateErrors(); 
    }
    else if (button == &mCompileButton)
    {
        if (mCodeDocument.hasChangedSinceSavePoint())
            processorReadAndCompile();
    }
    else if (button == &mSyncToggleBox)
    {
        // TODO: This logic should probably be somewhere else.
        const bool shouldSync = mSyncToggleBox.getToggleState();
        mTogglePlayButton.setEnabled(!shouldSync);
        mBpmBox.setEnabled(!shouldSync);

        if(shouldSync)
        {
            audioProcessor.IsRunning = false;
            mTogglePlayButton.setButtonText("Play");
        }
    }
}

void ORchestraAudioProcessorEditor::UpdateErrors()
{
    const std::vector<LogEntry>& errors = audioProcessor.GetErrors();
    if (errors.size() > 0)
        mErrorTextBox.setText(errors[0].mMessage);
    else
        mErrorTextBox.setText("Compiled successfully!");
}

//==============================================================================
void ORchestraAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(BackgroundColor);
    mCompileButton.setEnabled(mCodeDocument.hasChangedSinceSavePoint());
}

void ORchestraAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
