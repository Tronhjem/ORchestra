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

constexpr int WINDOW_WIDTH = 1200;
constexpr int WINDOW_HEIGHT = 800;
constexpr int COMPONENT_MARGIN = 15;
constexpr int OUTER_MARGIN = 20;
constexpr int ROW_SPACING = 10;

//==============================================================================
ORchestraAudioProcessorEditor::ORchestraAudioProcessorEditor(ORchestraAudioProcessor& p)
        : AudioProcessorEditor(&p),
          audioProcessor(p),
          mTransportControls(),
          mCodeEditorPanel(this),
          mTimeline(mTriggerRectangle)
{
    setResizable(true, true);
    setSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    audioProcessor.addChangeListener(this);

    mGeneralLookAndFeel = std::make_unique<GeneralLookAndFeel>();
    mButtonLookAndFeel = std::make_unique<ButtonLookAndFeel>();
    mTextEditorLookAndFeel = std::make_unique<TextEditorLookAndFeel>();

    int xPos = 10;
    int yPos = OUTER_MARGIN;
    
    // ==============================================================================
    // Row 1: button controls
    const int transportWidth = mTransportControls.getPreferredWidth();
    const int transportHeight = mTransportControls.getPreferredHeight();
    mTransportControls.setBounds(xPos, yPos, transportWidth, transportHeight);
    
    // ==============================================================================
    // Row 2: CodeEditorPanel
    xPos = OUTER_MARGIN;
    yPos += transportHeight + ROW_SPACING;

    int codeEditorWidth = static_cast<int>(static_cast<float>(WINDOW_WIDTH) / 2.5f + OUTER_MARGIN);
    int codeEditorHeight = mCodeEditorPanel.getPreferredHeight();
    mCodeEditorPanel.setBounds(xPos, yPos, codeEditorWidth, codeEditorHeight);

    xPos += codeEditorWidth + COMPONENT_MARGIN;
    
    const int timelineHeight = WINDOW_HEIGHT - yPos - OUTER_MARGIN;
    mTimeline.setBounds(xPos, yPos, WINDOW_WIDTH - OUTER_MARGIN * 2, timelineHeight);
    mTriggerRectangle.setBounds(xPos, yPos, 100, timelineHeight);

    // ==============================================================================
    
    juce::LookAndFeel::setDefaultLookAndFeel(mGeneralLookAndFeel.get());

    mTransportControls.setButtonLookAndFeel(mButtonLookAndFeel.get());

    mCodeEditorPanel.setEditorLookAndFeel(mTextEditorLookAndFeel.get());
    mCodeEditorPanel.setErrorBoxLookAndFeel(mTextEditorLookAndFeel.get());
    mCodeEditorPanel.setFileOperationButtonsLookAndFeel(mButtonLookAndFeel.get());
    mCodeEditorPanel.applyDefaultStyling();

    mTimeline.SetProcessor(&audioProcessor);
    mTriggerRectangle.SetProcessor(&audioProcessor);

    mTransportControls.setPlayButtonCallback([this]() { handlePlayButton(); });
    mCodeEditorPanel.setCompileCallback([this]() { handleCompile(); });
    mCodeEditorPanel.setExportCallback([this]() { handleExportFile(); });
    mCodeEditorPanel.setImportCallback([this]() { handleImportFile(); });
    mCodeEditorPanel.setClearLogCallback([this]() { handleClearLog(); });
    
    audioProcessor.SetErrorListener(this);

    addAndMakeVisible(mTransportControls);
    addAndMakeVisible(mCodeEditorPanel);
    addAndMakeVisible(mTimeline);
    addAndMakeVisible(mTriggerRectangle);

    const std::string& data = audioProcessor.GetInstructionData();
    juce::String dataAsString{ data };
    mCodeEditorPanel.loadContent(dataAsString);
    
    if (audioProcessor.IsORchestraVMInit())
    {
        mCodeEditorPanel.markSaved();
        mCodeEditorPanel.setCompileButtonEnabled(false);
        UpdateErrors();
    }

    setWantsKeyboardFocus(true);
}

ORchestraAudioProcessorEditor::~ORchestraAudioProcessorEditor()
{
    audioProcessor.removeChangeListener(this);
    audioProcessor.SetErrorListener(nullptr);
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
        mCodeEditorPanel.setCompileButtonEnabled(true);
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


void ORchestraAudioProcessorEditor::handleCompile()
{
    juce::String text = mCodeEditorPanel.getCodeDocument().getAllContent();
    std::string utf8Text = text.toRawUTF8();
    audioProcessor.Compile(utf8Text);
    if (audioProcessor.IsORchestraVMInit())
    {
        mCodeEditorPanel.markSaved();
        mCodeEditorPanel.setCompileButtonEnabled(false);
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
            
            handleCompile();
            UpdateErrors();
        });
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
    mCodeEditorPanel.updateErrorDisplay(audioProcessor.GetErrors());
}

void ORchestraAudioProcessorEditor::OnLogUpdated()
{
    triggerAsyncUpdate();
}

void ORchestraAudioProcessorEditor::handleAsyncUpdate()
{
    UpdateErrors();
}

void ORchestraAudioProcessorEditor::handleClearLog()
{
    audioProcessor.RequestClearErrors();
}

//==============================================================================
void ORchestraAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(BackgroundColor);
    mCodeEditorPanel.setCompileButtonEnabled(mCodeEditorPanel.hasUnsavedChanges());

    // float width = static_cast<float>(mCodeEditorPanel.getWidth() + COMPONENT_MARGIN + 10);
    // g.setColour(juce::Colours::white);
    // g.drawLine(width, 0, width, static_cast<float>(getHeight()), 2.f);
}

void ORchestraAudioProcessorEditor::resized()
{
}
