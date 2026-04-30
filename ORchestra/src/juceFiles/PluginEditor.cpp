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

#if JUCE_STANDALONE_APPLICATION
#include "juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h"
#endif

constexpr int WINDOW_WIDTH = 1200;
constexpr int WINDOW_HEIGHT = 800;
constexpr int COMPONENT_MARGIN = 15;
constexpr int OUTER_MARGIN = 20;
constexpr int ROW_SPACING = 10;
constexpr int LOG_BOX_HEIGHT = 180;
constexpr int CLEAR_BUTTON_HEIGHT = 20;
constexpr int SEPARATOR_WIDTH = 2;

//==============================================================================
// MidiSettingsComponent
//==============================================================================
MidiSettingsComponent::MidiSettingsComponent(juce::AudioDeviceManager& deviceManager)
    : mDeviceManager(deviceManager)
{
    mInputLabel.setText("MIDI Inputs", juce::dontSendNotification);
    mInputLabel.setColour(juce::Label::textColourId, ORchestra::TextColor);
    addAndMakeVisible(mInputLabel);

    for (const auto& device : juce::MidiInput::getAvailableDevices())
    {
        auto* btn = mInputButtons.add(std::make_unique<juce::TextButton>(device.name));
        btn->setClickingTogglesState(true);
        btn->setToggleState(mDeviceManager.isMidiInputDeviceEnabled(device.identifier),
                            juce::dontSendNotification);
        btn->setColour(juce::TextButton::textColourOffId, ORchestra::TextColor);
        btn->setColour(juce::TextButton::textColourOnId, ORchestra::BackgroundColor);
        mInputIds.add(device.identifier);
        const int idx = mInputButtons.size() - 1;
        btn->onClick = [this, idx]()
        {
            mDeviceManager.setMidiInputDeviceEnabled(mInputIds[idx],
                                                     mInputButtons[idx]->getToggleState());
        };
        addAndMakeVisible(btn);
    }

    mOutputLabel.setText("MIDI Output", juce::dontSendNotification);
    mOutputLabel.setColour(juce::Label::textColourId, ORchestra::TextColor);
    addAndMakeVisible(mOutputLabel);

    mOutputCombo.addItem("None", 1);
    const auto currentOutputId = mDeviceManager.getDefaultMidiOutputIdentifier();
    int itemId = 2;
    int selectedId = 1;
    for (const auto& device : juce::MidiOutput::getAvailableDevices())
    {
        mOutputCombo.addItem(device.name, itemId);
        mOutputIds.add(device.identifier);
        if (device.identifier == currentOutputId)
            selectedId = itemId;
        ++itemId;
    }
    mOutputCombo.setSelectedId(selectedId, juce::dontSendNotification);
    mOutputCombo.onChange = [this]()
    {
        const int id = mOutputCombo.getSelectedId();
        if (id == 1)
            mDeviceManager.setDefaultMidiOutputDevice({});
        else if (id >= 2 && (id - 2) < mOutputIds.size())
            mDeviceManager.setDefaultMidiOutputDevice(mOutputIds[id - 2]);
    };
    addAndMakeVisible(mOutputCombo);

    const int numInputs = mInputButtons.size();
    const int height = PADDING * 2
                     + LABEL_H + ROW_GAP
                     + numInputs * (ROW_H + ROW_GAP)
                     + SECTION_GAP
                     + LABEL_H + ROW_GAP
                     + ROW_H;
    setSize(PANEL_W, height);
}

MidiSettingsComponent::~MidiSettingsComponent()
{
    for (auto* btn : mInputButtons)
        btn->setLookAndFeel(nullptr);
    mOutputCombo.setLookAndFeel(nullptr);
}

void MidiSettingsComponent::resized()
{
    auto bounds = getLocalBounds().reduced(PADDING);

    mInputLabel.setBounds(bounds.removeFromTop(LABEL_H));
    bounds.removeFromTop(ROW_GAP);

    for (auto* btn : mInputButtons)
    {
        btn->setBounds(bounds.removeFromTop(ROW_H));
        bounds.removeFromTop(ROW_GAP);
    }

    bounds.removeFromTop(SECTION_GAP);
    mOutputLabel.setBounds(bounds.removeFromTop(LABEL_H));
    bounds.removeFromTop(ROW_GAP);
    mOutputCombo.setBounds(bounds.removeFromTop(ROW_H));
}

void MidiSettingsComponent::setButtonLookAndFeel(juce::LookAndFeel* laf)
{
    for (auto* btn : mInputButtons)
        btn->setLookAndFeel(laf);
}

//==============================================================================
ORchestraAudioProcessorEditor::ORchestraAudioProcessorEditor(ORchestraAudioProcessor& p)
        : AudioProcessorEditor(&p),
          audioProcessor(p),
          mCodeEditorPanel(this),
          mTimeline(mTriggerRectangle)
{
    setResizable(true, true);
    setResizeLimits(800, 500, 99999, 99999);
    setSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    audioProcessor.addChangeListener(this);

    mGeneralLookAndFeel = std::make_unique<GeneralLookAndFeel>();
    mButtonLookAndFeel = std::make_unique<ButtonLookAndFeel>();
    mTextEditorLookAndFeel = std::make_unique<TextEditorLookAndFeel>();

    juce::LookAndFeel::setDefaultLookAndFeel(mGeneralLookAndFeel.get());

    mCodeEditorPanel.setEditorLookAndFeel(mTextEditorLookAndFeel.get());
    mCodeEditorPanel.setFileOperationButtonsLookAndFeel(mButtonLookAndFeel.get());
    mCodeEditorPanel.applyDefaultStyling();

    mLogBox.setFont(MONOSPACE_FONT_OPTIONS);
    mLogBox.setColour(juce::TextEditor::textColourId, ORchestra::TextColor);
    mLogBox.setColour(juce::TextEditor::backgroundColourId, ORchestra::TextEditorBackgroundColor);
    mLogBox.setMultiLine(true);
    mLogBox.setEnabled(false);
    mLogBox.setLookAndFeel(mTextEditorLookAndFeel.get());

    mClearLogButton.setLookAndFeel(mButtonLookAndFeel.get());
    mClearLogButton.onClick = [this]() { handleClearLog(); };

    mSettingsButton.setLookAndFeel(mButtonLookAndFeel.get());
    mSettingsButton.onClick = [this]()
    {
#if JUCE_STANDALONE_APPLICATION
        if (auto* holder = juce::StandalonePluginHolder::getInstance())
        {
            auto comp = std::make_unique<MidiSettingsComponent>(holder->deviceManager);
            comp->setLookAndFeel(mGeneralLookAndFeel.get());
            comp->setButtonLookAndFeel(mButtonLookAndFeel.get());
            juce::CallOutBox::launchAsynchronously(std::move(comp),
                                                   mSettingsButton.getScreenBounds(),
                                                   nullptr);
        }
#endif
    };

    mCloseButton.setLookAndFeel(mButtonLookAndFeel.get());
    mCloseButton.onClick = []()
    {
#if JUCE_STANDALONE_APPLICATION
        juce::JUCEApplicationBase::quit();
#endif
    };

    mTimeline.SetProcessor(&audioProcessor);
    mTriggerRectangle.SetProcessor(&audioProcessor);

    mCodeEditorPanel.setPlayCallback([this]() { handlePlayButton(); });
    mCodeEditorPanel.setCompileCallback([this]() { handleCompile(); });
    mCodeEditorPanel.setExportCallback([this]() { handleExportFile(); });
    mCodeEditorPanel.setImportCallback([this]() { handleImportFile(); });

    audioProcessor.SetErrorListener(this);

    addAndMakeVisible(mCodeEditorPanel);
    addAndMakeVisible(mTimeline);
    addAndMakeVisible(mTriggerRectangle);
    addAndMakeVisible(mLogBox);
    addAndMakeVisible(mClearLogButton);
    addAndMakeVisible(mSettingsButton);
    addAndMakeVisible(mCloseButton);

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

void ORchestraAudioProcessorEditor::parentHierarchyChanged()
{
    juce::AudioProcessorEditor::parentHierarchyChanged();
    if (auto* w = dynamic_cast<juce::DocumentWindow*>(getTopLevelComponent()))
        if (w->getTitleBarHeight() > 0)
            w->setTitleBarHeight(0);
}

ORchestraAudioProcessorEditor::~ORchestraAudioProcessorEditor()
{
    audioProcessor.removeChangeListener(this);
    audioProcessor.SetErrorListener(nullptr);
    mLogBox.setLookAndFeel(nullptr);
    mClearLogButton.setLookAndFeel(nullptr);
    mSettingsButton.setLookAndFeel(nullptr);
    mCloseButton.setLookAndFeel(nullptr);
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
        mCodeEditorPanel.updatePlayButtonState(audioProcessor.IsRunning);
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
    const auto& errors = audioProcessor.GetErrors();
    juce::String mess;
    for (auto it = errors.rbegin(); it != errors.rend(); ++it)
    {
        mess.append(it->mMessage.data(), it->mMessage.size());
        mess.append("\n", 1);
    }
    mLogBox.setText(mess);
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

    // Vertical separator between code panel and timeline/log area
    const float sepX = static_cast<float>(mCodeEditorPanel.getRight()) + static_cast<float>(COMPONENT_MARGIN) / 2.0f;
    const float topY = static_cast<float>(mCodeEditorPanel.getY());
    const float botY = static_cast<float>(mCodeEditorPanel.getBottom());
    g.setColour(ORchestra::OutlineColor.brighter(0.3f));
    g.drawLine(sepX, topY, sepX, botY, static_cast<float>(SEPARATOR_WIDTH));
}

void ORchestraAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(OUTER_MARGIN);

    // Top-right corner buttons: [... settings] [X close]
    constexpr int topBtnW = 30;
    constexpr int topBtnH = 20;
    constexpr int topBtnSpacing = 4;
    const int topBtnY = bounds.getY();
    mCloseButton.setBounds(bounds.getRight() - topBtnW, topBtnY, topBtnW, topBtnH);
    mSettingsButton.setBounds(bounds.getRight() - topBtnW * 2 - topBtnSpacing, topBtnY, topBtnW, topBtnH);

    // Vertical split: left = code editor, right = timeline + log
    const int codeWidth = bounds.getWidth() * 2 / 5;
    auto codeBounds = bounds.removeFromLeft(codeWidth);
    bounds.removeFromLeft(COMPONENT_MARGIN);

    // Code editor panel fills entire left column
    mCodeEditorPanel.setBounds(codeBounds);

    // Right column: clear button at bottom, log above it, timeline fills the rest
    auto clearRow = bounds.removeFromBottom(CLEAR_BUTTON_HEIGHT);
    mClearLogButton.setBounds(clearRow.removeFromLeft(60));
    bounds.removeFromBottom(ROW_SPACING);
    mLogBox.setBounds(bounds.removeFromBottom(LOG_BOX_HEIGHT));
    bounds.removeFromBottom(ROW_SPACING);

    // Timeline fills remaining right area
    mTimeline.setBounds(bounds);
    mTriggerRectangle.setBounds(bounds.removeFromLeft(100));
}
