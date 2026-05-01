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

// Default script shown on first load when no saved state exists.
// Edit this string to change what new users see in the editor.
constexpr const char* DEFAULT_SCRIPT =
    "bpm(120)\n"
    "bpmDiv(n8)\n"
    "\n"
    "trig = euc(3, 8)\n"
    "\n"
    "note(trig, C4, 100, n8, 1)\n";

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

    mFileLabel.setText("File", juce::dontSendNotification);
    mFileLabel.setColour(juce::Label::textColourId, ORchestra::TextColor);
    addAndMakeVisible(mFileLabel);

    mImportButton.setColour(juce::TextButton::textColourOffId, ORchestra::TextColor);
    mImportButton.onClick = [this]() { if (mImportCallback) mImportCallback(); };
    addAndMakeVisible(mImportButton);

    mExportButton.setColour(juce::TextButton::textColourOffId, ORchestra::TextColor);
    mExportButton.onClick = [this]() { if (mExportCallback) mExportCallback(); };
    addAndMakeVisible(mExportButton);

    const int numInputs = mInputButtons.size();
    const int height = PADDING * 2
                     + LABEL_H + ROW_GAP
                     + numInputs * (ROW_H + ROW_GAP)
                     + SECTION_GAP
                     + LABEL_H + ROW_GAP
                     + ROW_H
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
    mImportButton.setLookAndFeel(nullptr);
    mExportButton.setLookAndFeel(nullptr);
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

    bounds.removeFromTop(SECTION_GAP);
    mFileLabel.setBounds(bounds.removeFromTop(LABEL_H));
    bounds.removeFromTop(ROW_GAP);
    auto fileRow = bounds.removeFromTop(ROW_H);
    const int btnW = (fileRow.getWidth() - ROW_GAP) / 2;
    mImportButton.setBounds(fileRow.removeFromLeft(btnW));
    fileRow.removeFromLeft(ROW_GAP);
    mExportButton.setBounds(fileRow);
}

void MidiSettingsComponent::setButtonLookAndFeel(juce::LookAndFeel* laf)
{
    for (auto* btn : mInputButtons)
        btn->setLookAndFeel(laf);
    mImportButton.setLookAndFeel(laf);
    mExportButton.setLookAndFeel(laf);
}

void MidiSettingsComponent::setImportCallback(std::function<void()> callback)
{
    mImportCallback = std::move(callback);
}

void MidiSettingsComponent::setExportCallback(std::function<void()> callback)
{
    mExportCallback = std::move(callback);
}

//==============================================================================
ORchestraAudioProcessorEditor::ORchestraAudioProcessorEditor(ORchestraAudioProcessor& p)
        : AudioProcessorEditor(&p),
          audioProcessor(p),
          mCodeEditorPanel(this),
          mTimeline(mTriggerRectangle)
{
    setResizable(true, true);
    setResizeLimits(900, 700, 99999, 99999);
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
    mSettingsButton.onClick = [&]()
    {
#if JUCE_STANDALONE_APPLICATION
        if (auto* holder = juce::StandalonePluginHolder::getInstance())
        {
            auto comp = std::make_unique<MidiSettingsComponent>(holder->deviceManager);
            comp->setLookAndFeel(mGeneralLookAndFeel.get());
            comp->setButtonLookAndFeel(mButtonLookAndFeel.get());
            comp->setImportCallback([this]() { handleImportFile(); });
            comp->setExportCallback([this]() { handleExportFile(); });
            juce::CallOutBox::launchAsynchronously(std::move(comp),
                                                   mSettingsButton.getScreenBounds(),
                                                   nullptr);
        }
#endif
    };

    mCloseButton.setLookAndFeel(mButtonLookAndFeel.get());
    mCloseButton.onClick = [this]()
    {
        UNUSED(this);
#if JUCE_STANDALONE_APPLICATION
        // Persist whatever is currently in the editor, compiled or not.
        const juce::String text = mCodeEditorPanel.getCodeDocument().getAllContent();
        audioProcessor.SetInstructionData(text.toStdString());

        if (auto* holder = juce::StandalonePluginHolder::getInstance())
            holder->savePluginState();

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

#if JUCE_STANDALONE_APPLICATION
    addAndMakeVisible(mSettingsButton);
    addAndMakeVisible(mCloseButton);
#endif

    const std::string& data = audioProcessor.GetInstructionData();
    const juce::String dataAsString = data.empty() ? juce::String(DEFAULT_SCRIPT)
                                                    : juce::String(data);
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

    if (auto* top = getTopLevelComponent())
        top->addKeyListener(this);
}

bool ORchestraAudioProcessorEditor::keyPressed(const juce::KeyPress& key, juce::Component*)
{
    const auto cmd = juce::ModifierKeys::commandModifier;

    if (key == juce::KeyPress('s', cmd, 0)) 
    { 
        handleCompile();    
        return true; 
    }

    if (key == juce::KeyPress('p', cmd, 0)) 
    { 
        handlePlayButton(); 
        return true; 
    }

    return false;
}

void ORchestraAudioProcessorEditor::mouseDown(const juce::MouseEvent& e)
{
    if (e.getPosition().getY() < OUTER_MARGIN * 2)
    {
        mIsDragging = true;
        mDragger.startDraggingComponent(getTopLevelComponent(), e);
    }
}

void ORchestraAudioProcessorEditor::mouseDrag(const juce::MouseEvent& e)
{
    if (mIsDragging)
        mDragger.dragComponent(getTopLevelComponent(), e, nullptr);
}

void ORchestraAudioProcessorEditor::mouseUp(const juce::MouseEvent&)
{
    mIsDragging = false;
}

ORchestraAudioProcessorEditor::~ORchestraAudioProcessorEditor()
{
    audioProcessor.removeChangeListener(this);
    audioProcessor.SetErrorListener(nullptr);
    if (auto* top = getTopLevelComponent())
        top->removeKeyListener(this);
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
    constexpr int exportFlags = juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles;
    mFileChooser.launchAsync(exportFlags, [this](const juce::FileChooser& fc)
        {
            UNUSED(fc);
            juce::File file = mFileChooser.getResult();
            if (file == juce::File{})
                return;
            std::string filePath{ file.getFullPathName().toRawUTF8() };
            audioProcessor.ExportToFile(filePath);
        });
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
    const float sepX = 
        static_cast<float>(mCodeEditorPanel.getRight()) + static_cast<float>(COMPONENT_MARGIN) / 2.0f;

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
