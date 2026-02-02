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

#include "Colors.h"
#include "Utility.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_gui_extra/juce_gui_extra.h"

constexpr int WINDOW_WIDTH = 1000;
constexpr int WINDOW_HEIGHT = 800;
constexpr int COMPONENT_MARGIN = 15;
constexpr int OUTER_MARGIN = 20;
constexpr int ROW_SPACING = 10;

//==============================================================================
ORchestraAudioProcessorEditor::ORchestraAudioProcessorEditor(ORchestraAudioProcessor& p)
        : AudioProcessorEditor(&p),
          audioProcessor(p),
          mTransportControls(p.GetValueTree()),
          mTempoControlsPanel(p.GetValueTree()),
          mCodeEditorPanel(this),
          mTimeline(mTriggerRectangle)
{
    setSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    audioProcessor.addChangeListener(this);

    mGeneralLookAndFeel = std::make_unique<GeneralLookAndFeel>();
    mButtonLookAndFeel = std::make_unique<ButtonLookAndFeel>();
    mTextEditorLookAndFeel = std::make_unique<TextEditorLookAndFeel>();

    int xPos = OUTER_MARGIN;
    int yPos = OUTER_MARGIN;
    
    // ==============================================================================
    // Row 1: button controls
    const int transportWidth = mTransportControls.getPreferredWidth();
    const int transportHeight = mTransportControls.getPreferredHeight();
    mTransportControls.setBounds(xPos, yPos, transportWidth, transportHeight);
    
    xPos += transportWidth + COMPONENT_MARGIN;
    const int tempoWidth = mTempoControlsPanel.getPreferredWidth();
    const int tempoHeight = mTempoControlsPanel.getPreferredHeight();
    mTempoControlsPanel.setBounds(xPos, yPos, tempoWidth, tempoHeight);
    
    xPos += tempoWidth + COMPONENT_MARGIN;
    const int fileOpsWidth = mFileOperationsToolbar.getPreferredWidth();
    const int fileOpsHeight = mFileOperationsToolbar.getPreferredHeight();
    mFileOperationsToolbar.setBounds(xPos, yPos + ROW_SPACING, fileOpsWidth, fileOpsHeight);
    
    // ==============================================================================
    // Row 2: CodeEditorPrnel
    yPos += std::max({tempoHeight, transportHeight, fileOpsHeight + ROW_SPACING}) + COMPONENT_MARGIN;
    int codeEditorWidth = WINDOW_WIDTH - 2 * OUTER_MARGIN;
    int codeEditorHeight = mCodeEditorPanel.getPreferredHeight();
    mCodeEditorPanel.setBounds(OUTER_MARGIN, yPos, codeEditorWidth, codeEditorHeight);
    
    // ==============================================================================
    // Row 3: Timeline and TriggerRectangle
    yPos += codeEditorHeight + COMPONENT_MARGIN;
    int timelineHeight = WINDOW_HEIGHT - yPos - OUTER_MARGIN;
    mTimeline.setBounds(OUTER_MARGIN, yPos, WINDOW_WIDTH - OUTER_MARGIN * 2, timelineHeight);
    mTriggerRectangle.setBounds(OUTER_MARGIN, yPos, 100, timelineHeight);

    // ==============================================================================
    
    juce::LookAndFeel::setDefaultLookAndFeel(mGeneralLookAndFeel.get());

    mTransportControls.setButtonLookAndFeel(mButtonLookAndFeel.get());
    mTempoControlsPanel.setGeneralLookAndFeel(mGeneralLookAndFeel.get());
    mFileOperationsToolbar.setButtonLookAndFeel(mButtonLookAndFeel.get());

    mCodeEditorPanel.setEditorLookAndFeel(mTextEditorLookAndFeel.get());
    mCodeEditorPanel.setErrorBoxLookAndFeel(mTextEditorLookAndFeel.get());
    mCodeEditorPanel.applyDefaultStyling();

    mTimeline.SetProcessor(&audioProcessor);
    mTriggerRectangle.SetProcessor(&audioProcessor);

    mTransportControls.setPlayButtonCallback([this]() { handlePlayButton(); });
    mTransportControls.setSyncToggleCallback([this](bool shouldSync) { handleSyncToggle(shouldSync); });

    mFileOperationsToolbar.setImportCallback([this]() { handleImportFile(); });
    mFileOperationsToolbar.setExportCallback([this]() { handleExportFile(); });
    mFileOperationsToolbar.setCompileCallback([this]() { handleCompile(); });

    addAndMakeVisible(mTempoControlsPanel);
    addAndMakeVisible(mTransportControls);
    addAndMakeVisible(mFileOperationsToolbar);
    addAndMakeVisible(mCodeEditorPanel);
    addAndMakeVisible(mTimeline);
    addAndMakeVisible(mTriggerRectangle);

    const std::string& data = audioProcessor.GetInstructionData();
    juce::String dataAsString{ data };
    mCodeEditorPanel.loadContent(dataAsString);

    setWantsKeyboardFocus(true);
}

ORchestraAudioProcessorEditor::~ORchestraAudioProcessorEditor()
{
    audioProcessor.removeChangeListener(this);
}

void ORchestraAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster* broadCaster)
{
    UNUSED(broadCaster);

    const std::string& data = audioProcessor.GetInstructionData();
    const juce::String dataAsString{ data };
    mCodeEditorPanel.loadContent(dataAsString);
}

void ORchestraAudioProcessorEditor::CodeEditorHasChanged()
{
    if(mCodeEditorPanel.hasUnsavedChanges())
    {
        mFileOperationsToolbar.setCompileButtonEnabled(true);
    }
}

void ORchestraAudioProcessorEditor::handlePlayButton()
{
    if (mCodeEditorPanel.hasUnsavedChanges())
        handleCompile();
    
    if (audioProcessor.IsORchestraVMInit())
    {
        audioProcessor.IsRunning = !audioProcessor.IsRunning;
        mTransportControls.updatePlayButtonState(audioProcessor.IsRunning);
    }
}

void ORchestraAudioProcessorEditor::handleSyncToggle(bool shouldSync)
{
    mTransportControls.setPlayButtonEnabled(!shouldSync);
    mTempoControlsPanel.setBpmEnabled(!shouldSync);

    if(shouldSync)
    {
        audioProcessor.IsRunning = false;
        mTransportControls.updatePlayButtonState(false);
    }
}

void ORchestraAudioProcessorEditor::handleCompile()
{
    juce::String text = mCodeEditorPanel.getCodeDocument().getAllContent();
    std::string utf8Text = text.toRawUTF8();
    audioProcessor.Compile(utf8Text);
    if (audioProcessor.IsORchestraVMInit())
    {
        mCodeEditorPanel.markSaved();
        mFileOperationsToolbar.setCompileButtonEnabled(false);
    }
    
    UpdateErrors();
}

void ORchestraAudioProcessorEditor::handleImportFile()
{
    mFileChooser.launchAsync(mFileChooserFlags, [this](const juce::FileChooser& fc)
         {
            UNUSED(fc);
            juce::File file = mFileChooser.getResult();
            std::string filePath{ file.getFullPathName().toRawUTF8() };
            const std::string& data = audioProcessor.ImportFromFile(filePath);
            juce::String dataAsString{ data };
            mCodeEditorPanel.loadContent(dataAsString);

        });

    handleCompile();
    UpdateErrors(); 
}

void ORchestraAudioProcessorEditor::handleExportFile()
{
    mFileChooser.launchAsync(mFileChooserFlags, [this](const juce::FileChooser& fc)
        {
            UNUSED(fc);
            juce::File file = mFileChooser.getResult();
            std::string filePath{ file.getFullPathName().toRawUTF8() };
            audioProcessor.ExportToFile(filePath);
        });

    UpdateErrors();
}

void ORchestraAudioProcessorEditor::UpdateErrors()
{
    const std::vector<LogEntry>& errors = audioProcessor.GetErrors();
    mCodeEditorPanel.updateErrorDisplay(errors);
}

//==============================================================================
void ORchestraAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(BackgroundColor);
    mFileOperationsToolbar.setCompileButtonEnabled(mCodeEditorPanel.hasUnsavedChanges());
}

void ORchestraAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
