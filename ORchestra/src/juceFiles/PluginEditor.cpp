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

#include "LookAndFeelConstants.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "Colors.h"
#include "Utility.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_gui_extra/juce_gui_extra.h"

#if JUCE_STANDALONE_APPLICATION
#include "juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h"
#endif

constexpr int WINDOW_WIDTH = 1300;
constexpr int WINDOW_HEIGHT = 800;
constexpr int OUTER_MARGIN = 16;
constexpr int TITLE_BAR_HEIGHT = 35;
constexpr int TRAFFIC_DOT_SIZE = 12;

// Default script shown on first load when no saved state exists.
// Edit this string to change what new users see in the editor.
constexpr const char* DEFAULT_SCRIPT =
    "beat(n8)\n"
    "\n"
    "trig = euc(3, 8)\n"
    "\n"
    "note(trig, C4, 100, n8)\n";

//==============================================================================
// MidiSettingsComponent
//==============================================================================
MidiSettingsComponent::MidiSettingsComponent(juce::AudioDeviceManager& deviceManager)
    : mDeviceManager(deviceManager)
{
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

    mVersionLabel.setText(juce::String("v") + ProjectInfo::versionString,
                          juce::dontSendNotification);
    mVersionLabel.setColour(juce::Label::textColourId,
                            ORchestra::TextColor.withAlpha(0.5f));
    mVersionLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(mVersionLabel);

    const int height = PADDING * 2
                     + LABEL_H + ROW_GAP
                     + ROW_H
                     + SECTION_GAP
                     + LABEL_H + ROW_GAP
                     + ROW_H
                     + SECTION_GAP
                     + LABEL_H;

    setSize(PANEL_W, height);
}

MidiSettingsComponent::~MidiSettingsComponent()
{
    mOutputCombo.setLookAndFeel(nullptr);
    mImportButton.setLookAndFeel(nullptr);
    mExportButton.setLookAndFeel(nullptr);
}

void MidiSettingsComponent::resized()
{
    auto bounds = getLocalBounds().reduced(PADDING);

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

    bounds.removeFromTop(SECTION_GAP);
    mVersionLabel.setBounds(bounds.removeFromTop(LABEL_H));
}

void MidiSettingsComponent::setButtonLookAndFeel(juce::LookAndFeel* laf)
{
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

    int savedW = audioProcessor.GetEditorWidth();
    int savedH = audioProcessor.GetEditorHeight();
    int savedCodeW = audioProcessor.GetCodePanelWidth();
    int savedConsoleH = audioProcessor.GetConsoleHeight();
    mCodePanelWidth = savedCodeW > 0 ? savedCodeW : WINDOW_WIDTH * 2 / 5;
    mConsoleHeight = savedConsoleH > 0 ? savedConsoleH : ConsolePanel::GetPreferredHeight();

    setSize(savedW > 0 ? savedW : WINDOW_WIDTH,
            savedH > 0 ? savedH : WINDOW_HEIGHT);

    audioProcessor.addChangeListener(this);

    mGeneralLookAndFeel = std::make_unique<GeneralLookAndFeel>();
    mButtonLookAndFeel = std::make_unique<ButtonLookAndFeel>();
    mTransportLookAndFeel = std::make_unique<TransportLookAndFeel>();
    mTextEditorLookAndFeel = std::make_unique<TextEditorLookAndFeel>();

    juce::LookAndFeel::setDefaultLookAndFeel(mGeneralLookAndFeel.get());

    setColour(juce::ResizableWindow::backgroundColourId, BackgroundColor);

    mCodeEditorPanel.setEditorLookAndFeel(mTextEditorLookAndFeel.get());
    mCodeEditorPanel.applyDefaultStyling();

    mConsolePanel.applyDefaultStyling();
    mConsolePanel.setTextEditorLookAndFeel(mTextEditorLookAndFeel.get());
    mConsolePanel.setButtonLookAndFeel(mButtonLookAndFeel.get());
    mConsolePanel.setClearCallback([this]() { handleClearLog(); });

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
    mCloseButton.setColour(juce::TextButton::buttonColourId, juce::Colour(ColorPalette::Red).darker(0.2f));
    mCloseButton.setButtonText("");
    mCloseButton.onClick = [this]()
    {
        UNUSED(this);
#if JUCE_STANDALONE_APPLICATION
        // Persist whatever is currently in the editor, compiled or not.
        const juce::String text = mCodeEditorPanel.getCodeDocument().getAllContent();
        audioProcessor.SetInstructionData(text.toStdString());
        audioProcessor.SetEditorSize(getWidth(), getHeight());
        audioProcessor.SetCodePanelWidth(mCodePanelWidth);
        audioProcessor.SetConsoleHeight(mConsoleHeight);

        if (auto* holder = juce::StandalonePluginHolder::getInstance())
            holder->savePluginState();

        juce::JUCEApplicationBase::quit();
#endif
    };

    mTimeline.SetProcessor(&audioProcessor);
    mTriggerRectangle.SetProcessor(&audioProcessor);

    mFileOperationsToolbar.setPlayCallback([this]() { handlePlayButton(); });
    mFileOperationsToolbar.setCompileCallback([this]() { handleCompile(); });
    mFileOperationsToolbar.setTransportLookAndFeel(mTransportLookAndFeel.get());

    audioProcessor.SetErrorListener(this);

    addAndMakeVisible(mCodeEditorPanel);
    addAndMakeVisible(mFileOperationsToolbar);
    addAndMakeVisible(mTimeline);
    addAndMakeVisible(mTriggerRectangle);
    addAndMakeVisible(mConsolePanel);

#if JUCE_STANDALONE_APPLICATION
    addAndMakeVisible(mSettingsButton);
    addAndMakeVisible(mCloseButton);
#endif

    mVerticalDivider.onDragStart = [this]()
    {
        mDragStartCodeWidth = mCodePanelWidth;
    };

    mVerticalDivider.onDrag = [this](int delta)
    {
        mCodePanelWidth = juce::jlimit(200, getWidth() - 300, mDragStartCodeWidth + delta);
        resized();
    };

    mHorizontalDivider.onDragStart = [this]()
    {
        mDragStartConsoleHeight = mConsoleHeight;
    };

    mHorizontalDivider.onDrag = [this](int delta)
    {
        int minH = ConsolePanel::HEADER_HEIGHT + 50;
        int maxH = getHeight() - TITLE_BAR_HEIGHT - 150;
        mConsoleHeight = juce::jlimit(minH, maxH, mDragStartConsoleHeight - delta);
        resized();
    };

    addAndMakeVisible(mVerticalDivider);
    addAndMakeVisible(mHorizontalDivider);

    const std::string& data = audioProcessor.GetInstructionData();
    const juce::String dataAsString = data.empty() ? juce::String(DEFAULT_SCRIPT)
                                                    : juce::String(data);
    mCodeEditorPanel.loadContent(dataAsString);
    
    if (audioProcessor.IsORchestraVMInit())
    {
        mCodeEditorPanel.markSaved();
        mFileOperationsToolbar.setCompileButtonEnabled(false);
        UpdateErrors();
    }

    setWantsKeyboardFocus(true);
}

void ORchestraAudioProcessorEditor::parentHierarchyChanged()
{
    juce::AudioProcessorEditor::parentHierarchyChanged();
    if (auto* w = dynamic_cast<juce::DocumentWindow*>(getTopLevelComponent()))
    {
        if (w->getTitleBarHeight() > 0)
        {
            int desiredW = getWidth();
            int desiredH = getHeight();
            w->setTitleBarHeight(0);
            auto border = w->getContentComponentBorder();
            w->setSize(desiredW + border.getLeftAndRight(),
                       desiredH + border.getTopAndBottom());
        }
    }

#if JUCE_STANDALONE_APPLICATION
    if (auto* top = getTopLevelComponent())
    {
        mKeyListenerComponent = top;
        top->addKeyListener(this);
    }
#endif
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
    UNUSED(e);

#if JUCE_STANDALONE_APPLICATION
    if (e.getPosition().getY() < TITLE_BAR_HEIGHT)
    {
        if (auto* top = getTopLevelComponent())
        {
            mIsDragging = true;
            mDragger.startDraggingComponent(top, e);
        }
    }
#endif
}

void ORchestraAudioProcessorEditor::mouseDrag(const juce::MouseEvent& e)
{
    UNUSED(e);

#if JUCE_STANDALONE_APPLICATION
    if (mIsDragging)
        if (auto* top = getTopLevelComponent())
            mDragger.dragComponent(top, e, nullptr);
#endif
}

void ORchestraAudioProcessorEditor::mouseUp(const juce::MouseEvent&)
{
    mIsDragging = false;
}

ORchestraAudioProcessorEditor::~ORchestraAudioProcessorEditor()
{
    audioProcessor.SetEditorSize(getWidth(), getHeight());
    audioProcessor.SetCodePanelWidth(mCodePanelWidth);
    audioProcessor.SetConsoleHeight(mConsoleHeight);

    audioProcessor.removeChangeListener(this);
    audioProcessor.SetErrorListener(nullptr);

    if (mKeyListenerComponent != nullptr)
    {
        mKeyListenerComponent->removeKeyListener(this);
        mKeyListenerComponent = nullptr;
    }

    mFileOperationsToolbar.setTransportLookAndFeel(nullptr);
    mConsolePanel.setButtonLookAndFeel(nullptr);
    mConsolePanel.setTextEditorLookAndFeel(nullptr);
    mCloseButton.setLookAndFeel(nullptr);
}

void ORchestraAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster* broadCaster)
{
    UNUSED(broadCaster);

    const std::string& data = audioProcessor.GetInstructionData();

    if (!data.empty())
    {
        const juce::String dataAsString{ data };
        mCodeEditorPanel.loadContent(dataAsString);
    }

    int savedCodeW = audioProcessor.GetCodePanelWidth();
    int savedConsoleH = audioProcessor.GetConsoleHeight();

    if (savedCodeW > 0)
        mCodePanelWidth = savedCodeW;

    if (savedConsoleH > 0)
        mConsoleHeight = savedConsoleH;

    resized();
}

void ORchestraAudioProcessorEditor::CodeEditorHasChanged()
{
    if(mCodeEditorPanel.hasUnsavedChanges())
        mFileOperationsToolbar.setCompileButtonEnabled(true);
}

void ORchestraAudioProcessorEditor::handlePlayButton()
{
    if (mCodeEditorPanel.hasUnsavedChanges())
        handleCompile();
    
    if (audioProcessor.IsORchestraVMInit())
    {
        audioProcessor.IsRunning = !audioProcessor.IsRunning;
        mFileOperationsToolbar.updatePlayButtonState(audioProcessor.IsRunning);
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
        mTimeline.SetTimelineDirty(true);
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
    }

    mConsolePanel.setText(mess);
    mConsolePanel.setMessageCount((int)errors.size());
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
    mFileOperationsToolbar.setCompileButtonEnabled(mCodeEditorPanel.hasUnsavedChanges());

    // Title bar background
    g.setColour(juce::Colour(ColorPalette::Mantle));
    g.fillRect(0, 0, getWidth(), TITLE_BAR_HEIGHT);

    // Title bar bottom border
    g.setColour(ComponentOutlineColor);
    g.drawLine(0.f, static_cast<float>(TITLE_BAR_HEIGHT),
               static_cast<float>(getWidth()), static_cast<float>(TITLE_BAR_HEIGHT), OUTLINE_THICKNESS);

    // "OR | CHESTRA" centered in title bar
    {
        juce::Font titleFont(juce::FontOptions{ MONOSPACE_FONT_OPTIONS });
        g.setFont(titleFont);
        g.setColour(TextColor);
        g.drawText("OR | CHESTRA", 0, 0, getWidth(), TITLE_BAR_HEIGHT,
                   juce::Justification::centred, false);
    }
}

void ORchestraAudioProcessorEditor::paintOverChildren(juce::Graphics& g)
{
    // Vertical separator between code editor and timeline/console
    g.setColour(ComponentOutlineColor);
    g.drawLine(static_cast<float>(mCodePanelWidth),
               static_cast<float>(TITLE_BAR_HEIGHT),
               static_cast<float>(mCodePanelWidth),
               static_cast<float>(getHeight()));

    // Horizontal separator between timeline and console
    const int consoleTop = getHeight() - mConsoleHeight;
    g.setColour(ComponentOutlineColor);
    g.drawLine(static_cast<float>(mCodePanelWidth),
               static_cast<float>(consoleTop),
               static_cast<float>(getWidth()),
               static_cast<float>(consoleTop));
}

void ORchestraAudioProcessorEditor::resized()
{
    auto localBounds = getLocalBounds();

    // Title bar: single red close dot on left, settings button on right
    localBounds.removeFromTop(TITLE_BAR_HEIGHT);
    const int dotY = (TITLE_BAR_HEIGHT - TRAFFIC_DOT_SIZE) / 2;
    mCloseButton.setBounds(OUTER_MARGIN, dotY, TRAFFIC_DOT_SIZE, TRAFFIC_DOT_SIZE);

    constexpr int settingsBtnW = 28;
    constexpr int settingsBtnH = 18;
    mSettingsButton.setBounds(getWidth() - OUTER_MARGIN - settingsBtnW,
                              (TITLE_BAR_HEIGHT - settingsBtnH) / 2,
                              settingsBtnW, settingsBtnH);

    // Clamp stored sizes to current window dimensions
    mCodePanelWidth = juce::jlimit(200, getWidth() - 300, mCodePanelWidth);
    int minConsoleH = ConsolePanel::HEADER_HEIGHT + 50;
    int maxConsoleH = getHeight() - TITLE_BAR_HEIGHT - 150;
    mConsoleHeight = juce::jlimit(minConsoleH, maxConsoleH, mConsoleHeight);

    Rectangle<int> leftPanel = localBounds;
    Rectangle<int> rightPanel = localBounds;

    // Left panel: toolbar strip at bottom, code editor takes the rest
    constexpr int toolbarH = FileOperationsToolbar::BUTTON_HEIGHT + 40;
    auto toolbarStrip = leftPanel.removeFromBottom(toolbarH).removeFromLeft(mCodePanelWidth);
    mFileOperationsToolbar.setBounds(toolbarStrip);

    // Move code panel slightly down.
    leftPanel.removeFromTop(18);

    // Vertical split: left = code editor + toolbar, right = timeline + console
    auto codeBounds = leftPanel.removeFromLeft(mCodePanelWidth);
    mCodeEditorPanel.setBounds(codeBounds);

    rightPanel.removeFromLeft(mCodePanelWidth);
    // Right column: console section at bottom, timeline fills rest
    mConsolePanel.setBounds(rightPanel.removeFromBottom(mConsoleHeight));

    // Timeline fills remaining right area
    mTimeline.setBounds(rightPanel);
    mTriggerRectangle.setBounds(rightPanel.removeFromLeft(rightPanel.getWidth()));

    // Position divider hit areas on top of the lines
    mVerticalDivider.setBounds(mCodePanelWidth - 3, TITLE_BAR_HEIGHT,
                                6, getHeight() - TITLE_BAR_HEIGHT);

    int consoleTop = getHeight() - mConsoleHeight;
    mHorizontalDivider.setBounds(mCodePanelWidth, consoleTop - 3,
                                  getWidth() - mCodePanelWidth, 6);
}
